#include "camera.h"
#include "gameManager.h"

void camera::update() {
    //gameManager�� player ���� �޾ƿ�
    gameManager& gm = gameManager::instance();
    auto p = gm.pInstance();
    if (gm.gamestate == gm.gamestate_loading && position.z > 7.0f) {
        float diff = position.z - 9.0f;
        //�����Ÿ��� 150%/s �ӵ��� �̵��ϸ�, �ش� �ӵ��� 2.0f/s ���� ���� �� ����
        float movedistance = (diff * 1.5f > 2.0f ? diff * 1.5f : 2.0f) * gm.real_delay;
        position.z -= movedistance;
        if (position.z <= 9.0f) {
            position.z = 9.0f;
        }
    }

    if (gm.gamestate == gm.gamestate_main || gm.gamestate == gm.gamestate_loading) {
        //���� ���� ���Ͻ�, �⺻ ��ȯ ��ġ�� �ٶ󺻴�.
        lookAt({ 0.0f, 0.0f, 4.0f }, 1, 1, 0);
        angle.x += 180.0f;
        return;
    }
    

    if (gm.gamestate == gm.gamestate_ingame || gm.gamestate == gm.gamestate_gameover) {
        //���� ���߿���, ĳ���͸� ��� ���󰣴�.
        position = { p->position.x, gm.camZoomIn * 1.5f, p->position.z + gm.camZoomIn };
        //ī�޶� ���� ����
        lookAt({p->position.x, 0.0f, p->position.z}, 1, 1, 0);
        angle.x += 180.0f;
    }
}

void camera::draw() {
    vector3 target = vector3::turn({ position.x , position.y, position.z + 1 }, position, angle);
    gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
}