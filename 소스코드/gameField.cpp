#include "gameField.h"
#include "gameManager.h"

void gameField::update() {
	gameManager& gm = gameManager::instance();

	//바깥 고리 공전시키기
	model_angle[2].x += gm.real_delay * 75.0f;
	model_angle[3].x -= gm.real_delay * 60.0f;
	model_angle[4].z += gm.real_delay * 45.0f;
	if (model_angle[2].x >= 360.0f) {
		model_angle[2].x -= 360.0f;
	}
	if (model_angle[3].x <= -360.0f) {
		model_angle[3].x += 360.0f;
	}
	if (model_angle[4].z >= 360.0f) {
		model_angle[4].z -= 360.0f;
	}

	if (gm.gamestate != gm.gamestate_ingame) {
		return;
	}

	//안쪽 고리 & 블록 돌리기
	float turn_angle = gm.real_delay * 30.0f;
	model_angle[1].y += turn_angle;
	for (long long i = 0; i < 12; ++i) {
		model_pos[i + 5] = vector3::turn(model_pos[i + 5], vector3(), { 0.0f, turn_angle, 0.0f });
	}
}

void gameField::init() {
	waitInit = 0;
	std::vector<std::string> model_list(17, "block");
	model_list[0] = "hollow_cylinder";
	model_list[1] = "gori";
	model_list[2] = "gori";
	model_list[3] = "gori";
	model_list[4] = "gori";
	//models : {main body, 가장 안쪽 고리, 2~4번 고리, 12개의 blocks(가장 안쪽 고리에 붙임)}
	model_init(model_list);
	position.y = -0.75f;
	scale = { 5.0f, 5.0f, 5.0f };
	model_scale[1] = { 1.3f, 1.3f, 1.3f };
	model_scale[2] = { 1.6f, 1.6f, 1.6f };
	model_scale[3] = { 1.7f, 1.7f, 1.7f };
	model_scale[4] = { 1.8f, 1.8f, 1.8f };
	model_angle[2].z = 45.0f;
	model_angle[3].z = -45.0f;
	vector3 block_pos = { 0.0f, 0.0f, model_scale[1].z };
	for (long long i = 0; i < 12; ++i) {
		model_pos[i + 5] = vector3::turn(block_pos, { 0.0f, 0.0f, 0.0f } , { 0.0f, 30.0f * (float)i, 0.0f });
	}
}