#include "aes.h"

std::vector<unsigned char> aes::gen_roundkey(std::vector<unsigned char>& before_roundkey, int round)
{
    std::vector<unsigned char> roundkey(text_byte_length, 0);
    roundkey[0] = before_roundkey[colsize * 2 - 1];
    roundkey[colsize] = before_roundkey[colsize * 3 - 1];
    roundkey[colsize * 2] = before_roundkey[colsize * 4 - 1];
    roundkey[colsize * 3] = before_roundkey[colsize * 1 - 1];
	for (unsigned int i = 0; i < 4; ++i) {
		unsigned char row = roundkey[colsize * i] >> 4;
		unsigned char col = roundkey[colsize * i] & ((1 << 4) - 1);
		roundkey[colsize * i] = bytesub_table[row][col];
	}
    roundkey[0] ^= rcon[round];

    for (int i = 1; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            roundkey[colsize * j + i] = roundkey[colsize * j + i - 1] ^ before_roundkey[colsize * j + i - 1];
        }
    }
    return roundkey;
}

void aes::bytesub(std::vector<unsigned char>& matrix, unsigned char table[][16])
{
    for (unsigned int i = 0; i < text_byte_length; ++i) {
        unsigned char row = matrix[i] >> 4;
        unsigned char col = matrix[i] & ((1 << 4) - 1);
        matrix[i] = table[row][col];
   }

}

void aes::shift(std::vector<unsigned char>& matrix, int row, int rsh)
{
    if (rsh > 0) {
        for (int i = 0; i < rsh; ++i) {
            auto out = matrix[(row + 1) * colsize - 1];
            for (unsigned int j = 0; j < colsize - 1; ++j) {
                matrix[(row + 1) * colsize - j - 1] = matrix[(row + 1) * colsize - j - 2];
            }
            matrix[row * colsize] = out;
        }
    }
    else if (rsh < 0) {
        int lsh = rsh * -1;
        for (int i = 0; i < lsh; ++i) {
            auto out = matrix[row*colsize];
            for (unsigned int j = 0; j < colsize - 1; ++j) {
                matrix[row * colsize + j] = matrix[row * colsize + j + 1];
            }
            matrix[(row + 1) * colsize - 1] = out;
        }
    }

}

void aes::shiftrow(std::vector<unsigned char>& matrix)
{
    shift(matrix, 0, 0);
    shift(matrix, 1, 3);
    shift(matrix, 2, 2);
    shift(matrix, 3, 1);
}

void aes::shiftrow_inverse(std::vector<unsigned char>& matrix)
{
    shift(matrix, 0, 0);
    shift(matrix, 1, -3);
    shift(matrix, 2, -2);
    shift(matrix, 3, -1);
}

unsigned char aes::gmul(unsigned char lookup_target, unsigned char x) {
    switch (lookup_target)
    {
    case 2:
        return mixcolumn_lookuptable[0][x];
    case 3:
        return mixcolumn_lookuptable[1][x];
    case 9:
        return mixcolumn_lookuptable[2][x];
    case 11:
        return mixcolumn_lookuptable[3][x];
    case 13:
        return mixcolumn_lookuptable[4][x];
    case 14:
        return mixcolumn_lookuptable[5][x];
    }
    return x;
}

void aes::mixcolumn(std::vector<unsigned char>& matrix, unsigned char table[][4])
{
    for (unsigned int i = 0; i < colsize; ++i) {
        unsigned char col[4] = { matrix[i], matrix[colsize * 1 + i], matrix[colsize * 2 + i], matrix[colsize * 3 + i] };
        for (int j = 0; j < 4; ++j) {
            matrix[colsize * j + i] = gmul(table[j][0], col[0]) ^ gmul(table[j][1], col[1]) ^ gmul(table[j][2], col[2]) ^ gmul(table[j][3], col[3]);
        }
    }
}

void aes::addroundkey(std::vector<unsigned char>& matrix, int round)
{
    for (unsigned int i = 0; i < text_byte_length; ++i) {
        matrix[i] ^= roundkeys[round][i];
    }
}

void aes::gen_inverse_table()
{
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            unsigned char row = (bytesub_table[i][j]) >> 4;
            unsigned char col = bytesub_table[i][j] & ((1 << 4) - 1);
            bytesub_inverse_table[row][col] = (i << 4) | (j);
        }
    }
}

void aes::setmode(std::string mode)
{
    if (mode == "ecb") {
        is_cbcmode = false;
    }
    else if (mode == "cbc") {
        is_cbcmode = true;
        if (IV.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(0, UINT8_MAX);
            std::vector<unsigned char> randIV;
            for (unsigned int i = 0; i < text_byte_length; ++i) {
                randIV.push_back((unsigned char)dis(gen));
            }
            setIV(randIV);
        }
    }
    else {
        throw ("mode : 'ecb' or 'cbc' 만 허용");
    }
}

aes::aes(std::string mode)
{
    setmode(mode);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, UINT8_MAX);
    std::vector<unsigned char> randkey;
    for (int i = 0; i < 16; ++i) {
        randkey.push_back((unsigned char)dis(gen));
    }
    gen_inverse_table();
    setkey(randkey);
}

aes::aes(std::string mode, std::vector<unsigned char>& key)
{
    setmode(mode);
    gen_inverse_table();
    setkey(key);
}

aes::aes(std::vector<unsigned char>& cbc_IV, std::vector<unsigned char>& key)
{
    setIV(cbc_IV);
    setmode("cbc");
    gen_inverse_table();
    setkey(key);
}

std::vector<unsigned char> aes::encrypto(std::vector<unsigned char>& plainbytes, int& out_padding)
{
    unsigned int plainsize = plainbytes.size();
    unsigned int ciphersize = ((plainsize - 1) / text_byte_length + 1) * text_byte_length;
    out_padding = ciphersize - plainsize;
    std::vector<unsigned char> cipherbytes(ciphersize, 0);
    std::vector<unsigned char> matrix(text_byte_length, 0);
    for (unsigned int i = 0; i < plainsize; i += text_byte_length) {
        for (unsigned int j = 0; j < text_byte_length; ++j) {
            if (i + j >= plainsize) {
                break;
            }
            matrix[j] = plainbytes[i + j];
        }

        //cbc mode일경우 전처리
        if (is_cbcmode) {
            //처음은 IV사용
            if (!i) {
                for (unsigned int k = 0; k < text_byte_length; ++k) {
                    matrix[k] ^= IV[k];
                }
            }
            else {
                for (unsigned int k = 0; k < text_byte_length; ++k) {
					matrix[k] ^= cipherbytes[i + k - text_byte_length];
                }
            }
        }

        addroundkey(matrix, 0);
        for (int round = 1; round < 11; ++round) {
            bytesub(matrix, bytesub_table);
            shiftrow(matrix);
            if (round != 10) {
                mixcolumn(matrix, mixcolumn_table);
            }
            addroundkey(matrix, round);
        }
        for (unsigned int j = 0; j < text_byte_length; ++j) {
            cipherbytes[i + j] = matrix[j];
        }
    }
    return cipherbytes;
}

void aes::encrypto_file(const char* inputfile, const char* outputfile, int& out_padding)
{
    std::fstream in, out;
    in.open(inputfile, std::ios::in | std::ios::binary | std::ios::ate);
    if (!in.is_open()) {
        throw ("입력 파일 열기 실패\n");
    }
    out.open(outputfile, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        throw ("출력 파일 열기 실패\n");
    }
    auto size = in.tellg();
    char* buf = new char[size];
    in.seekg(0, std::ios::beg);
    in.read(buf, size);
    std::vector<unsigned char> bytes(buf, buf + size);
    delete[] buf;
    auto e = encrypto(bytes, out_padding);
    buf = new char[e.size()];
    for (int i = 0; i < e.size(); ++i) {
        buf[i] = e[i];
    }
    out.write(buf, e.size());
    delete[] buf;

    in.close();
    out.close();
}

std::vector<unsigned char> aes::decrypto(std::vector<unsigned char>& cipherbytes, int padding)
{
    unsigned int ciphersize = cipherbytes.size();
    unsigned int plainsize = ciphersize - padding;
    std::vector<unsigned char> plainbytes(plainsize, 0);
    std::vector<unsigned char> matrix(text_byte_length, 0);
    for (unsigned int i = 0; i < ciphersize; i += text_byte_length) {
        for (unsigned int j = 0; j < text_byte_length; ++j) {
            matrix[j] = cipherbytes[i + j];
        }
        for (int round = 10; round > 0; --round) {
            addroundkey(matrix, round);
            if (round != 10) {
                mixcolumn(matrix, mixcolumn_inverse_table);
            }
            shiftrow_inverse(matrix);
            bytesub(matrix, bytesub_inverse_table);
        }
        addroundkey(matrix, 0);

        //cbc mode일경우 후처리
        if (is_cbcmode) {
            //처음은 IV사용
            if (!i) {
                for (unsigned int k = 0; k < text_byte_length; ++k) {
                    matrix[k] ^= IV[k];
                }
            }
            else {
                for (unsigned int k = 0; k < text_byte_length; ++k) {
                    matrix[k] ^= cipherbytes[i + k - text_byte_length];
                }
            }
        }
        for (unsigned int j = 0; j < text_byte_length; ++j) {
            if (i + j >= plainsize) {
                break;
            }
            plainbytes[i + j] = matrix[j];
        }
    }
    return plainbytes;
}

void aes::decrypto_file(const char* inputfile, const char* outputfile, int padding)
{
    std::fstream in, out;
    in.open(inputfile, std::ios::in | std::ios::binary | std::ios::ate);
    if (!in.is_open()) {
        throw ("입력 파일 열기 실패\n");
    }
    out.open(outputfile, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        throw ("출력 파일 열기 실패\n");
    }
    auto size = in.tellg();
    char* buf = new char[size];
    in.seekg(0, std::ios::beg);
    in.read(buf, size);
    std::vector<unsigned char> bytes(buf, buf + size);
    delete[] buf;
    auto d = decrypto(bytes, padding);
    buf = new char[d.size()];
    for (int i = 0; i < d.size(); ++i) {
        buf[i] = d[i];
    }
    out.write(buf, d.size());
    delete[] buf;

    in.close();
    out.close();
}

void aes::loadsetting(const char* filename)
{
    std::fstream in;
    in.open(filename, std::ios::in);
    if (!in.is_open()) {
        throw ("입력 파일 열기 실패\n");
    }
    std::string line;
    std::getline(in, line);
    auto key = hexstr_to_bytes(line);
    setkey(key);
    if (is_cbcmode) {
        std::getline(in, line);
        auto IV = hexstr_to_bytes(line);
        setIV(IV);
    }
    in.close();
}

void aes::savesetting(const char* filename)
{
    std::fstream out;
    out.open(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        throw ("출력 파일 열기 실패\n");
    }
    out << bytes_to_hexstr(getkey()) << "\n";
    if (is_cbcmode) {
        out << bytes_to_hexstr(getIV()) << "\n";
    }
    out.close();
}

std::vector<unsigned char>& aes::getkey()
{
    return key;
}

void aes::setkey(std::vector<unsigned char>& key)
{
    int size = key.size();
    if (size != 16) {
        throw ("key length : only supports 16byte");
    }
    this->key = key;
    key_byte_length = size;
    roundkeys.clear();
    roundkeys.push_back(gen_roundkey(this->key, 0));
    for (int i = 1; i < 11; ++i) {
        roundkeys.push_back(gen_roundkey(roundkeys[i-1], i));
    }
}

std::vector<unsigned char>& aes::getIV()
{
    if (!is_cbcmode) {
        throw ("cbc 모드가 아닙니다.");
    }
    return IV;
}

void aes::setIV(std::vector<unsigned char>& IV)
{
    if (!is_cbcmode || IV.size() != text_byte_length) {
        throw ("cbc 모드가 아니거나 IV length가 block 단위와 일치하지 않습니다");
    }
    this->IV = IV;
}
