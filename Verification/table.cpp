#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int main(int argc, char** argv){
    string filename = argv[1];
    ifstream ifs_crt, ifs_aby;
    string buffer;

    ifs_crt.open("./correct/" + filename + "_0.txt");
    ifs_aby.open("./aby/" + filename + "_0.txt");
    if (!ifs_crt.is_open() || !ifs_aby.is_open()) {
        cout << "Failed to open file.\n";
        return 1;
    }

    vector<double> crt_num, aby_num, crt_modf_num, aby_modf_num;
    string tok, s;
    const char delimiter = ' ';
    int cnt = 1;
    while (getline(ifs_crt, s)) {
        stringstream ss(s);
        getline(ss, tok, delimiter);
        getline(ss, tok, delimiter);
        double d = strtod(tok.c_str(), nullptr);
        crt_num.push_back(d);
        if(filename == "modf"){
            getline(ss, tok, delimiter);
            d = strtod(tok.c_str(), nullptr);
            crt_modf_num.push_back(d);
        }
    }
    while (getline(ifs_aby, s)) {
        stringstream ss(s);
        getline(ss, tok, delimiter);
        getline(ss, tok, delimiter);
        double d = strtod(tok.c_str(), nullptr);
        aby_num.push_back(d);
        if(filename == "modf"){
            getline(ss, tok, delimiter);
            d = strtod(tok.c_str(), nullptr);
            aby_modf_num.push_back(d);
        }
    }
    if(crt_num.size() != aby_num.size()){
        cout << "Failed.\n";
    }
    int size = crt_num.size();
    int fail = 0;
    while(size){
        double diff = fabs(crt_num[size-1] - aby_num[size-1]);
        if(diff > 1e-8){
            fail++;
        }
        if(filename == "modf"){
            diff = fabs(crt_modf_num[size-1] - aby_modf_num[size-1]);
            if(diff > 1e-8){
                fail++;
            }
        }
        size--;
    }

    double rate = (double)fail / (double)crt_num.size();
    if(filename == "modf"){
        rate /= 2;
    }
    cout << filename << " Miss rate: " << rate << endl;

    ifs_crt.close();
    ifs_aby.close();
    return 0;
}