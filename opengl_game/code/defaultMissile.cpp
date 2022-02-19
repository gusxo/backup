#include "defaultMissile.h"
#include "gameManager.h"

void defaultMissile::update() {
    gameManager& gm = gameManager::instance();
    auto p = gm.pInstance();
    remaintime -= gm.real_delay;
    if (remaintime <= 0.0f) {
        gm.delobj(getoid());
        enable = false;
        return;
    }
    auto next_pos = vector3::turn({ position.x  ,position.y,position.z + (movespeed * gm.real_delay) }, position, { 0,angle.y, 0 });
    //알수없는 각도버그 방지용
    position.x = next_pos.x;
    position.y = 0.0f;
    position.z = next_pos.z;
    angle = { 0.0f, angle.y, 0.0f };
    //충돌 체크
    if (p) {
        float distance = sqrt(pow(position.x - p->position.x, 2) + pow(position.z - p->position.z, 2));
        if (distance < 0.2f) {
            gm.hitplayer();
            gm.delobj(getoid());
        }
    }
}

void defaultMissile::init() {
    waitInit = false;
    model_init({ "red_sphere" });
    model_scale[0] = { 0.1f, 0.1f, 0.1f };
}