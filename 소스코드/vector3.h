#pragma once
#include<cmath>
#define PI 3.14159265f

/// <summary>
/// vector3 Ŭ���� ����
/// ���� ��ȯ �Լ�(degtorad(float), radtodeg(float) ����
/// PI ����
/// </summary>


class vector3 {
private:
    static void turn_point(float base_x, float base_y, float& x, float& y, float angle);
public:
    float x;
    float y;
    float z;
    vector3() : x(0), y(0), z(0) {};
    vector3(float x, float y, float z) : x(x), y(y), z(z) {};
    //vector3(const vector3& other);
    //~vector3();
    //base�� �������� point�� ȸ����ȯ
    static vector3 turn(vector3 point, vector3 base, vector3 angles);
    //base���� target������ ���� ���
    static vector3 getAngle(vector3 base, vector3 target, bool xAxis, bool yAxis, bool zAxis);
    static vector3 getAngle(vector3 base, vector3 target);
    //vector3& operator=(const vector3& other);
};


float degtorad(float degree);
float radtodeg(float radian);
