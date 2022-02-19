#include "summonEffect.h"
#include"gameManager.h"

void summonEffect::init()
{
	waitInit = 0;
	model_init({ "summon_effect" });
}

void summonEffect::update()
{
	frame++;
	if (frame <= 30) {
		float rate = (float)frame / 30.0f;
		model_pos[0].y =  rate;
		model_scale[0].y = rate;
	}
	else if (frame <= 60) {
		float rate = (float)(frame - 30) / 30.0f;
		model_pos[0].y = 1.0f + rate;
		model_scale[0].y = 1.0f - rate;
	}
	else {
		gameManager::instance().delobj(getoid());	//self destory
		enable = false;
	}

	//�ڽ� ����Ʈ�� �پ��� �����Ҷ� ��ȯ
	if (frame == 26) {
		if (summon_object) {
			gameManager::instance().addobj(summon_object);
		}
	}
}
