#pragma once
#include"object.h"

class camera : public object {
public:
    void update() override;
    // camera.draw는 gluLookAt()을 호출하는 것으로 대체
    void draw() override;
};
