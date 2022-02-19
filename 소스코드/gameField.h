#pragma once
#include"object.h"

class gameField : public object {
private:
public:
    void update() override;
    void init() override;
};