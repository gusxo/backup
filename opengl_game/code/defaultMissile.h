#pragma once
#include"object.h"
class defaultMissile : public object {
private:
    float movespeed = 2.7f;
    float remaintime = 8.0f;
public:
    void update() override;
    void init() override;
};

