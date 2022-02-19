#include<fstream>
#include<functional>
#include<string>

int main(){
    std::hash<std::string> h;
    std::fstream fin, fout;
    std::string str;
    fin.open("pw", std::ios::in);
    fout.open("pw_hash", std::ios::out);
    while(std::getline(fin,str)){
        int blankpos = str.find(' ');
        auto id = str.substr(0,blankpos);
        auto password = str.substr(blankpos+1);
        fout << h(id + "5") << " " << h(password + "5") << "\n";
    }
    fin.close();
    fout.close();
}