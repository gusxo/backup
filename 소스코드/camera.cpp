#include "camera.h"
#include "gameManager.h"

void camera::update() {
    //gameManager와 player 정보 받아옴
    gameManager& gm = gameManager::instance();
    auto p = gm.pInstance();
    if (gm.gamestate == gm.gamestate_loading && position.z > 7.0f) {
        float diff = position.z - 9.0f;
        //남은거리의 150%/s 속도로 이동하며, 해당 속도가 2.0f/s 보다 작을 시 보정
        float movedistance = (diff * 1.5f > 2.0f ? diff * 1.5f : 2.0f) * gm.real_delay;
        position.z -= movedistance;
        if (position.z <= 9.0f) {
            position.z = 9.0f;
        }
    }

    if (gm.gamestate == gm.gamestate_main || gm.gamestate == gm.gamestate_loading) {
        //게임 시작 전일시, 기본 소환 위치를 바라본다.
        lookAt({ 0.0f, 0.0f, 4.0f }, 1, 1, 0);
        angle.x += 180.0f;
        return;
    }
    

    if (gm.gamestate == gm.gamestate_ingame || gm.gamestate == gm.gamestate_gameover) {
        //게임 도중에는, 캐릭터를 계속 따라간다.
        position = { p->position.x, gm.camZoomIn * 1.5f, p->position.z + gm.camZoomIn };
        //카메라 각도 변경
        lookAt({p->position.x, 0.0f, p->position.z}, 1, 1, 0);
        angle.x += 180.0f;
    }
}

void camera::draw() {
    vector3 target = vector3::turn({ position.x , position.y, position.z + 1 }, position, angle);
    gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
}