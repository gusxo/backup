#pragma once
#include"object.h"

class camera : public object {
public:
    void update() override;
    // camera.draw�� gluLookAt()�� ȣ���ϴ� ������ ��ü
    void draw() override;
};
