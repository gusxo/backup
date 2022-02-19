#include "gameManager.h"
#include "model.h"

model::model() {}

std::vector<float> model::my_strtok_f(char* str, char* delimeter) {
    std::vector<float> v;
    char* context = NULL;
    char* tok = strtok_s(str, delimeter, &context);

    while (tok != NULL) {
        v.push_back(atof(tok));
        tok = strtok_s(NULL, delimeter, &context);
    }
    return v;
}

std::vector<std::string> model::my_strtok_s(char* str, char* delimeter) {
    std::vector<std::string> v;
    char* context = NULL;
    char* tok = strtok_s(str, delimeter, &context);

    while (tok != NULL) {
        v.push_back(tok);
        tok = strtok_s(context, delimeter, &context);
    }
    return v;
}
std::vector<int> model::my_strtok_i(char* str, char* delimeter) {
    std::vector<int> v;
    char* context = NULL;
    char* tok = strtok_s(str, delimeter, &context);

    while (tok != NULL) {
        v.push_back(atoi(tok));
        tok = strtok_s(context, delimeter, &context);
    }
    return v;
}


void model::clear()
{
    objs.clear();
}

void model::loadObj(std::string obj_path)
{
    std::ifstream fin;
    try {
        fin.open(obj_path);
        std::string line;
        _obj obj_tmp;
        long long cnt = 0;
        int cnt_prev_vertex = 0;
        int cnt_prev_texture = 0;
        int cnt_prev_normal = 0;
        while (std::getline(fin, line)) {
            long long len = line.length();
            std::vector<float> vf;
            std::vector<std::string> s;
            std::vector<int> vi;
            vector3 p3;
            vector3 p2;
            _vec3i p3i;
            if (line[0] == 'o' && line[1] == ' ') {
                obj_tmp.name = line.substr(2, len - 2);
                objs.push_back(obj_tmp);
                if (cnt > 0) {
                    cnt_prev_vertex += objs[cnt - 1].v.size();
                    cnt_prev_texture += objs[cnt - 1].vt.size();
                    cnt_prev_normal += objs[cnt - 1].vn.size();
                }
                cnt += 1;
            }

            if (line[0] == 'v' && line[1] == ' ') {
                vf = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
                p3 = { vf[0], vf[1], vf[2] };
                objs[cnt - 1].v.push_back(p3);
            }

            if (line[0] == 'v' && line[1] == 't') {
                vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
                p2 = { vf[0], vf[1], 0 };
                objs[cnt - 1].vt.push_back(p2);
            }

            if (line[0] == 'v' && line[1] == 'n') {
                vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
                p3 = { vf[0], vf[1], vf[2] };
                objs[cnt - 1].vn.push_back(p3);
            }

            if (line[0] == 'f' && line[1] == ' ') {
                s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
                int nVertexes = s.size();
                _face face_tmp;
                for (int i = 0; i < nVertexes; ++i) {
                    vi = my_strtok_i((char*)s[i].c_str(), (char*)"/");
                    p3i.d = { vi[0] - cnt_prev_vertex, vi[1] - cnt_prev_texture, vi[2] - cnt_prev_normal };
                    face_tmp.v_pairs.push_back(p3i);
                }
                objs[cnt - 1].f.push_back(face_tmp);
            }
        }
    }
    catch (std::exception error) {
        //실패시 파일 닫기만 수행
        if (fin.is_open()) {
            fin.close();
        }
    }
}

void model::draw(vector3 position, vector3 angle, vector3 scale)
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(angle.x, 1, 0, 0);
    glRotatef(angle.y, 0, 1, 0);
    glRotatef(angle.z, 0, 0, 1);
    glScalef(scale.x, scale.y, scale.z);
    glDisable(GL_COLOR_MATERIAL);
    for (auto& obj : objs) {
        bool draw_texture = 0;

        //texture
        if (obj.texture_id > 0 && gameManager::instance().mats.size() >= obj.texture_id) {
            glBindTexture(GL_TEXTURE_2D, obj.texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gameManager::instance().mats[obj.texture_id - 1].cols, gameManager::instance().mats[obj.texture_id - 1].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, gameManager::instance().mats[obj.texture_id - 1].data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            draw_texture = 1;
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //Matrials
        if (obj.useMatrial[0]) {
            glMaterialfv(GL_FRONT, GL_AMBIENT, obj.ambient);
        }
        if (obj.useMatrial[1]) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, obj.diffuse);
        }
        if (obj.useMatrial[2]) {
            glMaterialfv(GL_FRONT, GL_SPECULAR, obj.specular);
        }
        if (obj.useMatrial[3]) {
            glMaterialfv(GL_FRONT, GL_SHININESS, obj.shininess);
        }
        if (obj.useMatrial[4]) {
            glMaterialfv(GL_FRONT, GL_EMISSION, obj.emisson);
        }

        


        for (auto& f : obj.f) {
            glBegin(GL_POLYGON);
            for (auto& v_pairs : f.v_pairs) {
                long long v_id = v_pairs.d[0];
                long long vt_id = v_pairs.d[1];
                long long vn_id = v_pairs.d[2];
                float x = obj.v[v_id - 1].x;
                float y = obj.v[v_id - 1].y;
                float z = obj.v[v_id - 1].z;

                float nx = obj.vn[vn_id - 1].x;
                float ny = obj.vn[vn_id - 1].y;
                float nz = obj.vn[vn_id - 1].z;

                glNormal3f(nx, ny, nz);
                if (draw_texture) {
                    float tx = obj.vt[vt_id - 1].x;
                    float ty = obj.vt[vt_id - 1].y;
                    glTexCoord2f(tx, ty);
                }
                glVertex3f(x, y, z);
            }
            glEnd();
        }
    }
    glPopMatrix();
}
