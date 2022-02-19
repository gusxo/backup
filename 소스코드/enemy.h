#pragma once
#include"object.h"
#include"defaultMissile.h"

class enemy : public object {
private:
    int chase_chance = 50;
    int cooltime_min = 1000;
    int cooltime_max = 5000;
    float cooltime = 1.0f;
    float updown_dir = 1;
public:
    void update() override;
    void init() override;
};
