#include "arrow.h"
#include "gameManager.h"

void arrow::update() {
    gameManager& gm = gameManager::instance();
    auto et = gm.etInstance();
    remaintime -= gm.real_delay;
    if (remaintime <= 0.0f) {
        gm.delobj(getoid());
        enable = false;
        return;
    }
    position = vector3::turn({ position.x  ,position.y,position.z + (movespeed * gm.real_delay) }, position, { 0,angle.y, 0 });
    //충돌 체크
    if (et) {
        float distance = sqrt(pow(position.x - et->position.x, 2) + pow(position.z - et->position.z, 2));
        if (distance < 0.75f) {
            gm.hittarget();
            gm.delobj(getoid());
        }
    }
}

void arrow::init() {
    waitInit = 0;
    model_init({ "black_sphere" });
    model_scale[0] = { 0.1f, 0.1f, 0.1f };

}