#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void create_file(string filename, long int n){
    ifstream dict("data/dict.txt");
    ofstream file(filename);
    vector<string> words;
    string line;
    while (getline(dict, line)) {
        words.push_back(line);
    }
    dict.close();
    int words_size = words.size();
    for (long int i = 0; i < n; i++) {
        file << words[rand() % words_size] << " ";
    }
    file.close();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <filename> <size in Mb>" << endl;
    }
    srand(time(NULL));
    string filename = argv[1];
    long int n = atoi(argv[2]) * 100000;
    create_file(filename, n);
    return 0;
}