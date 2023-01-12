import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import math
from tensorflow.keras.utils import to_categorical

#https://stackoverflow.com/questions/10252412/matplotlib-varying-color-of-line-to-capture-natural-time-parameterization-in-da/10253183#10253183
# #plot line with color through time
from matplotlib.collections import LineCollection
def draw_gradation(x, y, axes, cmap_name="jet"):
    t = np.linspace(0, 1, x.shape[0])
    points = np.array([x, y]).transpose().reshape(-1, 1, 2)
    segs = np.concatenate([points[:-1],points[1:]],axis=1)
    # make the collection of segments
    cmap = plt.get_cmap(cmap_name)
    lc = LineCollection(segs, cmap=plt.get_cmap(cmap_name))
    lc.set_array(t) # color the segments by our parameter

    # plot the collection
    axes.add_collection(lc) # add the collection to the plot
    axes.set_xlim(x.min(), x.max()) # line collections don't auto-scale the plot
    axes.set_ylim(y.min(), y.max())

def fft_analysis(d, fs):
    #d에 대해 FFT 변환을 수행하고, 
    channel = d.shape[1]
    freqs = np.linspace(-fs / 2, fs / 2, len(d))
    fft_val = np.array([np.fft.fft(d[:,i]) / len(d) for i in range(0, channel)])
    fft_magnitude = abs(fft_val)
    amp = np.array([np.fft.fftshift(fft_magnitude[i]) for i in range(0, channel)])

    #주파수 값을 y(amp)값 기준으로 인덱스 번호를 desc 정렬
    argsorted = np.array([np.argsort(amp[i])[::-1] for i in range(0, channel)])

    return freqs, amp, argsorted

def fft_plot(d, fs, channel_list=None, xlim=(-32,32), ylim=(0,2.5)):
    if channel_list is None:
        channel_list = np.arange(0, d.shape[1])
    ch_n = len(channel_list)

    freq, amp, _ = fft_analysis(d, fs)
    amp_t = np.transpose(amp, [1,0])
    
    fig_x = min([4, ch_n])
    fig_y = int(math.ceil(ch_n/4))
    plt.figure(figsize=(4 * fig_x, 2 * fig_y))
    for i in range(ch_n):
        plt.subplot(fig_y, fig_x, i+1)
        plt.title(f"ch {channel_list[i]}")
        plt.xlim(xlim)
        plt.ylim(ylim)
        plt.stem(freq, amp_t[:,channel_list[i]])
    plt.tight_layout()
    plt.show()
    return

##########
# 표준화
# 채널별로 적용
# 파라미터에 따라 무시됨
##########

def z_score(x):
  for ch in range(x.shape[2]):
    mean = np.mean(x[:,:,ch])
    std = np.std(x[:,:,ch])
    x[:,:,ch] = (x[:,:,ch] - mean)/std
    print(f"channel {ch} mean : {mean} / std : {std}")
  return

##########
# 정규화 적용
# 채널별로 적용
# 파라미터에 따라 무시됨
##########

def min_max_scaler(x):
  for ch in range(x.shape[2]):
    minval = np.min(x[:,:,ch])
    maxval = np.max(x[:,:,ch])
    x[:,:,ch] = (x[:,:,ch] - minval)/(maxval - minval)
    print(f'channel {ch} value range : {minval} ~ {maxval}')
  return

##########
# 데이터 패턴 합치기
##########

def pattern_combine(x, y, pattern_list, except_processing="error"):
  if except_processing != "error" and except_processing != "drop":
    raise ValueError("excpet_processing = (error | drop)")

  new_x = np.copy(x)
  new_y = np.ones_like(y) * -1
  for new_p_n, pattern_numbers in enumerate(pattern_list):
    for p_n in pattern_numbers:
      new_y[y == p_n] = new_p_n

  # -1 값이 new_y에 있을 경우 처리...
  not_combined_values = (new_y == -1)
  not_combined_value_cnt = sum(not_combined_values)
  if not_combined_value_cnt > 0:
    if except_processing=="error":
      raise Exception("Not combined data exist!")
    elif except_processing=="drop":
      valid_data_mask = (not_combined_values == False)
      new_y = new_y[valid_data_mask]
      new_x = new_x[valid_data_mask]
  return new_x, new_y

def make_confusion_matrix(n, o, y):
#n : 행렬 크기
#o : 예측 결과 리스트
#y : 정답 리스트
  c_matrix = np.zeros([n,n]).astype(int)
  for i in range(y.shape[0]):
    idx_target = y[i]  #target
    idx_output = o[i]       #output
  
    #c_matrix 값 증가
    c_matrix[idx_target, idx_output] += 1
  
  return c_matrix

#행 : 예측된 패턴
#열 : 정답 패턴


##########
# 데이터 나누기 : 패턴별로 x 데이터 분류
# data_x : (pattern_id)(N, time_steps, channels)의 list.
##########
def data_separate_by_pattern(origin_x, origin_y, pattern_n, pattern_cnt):
  time_steps = origin_x.shape[1]
  channels = origin_x.shape[2]
  data_x = [np.zeros((pattern_cnt[i], time_steps, channels)) for i in range(pattern_n)]

  #패턴별 data_x에 저장해야할 인덱스 번호 저장용
  p_idx = np.zeros(pattern_n).astype(int)

  #분류 실행
  for i in range(origin_x.shape[0]):
    p_n = int(origin_y[i])
    data_x[p_n][p_idx[p_n], :, :] = origin_x[i, :, :]
    p_idx[p_n] += 1
  return data_x



##########
# 데이터 k 개의 segment로 나눔
##########
def data_separate_by_segment(data_x, pattern_n, k_fold, data_shuffle=True):
  patterns_per_segment = int(np.min([len(data_x[i]) for i in range(pattern_n)]) / k_fold)
  time_steps = data_x[0].shape[1]
  channels = data_x[0].shape[2]
  segment_len = patterns_per_segment * pattern_n
  print(f'segment 당 pattern 별 데이터 갯수 : {patterns_per_segment} / segment_len = {segment_len}')
  seg_x = np.zeros((k_fold, segment_len, time_steps, channels)).astype(float)
  seg_y_tmp = np.zeros((k_fold, segment_len))   #one-hot encoding 안한거 저장용
  seg_y = np.zeros((k_fold, segment_len, pattern_n))

  for p_n in range(pattern_n):
    n = len(data_x[p_n])
    print(f'pattern {p_n} len : {n} / 사용되지 않는 data 갯수 : {n - patterns_per_segment * k_fold}')

    if data_shuffle:
      order = np.arange(n)
      np.random.shuffle(order)
      data_x[p_n] = data_x[p_n][order]
    for seg_n in range(k_fold):
      seg_x[seg_n, p_n * patterns_per_segment : (p_n + 1) * patterns_per_segment, :, :] = data_x[p_n][seg_n * patterns_per_segment: (seg_n + 1 ) * patterns_per_segment, : ,:]
      seg_y_tmp[seg_n, p_n * patterns_per_segment : (p_n + 1) * patterns_per_segment] = p_n
      #debug msg
      # print(f"seg_x[{seg_n},{ p_n * patterns_per_segment} : {(p_n + 1) * patterns_per_segment}, :, :] = data_x[{p_n}][{seg_n * patterns_per_segment}: {(seg_n + 1 ) * patterns_per_segment}, : ,:]")
      # print(f"seg_y_tmp[{seg_n}, {p_n * patterns_per_segment} :{ (p_n + 1) * patterns_per_segment}] = {p_n}")

  for i in range(k_fold):
    seg_y[i] = to_categorical(seg_y_tmp[i])
  print(f'seg_x.shape : {seg_x.shape} / seg_y.shape : {seg_y.shape}')

  return seg_x, seg_y


def data_separate_by_train_val_test(data_x, pattern_n, train_val_test):
  if sum(train_val_test) != 100:
    raise Exception("파라미터 에러")

  time_steps = data_x[0].shape[1]
  channels = data_x[0].shape[2]

  train_len = [0] * pattern_n
  val_len = [0] * pattern_n
  test_len = [0] * pattern_n
  for pattern in range(pattern_n):
    datalen = len(data_x[pattern])
    train_len[pattern] = int(datalen * train_val_test[0] / 100)
    val_len[pattern] = int(datalen * (train_val_test[0] + train_val_test[1])/ 100) - train_len[pattern]
    test_len[pattern] = datalen - val_len[pattern] - train_len[pattern]
    print(f"패턴 {pattern} 총 갯수 : {datalen}\ntrain : val : test = {train_len[pattern]} : {val_len[pattern]} : {test_len[pattern]}")

  #real shuffle & assign
  train_x = np.zeros((sum(train_len), time_steps, channels)).astype(float)
  train_y = np.zeros(sum(train_len))
  val_x = np.zeros((sum(val_len), time_steps, channels)).astype(float)
  val_y = np.zeros(sum(val_len))
  test_x = np.zeros((sum(test_len), time_steps, channels)).astype(float)
  test_y = np.zeros(sum(test_len))
  print(f"train_x.shape = {train_x.shape} / val_x.shape = {val_x.shape} / test_x.shape = {test_x.shape}\n")

  train_start = 0
  val_start = 0
  test_start = 0
  for p in range(pattern_n):
    #셔플
    n = len(data_x[p])
    order = np.arange(n)
    np.random.shuffle(order)
    data_x[p] = data_x[p][order]
    #할당
    train_x[train_start:train_start+train_len[p], :, :] = data_x[p][0:train_len[p], :, :]
    train_y[train_start:train_start+train_len[p]] = p
    val_x[val_start:val_start+val_len[p], :, :] = data_x[p][train_len[p]:train_len[p]+val_len[p], :, :]
    val_y[val_start:val_start+val_len[p]] = p
    test_x[test_start:test_start+test_len[p], :, :] = data_x[p][train_len[p]+val_len[p]:, :, :]
    test_y[test_start:test_start+test_len[p]] = p

    #debug msg
    # print(f"train_x[{train_start} : {train_start+train_len[p]}] = data_x[{p}][{0} : {train_len[p]}]")
    # print(f"val_x[{val_start} : {val_start+val_len[p]}] = data_x[{p}][{train_len[p]} : {train_len[p]+val_len[p]}]")
    # print(f"test_x[{test_start} : {test_start+test_len[p]}] = data_x[{p}][{train_len[p]+val_len[p]} : {n}]")
    

    #다음 루프용 변수 정리
    train_start += train_len[p]
    val_start += val_len[p]
    test_start += test_len[p]


  #to categorical
  train_y = to_categorical(train_y)
  val_y = to_categorical(val_y)

  return train_x, train_y, val_x, val_y, test_x, test_y