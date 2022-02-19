#pragma once
#include"vector3.h"
#include<GL/glut.h>
#include<fstream>
#include<string>
#include<vector>

class model{
private:
	//내부 클래스
	class _vec3i {
	public:
		std::vector<int> d{ 0,0,0 };
	};
	class _face {
	public:
		std::vector<_vec3i> v_pairs;
	};
	class _obj {
	public:
		std::string name;
		std::vector<vector3> v;
		std::vector<vector3> vt;
		std::vector<vector3> vn;
		std::vector<_face> f;
		//mat
		GLfloat ambient[4];
		GLfloat diffuse[4];
		GLfloat specular[4];
		GLfloat shininess[1];
		GLfloat emisson[4];
		std::vector<bool> useMatrial;	//{ambient, diffuse, specular, shiniess, emisson}
		//texture
		unsigned long long texture_id;	// default 0, (0 == unused)
		_obj() : ambient{ 1.0f,1.0f,1.0f,1.0f }, diffuse{ 1.0f,1.0f,1.0f,1.0f }, specular{ 1.0f,1.0f,1.0f,1.0f }, shininess{ 25.0f }, emisson{ 1.0f,1.0f,1.0f,1.0f }, useMatrial({ 0,0,0,0,0 }), texture_id(0) {}
	};

	//함수
	std::vector<float> my_strtok_f(char* str, char* delimeter);
	std::vector<std::string> my_strtok_s(char* str, char* delimeter);
	std::vector<int> my_strtok_i(char* str, char* delimeter);
public:
	//변수
	std::vector<_obj> objs;

	//함수
	model();
	void clear();	//objs 벡터 초기화
	void loadObj(std::string obj_path);	//objs 벡터 설정
	void draw(vector3 position = vector3(), vector3 angle = vector3(), vector3 scale = { 1.0f,1.0f,1.0f });	//실제 그리기 처리
};

