#pragma once
#include "object.h"
class summonEffect : public object
{
	unsigned int frame = 0;
public:
	void init();
	void update();
	std::shared_ptr<object> summon_object;	//����Ʈ ������ �ڵ� ��ġ(addobj)�� object ������
};

