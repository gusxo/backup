#pragma once
#include"object.h"
class enemyTarget : public object {
private:
public:
    int hp = 3;
    void update() override;
    void init() override;
};
