#pragma once
#include"object.h"
#include"arrow.h"

class player : public object {
private:
    float movespeed = 2.1f;
    int ani_state;          //�̹� ������Ʈ�� ������ animation
    int recent_ani_state;   //���� ������Ʈ�� ����� animation(state ���� ������)
    float ani_timer;        //animation�� Ÿ�̸� ����
    bool waitshot;          //attack - �Ѿ� �߻� ������
    int xv;                 //MOVE - �¿� ���⺤��
    int zv;                 //MOVE - ���� ���⺤��
    float dir;              //DIE - ���� ���⺤��

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