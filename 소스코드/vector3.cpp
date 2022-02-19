#include"vector3.h"

void vector3::turn_point(float base_x, float base_y, float& x, float& y, float angle) {
	float radian = degtorad(angle);
	float tmpx = (x - base_x) * cos(radian) - (y - base_y) * sin(radian);
	float tmpy = (x - base_x) * sin(radian) + (y - base_y) * cos(radian);
	x = base_x + tmpx;
	y = base_y + tmpy;
}
vector3 vector3::turn(vector3 point, vector3 base, vector3 angles) {
	turn_point(base.y, base.z, point.y, point.z, angles.x);
	turn_point(base.z, base.x, point.z, point.x, angles.y);
	turn_point(base.x, base.y, point.x, point.y, angles.z);
	return point;
}
vector3 vector3::getAngle(vector3 base, vector3 target, bool xAxis, bool yAxis, bool zAxis) {
	vector3 diff = { target.x - base.x, target.y - base.y, target.z - base.z };
	vector3 angle;
	if (xAxis) {
		float rad = atan2(diff.y, diff.z);
		float deg = radtodeg(rad);
		angle.x = deg;
	}
	if (yAxis) {
		float rad = atan2(diff.x, diff.z);
		float deg = radtodeg(rad);
		angle.y = deg;
	}
	if (zAxis) {
		float rad = atan2(diff.x, diff.y);
		float deg = radtodeg(rad);
		angle.z = deg;
	}
	return angle;
}
vector3 vector3::getAngle(vector3 base, vector3 target) {
	return getAngle(base, target, true, true, true);
}

//vector3& vector3::operator=(const vector3& other) {
//	x = other.x;
//	y = other.y;
//	z = other.z;
//}
//
//vector3::vector3(const vector3& other) {
//	x = other.x;
//	y = other.y;
//	z = other.z;
//}
//
//vector3::~vector3(){}


float degtorad(float degree) {
	return degree * PI / 180.0f;
}
float radtodeg(float radian) {
	return radian * 180.0f / PI;
}
