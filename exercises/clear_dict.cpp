#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <sstream>

using namespace std;

int main() {
    unordered_set<string> stopwords;
    string line;

    ifstream dict("data\\dict.txt");
    ifstream stop("data\\stopwords.txt");
    ofstream fout("data\\clean_dict.txt");

    if (!dict.is_open() || !stop.is_open() || !fout.is_open()) {
        cerr << "Error al abrir los archivos." << endl;
        return 1;
    }

    while (getline(stop, line)) {
        std::istringstream iss(line);
        string word;
        while (iss >> word) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            stopwords.insert(word);
        }
    }
    stop.close();

    while (getline(dict, line)) {
        std::istringstream iss(line);
        string word;
        while (iss >> word) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (stopwords.find(word) == stopwords.end()) {
                fout << word << endl;
            }
        }
    }
    dict.close();

    fout.close();
    return 0;
}