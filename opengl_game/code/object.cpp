#include "gameManager.h"
#include "object.h"

void object::model_init(std::vector<std::string> modelnames)
{
	for (auto& name : modelnames) {
		auto pair = gameManager::instance().model_dictionary.find(name);
		auto n = pair->second;
		auto index = model_index.size();
		model_dictionary[pair->first] = index;
		model_index.push_back(n);
		model_pos.push_back(vector3());
		model_angle.push_back(vector3());
		model_scale.push_back(vector3({ 1.0f,1.0f,1.0f }));
	}
}

//object의 초기화 수행
void object::init() {
	waitInit = false;
}    

//object의 frame당 작동 정의
void object::update() {}   

//object에 속한 model들의 draw() 호출
void object::draw() {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle.x, 1, 0, 0);
	glRotatef(angle.y, 0, 1, 0);
	glRotatef(angle.z, 0, 0, 1);
	glScalef(scale.x, scale.y, scale.z);
	unsigned long long model_cnt = model_index.size();
	for (unsigned long long i = 0; i < model_cnt; ++i) {
		gameManager::instance().models[model_index[i]].draw(model_pos[i], model_angle[i], model_scale[i]);
	}
	glPopMatrix();
}
unsigned int object::getoid() { return oid; }
//target을 보도록 angle 변경
void object::lookAt(vector3 target, bool xAxis, bool yAxis, bool zAxis) {
	auto newangle = vector3::getAngle(position, target, xAxis, yAxis, zAxis);
	if (xAxis) {
		angle.x = newangle.x;
	}
	if (yAxis) {
		angle.y = newangle.y;
	}
	if (zAxis) {
		angle.z = newangle.z;
	}
}
void object::lookAt(vector3 target) {
	angle = vector3::getAngle(position, target, true, true, true);
}
unsigned int object::next_oid = 0;

