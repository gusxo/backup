#pragma once
#include"object.h"
#include<random>
#include<chrono>
#include<unordered_map>
#include<GL/glut.h>
#include"player.h"
#include"enemyTarget.h"
#include<opencv2/opencv.hpp>

#define MAX_FPS 60
#define FRAME_DELAY 1000/MAX_FPS

class gameManager {
private:
    std::random_device rd;
    void delobj_force(unsigned int oid);
    static gameManager* _instance;
    std::shared_ptr<player> _player;
    std::shared_ptr<enemyTarget> _enemyTarget;
    gameManager();
    unsigned int update_calls;   //�ε� �ִϸ��̼ǿ� ī����

public:
    unsigned int gamestate = -1;
    const unsigned int gamestate_main = 0;
    const unsigned int gamestate_loading = 1;
    const unsigned int gamestate_ingame = 2;
    const unsigned int gamestate_gameover = 3;
    std::unordered_map<unsigned int, std::shared_ptr<object>> objects;
    std::vector<unsigned int> deleteList;
    std::vector<std::pair<unsigned int, std::shared_ptr<object>>> insertList;


    std::mt19937 gen;                                   //random �������

    int currentWindowWidth;
    int currentWindowHeight;

    //�Է� �÷��׿� ����� ��Ʈ����� ���
    unsigned int iflag;                 //input flag
    int mouse_x, mouse_y;
    static const unsigned int KEY_UP;      //w key
    static const unsigned int KEY_DOWN;     //s key
    static const unsigned int KEY_LEFT;     //a key
    static const unsigned int KEY_RIGHT;    //d key
    static const unsigned int MOUSE_LEFT;   //mouse left click

    //frame ���� ������ üũ��
    std::chrono::high_resolution_clock::time_point last_frame_called;
    std::chrono::high_resolution_clock::time_point current_time;
    float real_delay;

    //model / texture ������
    std::unordered_map<std::string, unsigned int> model_dictionary;
    std::vector<model> models;
    std::vector<cv::Mat> mats;

    //camera �Ÿ� ������
    float camZoomIn;

    void init();
    void loadFiles();
    void hitplayer();
    void hittarget();
    void update();
    
    void addobj(std::shared_ptr<object> obj);
    void delobj(unsigned int oid);

    //singletone
    static gameManager& instance();
    std::shared_ptr<player> const & pInstance();
    std::shared_ptr<enemyTarget> const & etInstance();
};