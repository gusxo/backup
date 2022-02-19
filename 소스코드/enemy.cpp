#include "enemy.h"
#include"gameManager.h"

void enemy::init() {
    waitInit = false;
    model_init({ "monkey" });
    model_scale[0] = { 0.5f, 0.5f, 0.5f };
    model_pos[0].y = 0.5f;
}

void enemy::update() {
    gameManager& gm = gameManager::instance();
    auto p = gm.pInstance();

    if (gm.gamestate == gm.gamestate_ingame) {
        //ȸ��
        vector3 turn_angle = { 0.0f, gm.real_delay * 30.0f, 0.0f };
        position = vector3::turn(position, vector3(), turn_angle);

        //���Ʒ��� ��鸲
        model_pos[0].y += updown_dir * gm.real_delay * 0.5f;
        if (model_pos[0].y >= 0.75f || model_pos[0].y <= 0.25f) {
            model_pos[0].y = 0.5f + 0.25f * updown_dir;
            updown_dir *= -1;
        }

        if (p) {
            lookAt(p->position, 0, 1, 0);
        }

        //���� ��Ÿ�� ����
        if (cooltime > 0.0f) {
            cooltime -= gm.real_delay;
        }

        //����
        if (cooltime <= 0.0f) {
            std::shared_ptr<object> missileObj = std::make_shared<defaultMissile>();
            std::uniform_int_distribution<int> cooltime_dis(cooltime_min, cooltime_max);

            //�̻��� �⺻����
            missileObj->position = position;
            missileObj->angle = angle;

            gm.addobj(missileObj);

            //��Ÿ�� �缳��
            cooltime = (float)(cooltime_dis(gm.gen)) / 1000.0f;
        }
    }
}