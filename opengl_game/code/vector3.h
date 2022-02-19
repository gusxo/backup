#pragma once
#include<cmath>
#define PI 3.14159265f

/// <summary>
/// vector3 클래스 정의
/// 각도 변환 함수(degtorad(float), radtodeg(float) 정의
/// PI 정의
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
    //base를 기준으로 point를 회전변환
    static vector3 turn(vector3 point, vector3 base, vector3 angles);
    //base에서 target으로의 각도 계산
    static vector3 getAngle(vector3 base, vector3 target, bool xAxis, bool yAxis, bool zAxis);
    static vector3 getAngle(vector3 base, vector3 target);
    //vector3& operator=(const vector3& other);
};


float degtorad(float degree);
float radtodeg(float radian);
