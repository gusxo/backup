#pragma once
#include "object.h"
class summonEffect : public object
{
	unsigned int frame = 0;
public:
	void init();
	void update();
	std::shared_ptr<object> summon_object;	//이펙트 종료후 자동 배치(addobj)할 object 포인터
};

