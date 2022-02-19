#include "explosion.h"
#include"gameManager.h"

void explosion::init()
{
	waitInit = 0;
	std::vector<std::string> models(12, "explosion_effect");
	model_init(models);
	std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<int> dis(-1000,1000);
	for (int i = 0; i < 12; ++i) {
		model_pos[i].x = (float)dis(gen) / 1000.0f;
		model_pos[i].y = (float)dis(gen) / 1000.0f;
		model_pos[i].z = (float)dis(gen) / 1000.0f;
	}
}

void explosion::update()
{
	frame++;
	for (int i = 0; i < 12; ++i) {
		//실제 표시 기간 : range_begin + 1 ~ range_end   frame일때
		int range_begin = i * 10;
		int range_end = range_begin + 25;
		if (frame > range_begin && frame <= range_end) {
			float rate = (float)(frame - range_begin) / (range_end - range_begin);
			model_scale[i] = { rate, rate, rate };
		}
		else {
			model_scale[i] = vector3();
		}
	}
	//self destory
	if (frame > 200) {
		gameManager::instance().delobj(getoid());
		enable = 0;
	}
}
