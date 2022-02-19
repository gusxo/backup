#pragma once
#include"object.h"
#include"arrow.h"

class player : public object {
private:
    float movespeed = 2.1f;
    int ani_state;          //이번 업데이트에 수행할 animation
    int recent_ani_state;   //저번 업데이트에 수행된 animation(state 변경 감지용)
    float ani_timer;        //animation용 타이머 변수
    bool waitshot;          //attack - 총알 발사 지연용
    int xv;                 //MOVE - 좌우 방향벡터
    int zv;                 //MOVE - 상하 방향벡터
    float dir;              //DIE - 상하 방향벡터

    static const int IDLE;
    static const int MOVE;
    static const int ATTACK;
    static const int DIE;

    void ani();
public:
    void update() override;
    void init() override;
    int hp = 1;
};