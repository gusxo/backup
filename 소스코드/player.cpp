#include "player.h"
#include "gameManager.h"

const int player::IDLE = 0;
const int player::MOVE = 1;
const int player::ATTACK = 2;
const int player::DIE = 3;

void player::update() {
    gameManager& gm = gameManager::instance();
    
    if (gm.gamestate != gm.gamestate_ingame) {
        if (hp <= 0) {
            ani_state = DIE;
        }
        if (ani_state == MOVE) {
            ani_state = IDLE;
        }
        ani();
        return;
    }

    //공격 - 시작
    if ((gm.iflag & gameManager::MOUSE_LEFT) && (ani_state == IDLE || ani_state == MOVE)) {
        ani_state = ATTACK;
        waitshot = true;
        ani_timer = 0.0f;
        vector3 mid = { (float)gm.currentWindowWidth / 2.0f, (float)gm.currentWindowHeight / 2.0f,0 };
        vector3 clickpoint = { (float)gm.mouse_x, (float)gm.mouse_y,0 };
        angle.y = vector3::getAngle(mid, clickpoint, 0, 0, 1).z;
    }
    //공격 - 총알 발사
    if (waitshot && ani_state == ATTACK && ani_timer > 0.4f) {
        auto missile = std::make_shared<arrow>();
        missile->angle = angle;
        missile->position = vector3::turn({ position.x, position.y + 0.1f, position.z + 0.3f }, position, { 0.0f, angle.y, 0.0f });
        gm.addobj(missile);
        waitshot = false;
    }

    //moving part
    xv = (gm.iflag & gameManager::KEY_LEFT ? -1 : 0) + (gm.iflag & gameManager::KEY_RIGHT ? 1 : 0); //좌우 이동벡터
    zv = (gm.iflag & gameManager::KEY_DOWN ? 1 : 0) + (gm.iflag & gameManager::KEY_UP ? -1 : 0); //상하 이동벡터

    //실제 이동
    if ((xv || zv) && (ani_state == IDLE || ani_state == MOVE)) {
        ani_state = MOVE;
        lookAt({ position.x + xv , position.y, position.z + zv }, 0, 1, 0);
        vector3 movepos = vector3::turn({ position.x  ,position.y,position.z + (movespeed * gm.real_delay) }, position, { 0,angle.y, 0 });
        //맵 범위 밖인지 체크
        float distance = (float)sqrt(pow(movepos.x, 2) + pow(movepos.z, 2));
        if (distance > 1 && distance < 5) {
            position = movepos;
        }
    }
    else if (!(xv || zv) && (ani_state == IDLE || ani_state == MOVE)) {
        ani_state = IDLE;
    }

    ani();
}

void player::ani() {
    gameManager& gm = gameManager::instance();

    //animation 처리 파트
    //init OR increment timer
    if (ani_state != recent_ani_state) {
        ani_timer = 0.0f;
    }
    else {
        ani_timer += gm.real_delay;
    }
    //update recent_ani_state
    recent_ani_state = ani_state;

    //IDLE
    //NO Animation
    if (ani_state == IDLE) {
        for (int i = 0; i < 15; ++i) {
            model_angle[i] = vector3();
            model_pos[i] = vector3();
        }
    }
    //MOVE
    else if (ani_state == MOVE) {
        //애니메이션 진행에 따른 변수 변화 표
        //    *아래 표 중 상체에 해당하는 부분은 상체 기울이기가 적용된 수치
        //    
        // 
        //           /      phase 1     /       phase 2     /       phase 3     /       phase 4     /
        // RUA angle /  {-15, 0, 0}     /   {30, 0, 0}      /   {45, 0, 0}      /   {30, 0, 0}      /
        //     pos   /  {0, -0.25, 2.5} /   {0, 0, 0}       /   {0, 0.5, -0.75} /   {0, 0, 0}       /
        // RMA angle /  {-15, 0, 0}     /   {30, 0, 0}      /   {45, 0, 0}      /   {30, 0, 0}      / 
        //     pos   /  {0, -0.25, 2.5} /   {0, 0, 0}       /   {0, 0.5, -0.75} /   {0, 0, 0}       /
        // RDA angle /  {-105, 0, 0}    /   {-60, 0, 0}     /   {135, 0, 0}     /   {-60, 0, 0}     /
        //     pos   /  {0, 1.5, 3.5}   /   {0, 0.5, 2}     /   {0, 1.25, 0.5}  /   {0, 0.5, 2}     /
        // LUA angle /  {45, 0, 0}      /   {30, 0, 0}      /   {-15, 0, 0}     /   {30, 0, 0}      / 
        //     pos   /  {0, 0.5, -0.75} /   {0, 0, 0}       /   {0, -0.25, 2.5} /   {0, 0, 0}       /
        // LMA angle /  {45, 0, 0}      /   {30, 0, 0}      /   {-15, 0, 0}     /   {30, 0, 0}      / 
        //     pos   /  {0, 0.5, -0.75} /   {0, 0, 0}       /   {0, -0.25, 2.5} /   {0, 0, 0}       /
        // LDA angle /  {135, 0, 0}     /   {-60, 0, 0}     /   {-105, 0, 0}    /   {-60, 0, 0}     /
        //     pos   /  {0, 1.25, 0.5}  /   {0, 0.5, 2}     /   {0, 1.5, 3.5}   /   {0, 0.5, 2}     /
        // RUL angle /  {30, 0, 0}      /   {0, 0, 0}       /   {-45, 0, 0}     /   {-30, 0, 0}     /
        //     pos   /  {0, 0, 0}       /   {0, 0, -0.5}    /   {0, 0, -1.5}    /   {0, 0, -1.5}    /
        // RML angle /  {30, 0, 0}      /   {0, 0, 0}       /   {-45, 0, 0}     /   {-30, 0, 0}     /
        //     pos   /  {0, 0, 0}       /   {0, 0, -0.5}    /   {0, 0, -1.5}    /   {0, 0, -1.5}    /
        // RDL angle /  {75, 0, 0}      /   {45, 0, 0}      /   {-15, 0, 0}     /   {30, 0, 0}      /
        //     pos   /  {0, -2, 1.5}    /   {0, -1, 2}      /   {0, 1, 0}       /   {0, 0, 2}       /
        // LUL angle /  {-45, 0, 0}     /   {-30, 0, 0}     /   {30, 0, 0}      /   {0, 0, 0}       /
        //     pos   /  {0, 0, -1.5}    /   {0, 0, -1.5}    /   {0, 0, 0}       /   {0, 0, -0.5}    /
        // LML angle /  {-45, 0, 0}     /   {-30, 0, 0}     /   {30, 0, 0}      /   {0, 0, 0}       /
        //     pos   /  {0, 0, -1.5}    /   {0, 0, -1.5}    /   {0, 0, 0}       /   {0, 0, -0.5}    /
        // LDL angle /  {-15, 0, 0}     /   {30, 0, 0}      /   {75, 0, 0}      /   {45, 0, 0}      /
        //     pos   /  {0, 1, 0}       /   {0, 0, 2}       /   {0, -2, 1.5}    /   {0, -1, 2}      /
        // gun angle /  {-105, 0, 0}    /   {-60, 0, 0}     /   {-45, 0, 0}     /   {-60, 0, 0}     /
        //     pos   /  {0, 1.5, 3.5}   /   {0, 0.5, 2}     /   {0, 0, 0.75}    /   {0, 0.5, 2}     /
        // head angle/  {30, 0, 0}      /                   /
        //      pos  /                  /                   /
        // body angle/  {30, 0, 0}      /                   /
        //      pos  /                  /                   /

        while (ani_timer >= 0.4f) {
            ani_timer -= 0.4f;
        }
        int phase = (int)floor(ani_timer / 0.1f);
        model_angle[0].x = model_angle[1].x = 30.0f;
        if (phase == 0) {
            model_angle[4] = { -15.0f, 0.0f, 0.0f };
            model_pos[4] = { 0.0f, -0.25f, 2.5f };
            model_angle[3] = { -15.0f, 0.0f, 0.0f };
            model_pos[3] = { 0.0f, -0.25f, 2.5f };
            model_angle[2] = { -105.0f, 0.0f, 0.0f };
            model_pos[2] = { 0.0f, 1.5f, 3.5f };
            model_angle[7] = { 45.0f, 0.0f, 0.0f };
            model_pos[7] = { 0.0f, 0.5f, -0.75f };
            model_angle[6] = { 45.0f, 0.0f, 0.0f };
            model_pos[6] = { 0.0f, 0.5f, -0.75f };
            model_angle[5] = { 135.0f, 0.0f, 0.0f };
            model_pos[5] = { 0.0f, 1.25f, 0.5f };
            model_angle[10] = { 30.0f, 0.0f, 0.0f };
            model_pos[10] = { 0.0f, 0.0f, 0.0f };
            model_angle[9] = { 30.0f, 0.0f, 0.0f };
            model_pos[9] = { 0.0f, 0.0f, 0.0f };
            model_angle[8] = { 75.0f, 0.0f, 0.0f };
            model_pos[8] = { 0.0f, -2.0f, 1.5f };
            model_angle[13] = { -45.0f, 0.0f, 0.0f };
            model_pos[13] = { 0.0f, 0.0f, -1.5f };
            model_angle[12] = { -45.0f, 0.0f, 0.0f };
            model_pos[12] = { 0.0f, 0.0f, -1.5f };
            model_angle[11] = { -15.0f, 0.0f, 0.0f };
            model_pos[11] = { 0.0f, 1.0f, 0.0f };
            model_angle[14] = { -105.0f, 0.0f, 0.0f };
            model_pos[14] = { 0.0f, 1.5f, 3.5f };
        }
        else if (phase == 1) {
            model_angle[4] = { 30.0f, 0.0f, 0.0f };
            model_pos[4] = { 0.0f, 0.0f, 0.0f };
            model_angle[3] = { 30.0f, 0.0f, 0.0f };
            model_pos[3] = { 0.0f, 0.0f, 0.0f };
            model_angle[2] = { -60.0f, 0.0f, 0.0f };
            model_pos[2] = { 0.0f, 0.5f, 2.0f };
            model_angle[7] = { 30.0f, 0.0f, 0.0f };
            model_pos[7] = { 0.0f, 0.0f, 0.0f };
            model_angle[6] = { 30.0f, 0.0f, 0.0f };
            model_pos[6] = { 0.0f, 0.0f, 0.0f };
            model_angle[5] = { -60.0f, 0.0f, 0.0f };
            model_pos[5] = { 0.0f, 0.5f, 2.0f };
            model_angle[10] = { 0.0f, 0.0f, 0.0f };
            model_pos[10] = { 0.0f, 0.0f, -0.5f };
            model_angle[9] = { 0.0f, 0.0f, 0.0f };
            model_pos[9] = { 0.0f, 0.0f, -0.5f };
            model_angle[8] = { 45.0f, 0.0f, 0.0f };
            model_pos[8] = { 0.0f, -1.0f, 2.0f };
            model_angle[13] = { -30.0f, 0.0f, 0.0f };
            model_pos[13] = { 0.0f, 0.0f, -1.5f };
            model_angle[12] = { -30.0f, 0.0f, 0.0f };
            model_pos[12] = { 0.0f, 0.0f, -1.5f };
            model_angle[11] = { 30.0f, 0.0f, 0.0f };
            model_pos[11] = { 0.0f, 0.0f, 2.0f };
            model_angle[14] = { -60.0f, 0.0f, 0.0f };
            model_pos[14] = { 0.0f, 0.5f, 2.0f };
        }
        else if (phase == 2) {
            model_angle[7] = { -15.0f, 0.0f, 0.0f };
            model_pos[7] = { 0.0f, -0.25f, 2.5f };
            model_angle[6] = { -15.0f, 0.0f, 0.0f };
            model_pos[6] = { 0.0f, -0.25f, 2.5f };
            model_angle[5] = { -105.0f, 0.0f, 0.0f };
            model_pos[5] = { 0.0f, 1.5f, 3.5f };
            model_angle[4] = { 45.0f, 0.0f, 0.0f };
            model_pos[4] = { 0.0f, 0.5f, -0.75f };
            model_angle[3] = { 45.0f, 0.0f, 0.0f };
            model_pos[3] = { 0.0f, 0.5f, -0.75f };
            model_angle[2] = { 135.0f, 0.0f, 0.0f };
            model_pos[2] = { 0.0f, 1.25f, 0.5f };
            model_angle[13] = { 30.0f, 0.0f, 0.0f };
            model_pos[13] = { 0.0f, 0.0f, 0.0f };
            model_angle[12] = { 30.0f, 0.0f, 0.0f };
            model_pos[12] = { 0.0f, 0.0f, 0.0f };
            model_angle[11] = { 75.0f, 0.0f, 0.0f };
            model_pos[11] = { 0.0f, -2.0f, 1.5f };
            model_angle[10] = { -45.0f, 0.0f, 0.0f };
            model_pos[10] = { 0.0f, 0.0f, -1.5f };
            model_angle[9] = { -45.0f, 0.0f, 0.0f };
            model_pos[9] = { 0.0f, 0.0f, -1.5f };
            model_angle[8] = { -15.0f, 0.0f, 0.0f };
            model_pos[8] = { 0.0f, 1.0f, 0.0f };
            model_angle[14] = { -45.0f, 0.0f, 0.0f };
            model_pos[14] = { 0.0f, 0.0f, 0.75f };
        }
        else if (phase == 3) {
            model_angle[4] = { 30.0f, 0.0f, 0.0f };
            model_pos[4] = { 0.0f, 0.0f, 0.0f };
            model_angle[3] = { 30.0f, 0.0f, 0.0f };
            model_pos[3] = { 0.0f, 0.0f, 0.0f };
            model_angle[2] = { -60.0f, 0.0f, 0.0f };
            model_pos[2] = { 0.0f, 0.5f, 2.0f };
            model_angle[7] = { 30.0f, 0.0f, 0.0f };
            model_pos[7] = { 0.0f, 0.0f, 0.0f };
            model_angle[6] = { 30.0f, 0.0f, 0.0f };
            model_pos[6] = { 0.0f, 0.0f, 0.0f };
            model_angle[5] = { -60.0f, 0.0f, 0.0f };
            model_pos[5] = { 0.0f, 0.5f, 2.0f };
            model_angle[13] = { 0.0f, 0.0f, 0.0f };
            model_pos[13] = { 0.0f, 0.0f, -0.5f };
            model_angle[12] = { 0.0f, 0.0f, 0.0f };
            model_pos[12] = { 0.0f, 0.0f, -0.5f };
            model_angle[11] = { 45.0f, 0.0f, 0.0f };
            model_pos[11] = { 0.0f, -1.0f, 2.0f };
            model_angle[10] = { -30.0f, 0.0f, 0.0f };
            model_pos[10] = { 0.0f, 0.0f, -1.5f };
            model_angle[9] = { -30.0f, 0.0f, 0.0f };
            model_pos[9] = { 0.0f, 0.0f, -1.5f };
            model_angle[8] = { 30.0f, 0.0f, 0.0f };
            model_pos[8] = { 0.0f, 0.0f, 2.0f };
            model_angle[14] = { -60.0f, 0.0f, 0.0f };
            model_pos[14] = { 0.0f, 0.5f, 2.0f };
        }
    }
    //ATTACK
    else if (ani_state == ATTACK) {
        //애니메이션 진행에 따른 변수 변화 표
        //    *빈칸 : 변동없음을 의미
        // 
        //           /      phase 1,4   /       phase 2     /       phase 3     /
        // RUA angle /  {-45, 0, 0}     /   {-90, 0, 30}    /
        //     pos   /  {0, 0.5, 2.25}  /   {1.5, 3, 2}     /
        // RMA angle /RUA에 종속
        //     pos   /
        // RDA angle /RUA에 종속                            /
        //     pos   /                                      /
        // LUA angle /  {-45, 0, 0}     /   {-90, 0, -30}   /
        //     pos   /  {0, 0.5, 2.25}  /   {-1.5, 3, 2}    /
        // LMA angle /LUA에 종속
        //     pos   /
        // LDA angle /LUA에 종속                            /
        //     pos   /                                      /
        // RUL angle /  {-60, 0, 0}     /
        //     pos   /  {0, -0.75, -1.0}/
        // RML angle /  {-60, 0, 0}     /
        //     pos   /  {0, -0.75, -1.0}/
        // RDL angle /  {0, 0, 0}       /
        //     pos   /  {0, 0.75, 2}    /
        // LUL angle /  {0, 0, 0}       /
        //     pos   /  {0, 0, 0}       /
        // LML angle /  {0, 0, 0}       /
        //     pos   /  {0, 0, 0}       /
        // LDL angle /  {45, 0, 0}      /
        //     pos   /  {0, -1, 2.5}    /
        // gun angle /  {-45, 0, 0}     /   {-90, 0, 0}     /
        //     pos   /  {0, 0.5, 2.25}  /   {2, 2.75, 2.75} /
        // head angle/
        //      pos  /
        // body angle/
        //      pos  /

        model_angle[0].x = model_angle[1].x = 0.0f;
        //하반신(애니메이션 중 고정값)
        model_angle[10] = { -60.0f, 0.0f, 0.0f };
        model_pos[10] = { 0.0f, -0.75f, -1.0f };
        model_angle[9] = { -60.0f, 0.0f, 0.0f };
        model_pos[9] = { 0.0f, -0.75f, -1.0f };
        model_angle[8] = { 0.0f, 0.0f, 0.0f };
        model_pos[8] = { 0.0f, 0.75f, 2.0f };
        model_angle[13] = { 0.0f, 0.0f, 0.0f };
        model_pos[13] = { 0.0f, 0.0f, 0.0f };
        model_angle[12] = { 0.0f, 0.0f, 0.0f };
        model_pos[12] = { 0.0f, 0.0f, 0.0f };
        model_angle[11] = { 45.0f, 0.0f, 0.0f };
        model_pos[11] = { 0.0f, -1.0f, 2.5f };

        //상체 분기
        if (ani_timer < 0.1f || ani_timer >= 0.6f) {
            model_angle[7] = { -45.0f, 0.0f, 0.0f };
            model_pos[7] = { 0.0f, 0.5f, 2.25f };
            model_angle[6] = { -45.0f, 0.0f, 0.0f };
            model_pos[6] = { 0.0f, 0.5f, 2.25f };
            model_angle[5] = { -45.0f, 0.0f, 0.0f };
            model_pos[5] = { 0.0f, 0.5f, 2.25f };
            model_angle[4] = { -45.0f, 0.0f, 0.0f };
            model_pos[4] = { 0.0f, 0.5f, 2.25f };
            model_angle[3] = { -45.0f, 0.0f, 0.0f };
            model_pos[3] = { 0.0f, 0.5f, 2.25f };
            model_angle[2] = { -45.0f, 0.0f, 0.0f };
            model_pos[2] = { 0.0f, 0.5f, 2.25f };

            model_angle[14] = { -45.0f, 0.0f, 0.0f };
            model_pos[14] = { 0.0f, 0.5f, 2.25f };
        }
        else if (ani_timer < 0.4f) {
            model_angle[7] = { -90.0f, 0.0f, -30.0f };
            model_pos[7] = { -1.5f, 3.0f, 2.0f };
            model_angle[6] = { -90.0f, 0.0f, -30.0f };
            model_pos[6] = { -1.5f, 3.0f, 2.0f };
            model_angle[5] = { -90.0f, 0.0f, -30.0f };
            model_pos[5] = { -1.5f, 3.0f, 2.0f };
            model_angle[4] = { -90.0f, 0.0f, 30.0f };
            model_pos[4] = { 1.5f, 3.0f, 2.0f };
            model_angle[3] = { -90.0f, 0.0f, 30.0f };
            model_pos[3] = { 1.5f, 3.0f, 2.0f };
            model_angle[2] = { -90.0f, 0.0f, 30.0f };
            model_pos[2] = { 1.5f, 3.0f, 2.0f };

            model_angle[14] = { -90.0f, 0.0f, 0.0f };
            model_pos[14] = { 2.0f, 2.75f, 2.75f };
        }
        else if (ani_timer < 0.6f) {
            model_angle[7] = { -90.0f, 0.0f, -30.0f };
            model_pos[7] = { -1.5f, 3.0f, 2.0f };
            model_angle[6] = { -90.0f, 0.0f, -30.0f };
            model_pos[6] = { -1.5f, 3.0f, 2.0f };
            model_angle[5] = { -120.0f, 0.0f, -30.0f };
            model_pos[5] = { -1.5f, 3.25f, 2.0f };
            model_angle[4] = { -90.0f, 0.0f, 30.0f };
            model_pos[4] = { 1.5f, 3.0f, 2.0f };
            model_angle[3] = { -90.0f, 0.0f, 30.0f };
            model_pos[3] = { 1.5f, 3.0f, 2.0f };
            model_angle[2] = { -120.0f, 0.0f, 30.0f };
            model_pos[2] = { 1.5f, 3.25f, 2.0f };

            model_angle[14] = { -120.0f, 0.0f, 0.0f };
            model_pos[14] = { 2.0f, 3.25f, 2.75f };
        }
        
        if(ani_timer >= 0.7f) {
            ani_state = IDLE;
            for (int i = 0; i < 15; ++i) {
                model_angle[i] = vector3();
                model_pos[i] = vector3();
            }
        }
    }
    //DIE
    else if (ani_state == DIE) {
        for (int i = 0; i < 15; ++i) {
           model_angle[i] = vector3();
           model_pos[i] = vector3();
        }
        angle.y -= gm.real_delay * 360.0f;
        position.y += dir * 0.1f;
        if (position.y >= 2.0f) {
            dir = -1.0f;
        }
    }
}

void player::init() {
    //character's model 순서
    //0 : head
    //1 : body
    //2 : Right Down Arm
    //3 : Right Mid Arm
    //4 : Right Up Arm
    //5 : Left Down Arm
    //6 : Left Mid Arm
    //7 : Left Up Arm
    //8 : Right Down Leg
    //9 : Right Mid Leg
    //10 : Right Up Leg
    //11 : Left Down Leg
    //12 : Left Mid Leg
    //13 : Left Up Leg
    //14 : Gun

    waitInit = false;
    std::vector<std::string> list;
    list.push_back("c_head");
    list.push_back("c_body");
    list.push_back("c_rda");
    list.push_back("c_rma");
    list.push_back("c_rua");
    list.push_back("c_lda");
    list.push_back("c_lma");
    list.push_back("c_lua");
    list.push_back("c_rdl");
    list.push_back("c_rml");
    list.push_back("c_rul");
    list.push_back("c_ldl");
    list.push_back("c_lml");
    list.push_back("c_lul");
    list.push_back("c_gun");

    model_init(list);
    scale = { 0.05f, 0.05f, 0.05f };
    ani_state = IDLE;
    recent_ani_state = IDLE;
    ani_timer = 0.0f;
    waitshot = 0;
    dir = 1.0f;
}