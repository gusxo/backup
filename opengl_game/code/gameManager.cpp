#include "gameManager.h"
#include"enemy.h"
#include"player.h"
#include"enemyTarget.h"
#include"gameField.h"
#include"camera.h"
#include"summonEffect.h"
#include"explosion.h"
#include"defeatImage.h"
#include"victoryImage.h"

const unsigned int gameManager::KEY_UP = 0x00000001;
const unsigned int gameManager::KEY_DOWN = 0x00000002;
const unsigned int gameManager::KEY_LEFT = 0x00000004;
const unsigned int gameManager::KEY_RIGHT = 0x00000008;
const unsigned int gameManager::MOUSE_LEFT = 0x00000010;
gameManager* gameManager::_instance = NULL;


gameManager::gameManager() {
    iflag = 0;
    mouse_x = 0;
    mouse_y = 0;
    currentWindowWidth = 0;
    currentWindowHeight = 0;
    real_delay = 0.0f;
    loadFiles();
    init();
}

void gameManager::init() {
    //system init
    gen = std::mt19937(rd());
    camZoomIn = 5.0f;
    update_calls = 0;
    
    _player = NULL;
    _enemyTarget = NULL;
    for (auto& it : objects) {
        delobj_force(it.first);
    }

    auto cam = std::make_shared<camera>();
    auto field = std::make_shared<gameField>();
    cam->is_important = true;
    field->is_important = true;
    addobj(cam);
    addobj(field);

    _enemyTarget = std::make_shared<enemyTarget>();    //enemyTarget 재생성
    addobj(_enemyTarget);

    gamestate = gamestate_main;
    cam->position = { 0.0f, 7.5f, 50.0f };
}

//타 object들이 참조할 Model 정보를 불러오고, 재질/텍스처를 지정한다.
void gameManager::loadFiles()
{
    models.clear();
    mats.clear();
    model_dictionary.clear();

    int n = 0;
    int tex_n = 0;

    // *** texture(cv::Mat) part ***

    //texture for (hollow_cylinder / block / gori)'s border
    mats.push_back(cv::Mat());
    mats[tex_n] = cv::imread("border.jpg", cv::IMREAD_COLOR);
    cvtColor(mats[tex_n], mats[tex_n], cv::COLOR_BGR2RGB);
    tex_n++;

    //texture for pylon(enemy target)
    mats.push_back(cv::Mat());
    mats[tex_n] = cv::imread("rainbow.png", cv::IMREAD_COLOR);
    cvtColor(mats[tex_n], mats[tex_n], cv::COLOR_BGR2RGB);
    tex_n++;

    //texture for victory image
    mats.push_back(cv::Mat());
    mats[tex_n] = cv::imread("victory.png", cv::IMREAD_COLOR);
    cvtColor(mats[tex_n], mats[tex_n], cv::COLOR_BGR2RGB);
    tex_n++;

    //texture for defeat image
    mats.push_back(cv::Mat());
    mats[tex_n] = cv::imread("defeat.png", cv::IMREAD_COLOR);
    cvtColor(mats[tex_n], mats[tex_n], cv::COLOR_BGR2RGB);
    tex_n++;

    //init texture
    GLuint* tex_ids = new GLuint[tex_n];
    for (int i = 0; i < tex_n; ++i) {
        tex_ids[i] = i + 1;
    }
    glGenTextures(tex_n, tex_ids);
    glEnable(GL_TEXTURE_2D);
    delete[] tex_ids;

    // *** model part ***
    // 
    //monkey
    models.push_back(model());
    models[n].loadObj("monkey.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[1] = 0.0f;
    models[n].objs[0].ambient[2] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.3f;
    models[n].objs[0].diffuse[2] = 0.3f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;

    model_dictionary["monkey"] = n;
    n++;

    //hollow_cylinder
    //objs : {body, out_border, inner_border}
    models.push_back(model());
    models[n].loadObj("hollow_cylinder.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[1].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[2].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[0].ambient[0] = 90.0f / 255.0f;
    models[n].objs[0].ambient[1] = 90.0f / 255.0f;
    models[n].objs[0].ambient[2] = 90.0f / 255.0f;

    models[n].objs[1].texture_id = 1;
    models[n].objs[2].texture_id = 1;

    model_dictionary["hollow_cylinder"] = n;
    n++;

    //red sphere
    models.push_back(model());
    models[n].loadObj("sphere.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 0.0f;
    models[n].objs[0].ambient[1] = 0.0f;
    models[n].objs[0].ambient[2] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["red_sphere"] = n;
    n++;

    //green sphere
    models.push_back(model());
    models[n].loadObj("sphere.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 0.0f;
    models[n].objs[0].ambient[1] = 0.0f;
    models[n].objs[0].ambient[2] = 0.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["green_sphere"] = n;
    n++;

    //black sphere
    models.push_back(model());
    models[n].loadObj("sphere.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 0.0f;
    models[n].objs[0].ambient[1] = 0.0f;
    models[n].objs[0].ambient[2] = 0.0f;
    models[n].objs[0].diffuse[0] = 0.1f;
    models[n].objs[0].diffuse[1] = 0.1f;
    models[n].objs[0].diffuse[2] = 0.1f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["black_sphere"] = n;
    n++;

    //pylon
    //objs : {down_cone / up_cone / torus}
    models.push_back(model());
    models[n].loadObj("pylon.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[1].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[2].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[0].texture_id = 2;
    models[n].objs[1].texture_id = 2;
    models[n].objs[2].texture_id = 2;

    model_dictionary["pylon"] = n;
    n++;

    //gori
    //objs : {out-border, body, inner-border}
    models.push_back(model());
    models[n].loadObj("gori.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[1].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[2].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[1].ambient[0] = 90.0f / 255.0f;
    models[n].objs[1].ambient[1] = 90.0f / 255.0f;
    models[n].objs[1].ambient[2] = 90.0f / 255.0f;

    models[n].objs[0].texture_id = 1;
    models[n].objs[2].texture_id = 1;

    model_dictionary["gori"] = n;
    n++;

    //block
    //objs : {body, border}
    models.push_back(model());
    models[n].loadObj("block.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[1].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[0].ambient[0] = 90.0f / 255.0f;
    models[n].objs[0].ambient[1] = 90.0f / 255.0f;
    models[n].objs[0].ambient[2] = 90.0f / 255.0f;

    models[n].objs[1].texture_id = 1;

    model_dictionary["block"] = n;
    n++;

    //summon_effect
    models.push_back(model());
    models[n].loadObj("cube.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[0].ambient[0] = 0.0f / 255.0f;
    models[n].objs[0].ambient[1] = 0.0f / 255.0f;
    models[n].objs[0].ambient[2] = 0.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f / 255.0f;
    models[n].objs[0].diffuse[1] = 204.0f / 255.0f;
    models[n].objs[0].diffuse[2] = 204.0f / 255.0f;
    models[n].objs[0].specular[0] = 0.0f / 255.0f;
    models[n].objs[0].specular[1] = 0.0f / 255.0f;
    models[n].objs[0].specular[2] = 0.0f / 255.0f;

    model_dictionary["summon_effect"] = n;
    n++;

    //explosion_effect
    models.push_back(model());
    models[n].loadObj("sphere.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };

    models[n].objs[0].ambient[0] = 0.1f;
    models[n].objs[0].ambient[1] = 0.1f;
    models[n].objs[0].ambient[2] = 0.1f;
    models[n].objs[0].diffuse[0] = 255.0f / 255.0f;
    models[n].objs[0].diffuse[1] = 153.0f / 255.0f;
    models[n].objs[0].diffuse[2] = 51.0f / 255.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;

    model_dictionary["explosion_effect"] = n;
    n++;

    //victory
    models.push_back(model());
    models[n].loadObj("plane.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].texture_id = 3;
    model_dictionary["victory"] = n;
    n++;

    //defeat
    models.push_back(model());
    models[n].loadObj("plane.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].texture_id = 4;
    model_dictionary["defeat"] = n;
    n++;

    //character body
    models.push_back(model());
    models[n].loadObj("c_body.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 0.0f / 255.0f;
    models[n].objs[0].ambient[1] = 102.0f / 255.0f;
    models[n].objs[0].ambient[2] = 204.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_body"] = n;
    n++;

    //character head
    models.push_back(model());
    models[n].loadObj("c_head.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_head"] = n;
    n++;

    //character gun
    models.push_back(model());
    models[n].loadObj("c_gun.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 64.0f / 255.0f;
    models[n].objs[0].ambient[1] = 64.0f / 255.0f;
    models[n].objs[0].ambient[2] = 64.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_gun"] = n;
    n++;

    //character right upper arm
    models.push_back(model());
    models[n].loadObj("c_RUA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rua"] = n;
    n++;

    //character right mid arm
    models.push_back(model());
    models[n].loadObj("c_RMA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rma"] = n;
    n++;

    //character right down arm
    models.push_back(model());
    models[n].loadObj("c_RDA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rda"] = n;
    n++;

    //character left upper arm
    models.push_back(model());
    models[n].loadObj("c_LUA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_lua"] = n;
    n++;

    //character left mid arm
    models.push_back(model());
    models[n].loadObj("c_LMA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_lma"] = n;
    n++;

    //character left down arm
    models.push_back(model());
    models[n].loadObj("c_LDA.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_lda"] = n;
    n++;

    //character right upper leg
    models.push_back(model());
    models[n].loadObj("c_RUL.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rul"] = n;
    n++;

    //character right mid leg
    models.push_back(model());
    models[n].loadObj("c_RML.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rml"] = n;
    n++;

    //character right down leg
    models.push_back(model());
    models[n].loadObj("c_RDL.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_rdl"] = n;
    n++;

    //character left upper leg
    models.push_back(model());
    models[n].loadObj("c_LUL.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_lul"] = n;
    n++;

    //character left mid leg
    models.push_back(model());
    models[n].loadObj("c_LML.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_lml"] = n;
    n++;

    //character left down leg
    models.push_back(model());
    models[n].loadObj("c_LDL.obj");
    models[n].objs[0].useMatrial = { 1, 1, 1, 1, 0 };
    models[n].objs[0].ambient[0] = 255.0f / 255.0f;
    models[n].objs[0].ambient[1] = 178.0f / 255.0f;
    models[n].objs[0].ambient[2] = 102.0f / 255.0f;
    models[n].objs[0].diffuse[0] = 0.0f;
    models[n].objs[0].diffuse[1] = 0.0f;
    models[n].objs[0].diffuse[2] = 0.0f;
    models[n].objs[0].specular[0] = 0.0f;
    models[n].objs[0].specular[1] = 0.0f;
    models[n].objs[0].specular[2] = 0.0f;
    model_dictionary["c_ldl"] = n;
    n++;
}

void gameManager::hitplayer() {
    if (!_player || gamestate == gamestate_gameover) return;
    if (--_player->hp <= 0) {
        gamestate = gamestate_gameover;
        update_calls = 0;
    }
}

void gameManager::hittarget() {
    if (!_enemyTarget || gamestate == gamestate_gameover) return;
    if (--_enemyTarget->hp <= 0) {
        gamestate = gamestate_gameover;
        update_calls = 0;
        auto exp_effect = std::make_shared<explosion>();
        exp_effect->position = _enemyTarget->position;
        exp_effect->scale = { 0.75f, 0.75f, 0.75f };
        addobj(exp_effect);
    }
}

void gameManager::addobj(std::shared_ptr<object> obj) {
    //oid 중복일시 무시
    if (objects.find(obj->getoid()) == objects.end()) {
        insertList.push_back({ obj->getoid(), obj });
    }
}
void gameManager::delobj(unsigned int oid) {
    auto obj = objects.find(oid);
    if (obj == objects.end()) return;
    if (obj->second->is_important) return;
    deleteList.push_back(oid);
}
gameManager& gameManager::instance()
{
    if (_instance == NULL) {
        _instance = new gameManager();
    }
    return *_instance;
}
std::shared_ptr<player> const& gameManager::pInstance()
{
    return _player;
}
std::shared_ptr<enemyTarget> const& gameManager::etInstance()
{
    return _enemyTarget;
}
void gameManager::delobj_force(unsigned int oid) {
    deleteList.push_back(oid);
}


void gameManager::update() {
    update_calls++;


    //게임 시작까지의 최초 대기시간
    if (gamestate == gamestate_main && update_calls > 200) {
        update_calls = 0;
        gamestate = gamestate_loading;
    }
    

    //gameLoading 연출
    if (gamestate == gamestate_loading && update_calls == 90) {
        //player생성파트
        _player = std::make_shared<player>();
        _player->position = { 0, 0, 4.0f };
        auto effect = std::make_shared<summonEffect>();
        effect->position = _player->position;
        effect->scale = { 0.2f, 0.2f, 0.2f };
        effect->summon_object = _player;
        addobj(effect);
    }
    if (gamestate == gamestate_loading && update_calls == 120) {
        //enemy생성파트
        vector3 summonpos = { 0,0,6.5f };
        int summoncnt = 12;
        float turnangle = 360.0f / summoncnt;
        for (int i = 0; i < summoncnt; ++i) {
            auto enemy_instance = std::make_shared<enemy>();
            auto effect_instance = std::make_shared<summonEffect>();
            effect_instance->position = summonpos;
            effect_instance->summon_object = enemy_instance;
            effect_instance->scale = { 0.5f, 0.5f, 0.5f };
            enemy_instance->position = summonpos;
            addobj(effect_instance);
            summonpos = vector3::turn(summonpos, { 0,0,0 }, { 0, turnangle, 0 });
        }
    }
    if (gamestate == gamestate_loading && update_calls > 180) {
        update_calls = 0;
        gamestate = gamestate_ingame;
    }

    //gameover 후 이미지 띄우기
    if (gamestate == gamestate_gameover && update_calls == 240) {
        std::shared_ptr<object> img;
        if (_enemyTarget->hp <= 0) {
            img = std::make_shared<victoryImage>();
        }
        else {
            img = std::make_shared<defeatImage>();
        }
        float disRate = (camZoomIn - 1.0f) / 8.0f + 0.5f;    //현재 카메라 줌인에 따라 scale 및 position 조정
        img->position = { _player->position.x , camZoomIn * 1.2f, _player->position.z + camZoomIn * 0.8f };
        img->scale = { 0.1f * disRate, 0.1f * disRate, 0.05f*disRate };
        img->angle = { 210.0f, 0.0f, 0.0f };
        addobj(img);
    }
    //자동 재시작
    if (gamestate == gamestate_gameover && update_calls == 600) {
        init();
    }
}