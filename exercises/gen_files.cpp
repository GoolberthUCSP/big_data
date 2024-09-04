#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <windows.h>

using namespace std;

const int N_FILES = 1000;
string SIZE_ = "10000"; // in Mb
string DIR = "data/inv_idx/";

int main(int argc, char *argv[]) {
    string cmd = "g++ -o gen_file.exe gen_file.cpp";
    system(cmd.c_str());
    string file_size = to_string(atoi(SIZE_.c_str()) / N_FILES);
    for (int i = 0; i < N_FILES; i++) {
        string filename = DIR + "file_" + to_string(i) + ".txt";
        cmd = "gen_file.exe " + filename + " " + file_size;
        system(cmd.c_str());
    }
}