#include <iostream>
#include <fstream>
#include <vector>
#include <random>


using namespace std;

int main(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 10000);
    
    string filename = "data\\dict.txt";

    fstream fin(filename);
    ofstream fout("data\\dict_reduce.txt");
    vector<string> words;
    string line;

    while (getline(fin, line)){
        words.push_back(line);
    }
    fin.close();

    int n = words.size();

    fout.close();
    return 0;
}