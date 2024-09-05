#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>
#include <windows.h>
#include "threadpool.h"

using namespace std;

int words_size = 0;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0, words_size - 1);

const long int N_FILES = 1000;
const long int POOL_SIZE = thread::hardware_concurrency();
const long int DATA_SIZE = 20000; // in Mb
vector<string> words;
string DIR = "data/inv_idx/";

void create_file(string filename, long int n_words);

int main(int argc, char *argv[]) {
    long int file_size = DATA_SIZE / N_FILES;
    long int n_words = file_size * 100000;

    ThreadPool pool(POOL_SIZE);
    vector<future<void>> futures;

    // Load dictionary    
    ifstream dict("data/dict.txt");
    string line;
    while (getline(dict, line)) {
        words.push_back(line);
    }
    dict.close();
    words_size = words.size();
    dis = uniform_int_distribution<>(0, words_size - 1);

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N_FILES; i++) {
        string filename = DIR + "file_" + to_string(i) + ".txt";
        futures.emplace_back(
            pool.enqueue([filename, n_words] { create_file(filename, n_words); })
        );
    }
    for (int i = 0; i < N_FILES; i++) {
        futures[i].get();
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time creating " << N_FILES << " files of " << file_size << " Mb each: " << elapsed.count() << " seconds" << endl;
}

void create_file(string filename, long int n_words) {
    ofstream fout(filename);
    for (long int i = 0; i < n_words; i++) {
        fout << words[dis(gen)] << " ";
    }
    fout.close();
}