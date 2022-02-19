#include "enemyTarget.h"
#include "gameManager.h"

void enemyTarget::update() {
    if (hp <= 0) {
        //사망 상태에선 회전안함
        return;
    }
    angle.y += gameManager::instance().real_delay * 30.0f;
    if (angle.y >= 360.0f) {
        angle.y -= 360.0f;
    }
}

void enemyTarget::init() {
    waitInit = false;
    model_init({ "pylon" });
    model_scale[0] = { 0.5f, 0.5f, 0.5f };
    //model_pos[0].y = 0.25f;
}