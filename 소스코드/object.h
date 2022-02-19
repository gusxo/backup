#pragma once
#include"vector3.h"
#include <GL/glut.h>
#include<vector>
#include"model.h"
#include<unordered_map>

class object {
private:
    static unsigned int next_oid;
    unsigned int oid;    //object id
protected:
    std::vector<vector3> model_pos;
    std::vector<vector3> model_angle;
    std::vector<vector3> model_scale;
    std::vector<int> model_index;
    std::unordered_map<std::string, unsigned int> model_dictionary;
    void model_init(std::vector<std::string> modelnames);
public:
    vector3 position;
    vector3 angle;
    vector3 scale;
    bool enable = true;
    bool waitInit = true;
    bool is_important = false;
    object() : position(), angle(), scale({ 1,1,1 }), oid(next_oid++) {};
    object(vector3 transform) : position(transform), angle(), scale({ 1,1,1 }), oid(next_oid++) {};
    object(vector3 transform, vector3 angle) : position(transform), angle(angle), scale({ 1,1,1 }), oid(next_oid++) {};
    object(vector3 transform, vector3 angle, vector3 scale) : position(transform), angle(angle), scale(scale), oid(next_oid++) {};
    virtual void init();
    virtual void update();
    virtual void draw();
    unsigned int getoid();
    //target을 보도록 angle 변경
    void lookAt(vector3 target, bool xAxis, bool yAxis, bool zAxis);
    void lookAt(vector3 target);
};