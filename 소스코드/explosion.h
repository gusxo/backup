#pragma once
#include "object.h"
#include<random>
class explosion : public object {
private:
    int frame = 0;
public:
    void init();
    void update();
};

