#include <GL/glut.h>
#include<chrono>
#include<iostream>
#include<random>
#include<unordered_map>

#include"vector3.h"
#include"object.h"
#include"gameManager.h"

void reshapefunc(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (double)w / h;
    gluPerspective(45, ratio, 0.1, 60.0);
    gameManager::instance().currentWindowWidth = w;
    gameManager::instance().currentWindowHeight = h;
    //glOrtho(-3, 3, -3, 3, 0.1, 10);
}

void frame(int val) {
    gameManager& gm = gameManager::instance();
    //check delay
    gm.current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltatime = gm.current_time - gm.last_frame_called;
    gm.real_delay = deltatime.count();
    gm.last_frame_called = gm.current_time;

    //objects들 등록/제거
    for (auto oid : gm.deleteList) {
        gm.objects.erase(oid);
    }
    for (auto pair : gm.insertList) {
        gm.objects.insert(pair);
    }
    gm.deleteList.clear();
    gm.insertList.clear();

    //run init
    for (auto& it : gm.objects) {
        if (it.second->enable && it.second->waitInit) {
            it.second->init();
        }
    }

    //run updates
    gm.update();
    for (auto& it : gm.objects) {
        if (it.second->enable) {
            it.second->update();
        }
    }

    //mouse flag 제거
    gm.iflag &= ~gameManager::MOUSE_LEFT;

    //call display func
    glutPostRedisplay();
    glutTimerFunc(FRAME_DELAY, frame, 0);
}

void keydownfunc(unsigned char key, int x, int y) {
    gameManager& gm = gameManager::instance();

    if (key == 'w' || key == 'W') {
        gm.iflag |= gameManager::KEY_UP;
    }
    else if (key == 'a' || key == 'A') {
        gm.iflag |= gameManager::KEY_LEFT;
    }
    else if (key == 's' || key == 'S') {
        gm.iflag |= gameManager::KEY_DOWN;
    }
    else if (key == 'd' || key == 'D') {
        gm.iflag |= gameManager::KEY_RIGHT;
    }
}

void keyupfunc(unsigned char key, int x, int y) {
    gameManager& gm = gameManager::instance();

    if (key == 'w' || key == 'W') {
        gm.iflag &= ~gameManager::KEY_UP;
    }
    else if (key == 'a' || key == 'A') {
        gm.iflag &= ~gameManager::KEY_LEFT;
    }
    else if (key == 's' || key == 'S') {
        gm.iflag &= ~gameManager::KEY_DOWN;
    }
    else if (key == 'd' || key == 'D') {
        gm.iflag &= ~gameManager::KEY_RIGHT;
    }
}

void mousefunc(int button, int state, int x, int y) {
    gameManager& gm = gameManager::instance();

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        gm.iflag |= gameManager::MOUSE_LEFT;
        gm.mouse_x = x;
        gm.mouse_y = y;
    }
    //button 3 : 마우스 휠 up, button 4 : 마우스 휠 down
    //게임 진행 상태에만 처리
    if (gm.gamestate == gm.gamestate_ingame) {
        if (button == 3 && gm.camZoomIn > 1.0f) {
            gm.camZoomIn -= 0.5f;
        }
        if (button == 4 && gm.camZoomIn < 5.0f) {
            gm.camZoomIn += 0.5f;
        }
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //안티 에일리어싱
    //미사용 : obj의 간선 사이사이에 배경이 투영되는 문제
    // 
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_POLYGON_SMOOTH);
    //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    for (auto& it : gameManager::instance().objects) {
        if (it.second->enable) {
            it.second->draw();
        }
    }

    glutSwapBuffers();
}



int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Test");
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);

    //Light init
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    float light_global_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float l0_pos[] = { 2.0f, 2.0f, 5.0f ,1.0f };
    float l0_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float l0_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float l0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_global_ambient);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, l0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

    glutDisplayFunc(display);
    glutReshapeFunc(reshapefunc);
    glutTimerFunc(FRAME_DELAY, frame, 0);
    glutKeyboardFunc(keydownfunc);
    glutKeyboardUpFunc(keyupfunc);
    glutMouseFunc(mousefunc);
    glutMainLoop();
}