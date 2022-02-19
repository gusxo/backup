#pragma once
#include"object.h"


class arrow : public object {
private:
    float movespeed = 4.5f;
    float remaintime = 3.0f;
public:
    void update() override;
    void init() override;
};