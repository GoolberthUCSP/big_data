#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <locale>
#include <set>
#include <sstream>
#include <chrono>
#include <utility>
#include <unordered_map>
#include "threadpool.h"

using namespace std;

const int N_FILES = 1000;
const long long int N_THREADS = 32;
const long long int BUFFER_SIZE = 512 * 1024;
const int POOL_SIZE = thread::hardware_concurrency();

void linear_word_counter(string filename, unordered_map<string, long long int> &result);
void word_counter(string filename, unordered_map<string, long long int> &result);
void process_chunk(string filename, long long int start, long long int end, unordered_map<string, long long int> &counter);
string DIR = "data/inv_idx/";

int main() {
    setlocale(LC_ALL, "es_CO.UTF-8");
    vector<unordered_map<string, long long int>> word_counts(N_FILES);
    
    ThreadPool pool(POOL_SIZE);
    vector<future<void>> futures;
    // PROCESS WORD COUNTS
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N_FILES; i++) {
        string filename = DIR + "file_" + to_string(i) + ".txt";
        futures.emplace_back(
            pool.enqueue([filename, &word_counts, i] {
                linear_word_counter(filename, word_counts[i]);
            })
        );
    }
    for (int i = 0; i < N_FILES; i++) {
        futures[i].get();
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time processing word_counts   : " << elapsed.count() << " seconds" << endl;
    futures.clear();
    
    // PROCESS INVERTED INDEX
    std::unordered_map<std::string, std::multiset<std::pair<int, int>, greater<>>> inverted_index;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N_FILES; i++) {
        for (auto it = word_counts[i].begin(); it != word_counts[i].end(); it++) {
            inverted_index[it->first].insert({it->second, i});
        }
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;

    cout << "Elapsed time processing inverted_index: " << elapsed.count() << " seconds" << endl;
    
    // Save inverted index
    ofstream fout("data\\inv_idx\\inv_idx.txt");
    for (auto it : inverted_index){
        fout << it.first << ',' << it.second.size() << ',';
        for (auto it2 : it.second){
            fout << it2.first << ',' << it2.second << ',';
        }
        fout << endl;
    }
    fout.close();

    string query;
    while (true) {
        cout << "Enter a word to search: ";
        getline(cin, query);
        if (query == "exit") {
            break;
        }
        auto it = inverted_index.find(query);
        if (it == inverted_index.end()) {
            cout << "Word not found" << endl;
            continue;
        }
        int i = 0;    
        for (auto it2 : it->second) {
            if (i == 20)
                break;
            cout << "file_" << it2.second << ".txt " << it2.first << endl;
            i++;
        }
    }
}

void linear_word_counter(string filename, unordered_map<string, long long int> &result) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    stringstream ss;
    ss << file.rdbuf();
    string word;
    while (ss >> word) {
        result[word]++;
    }
    file.close();
}

void word_counter(string filename, unordered_map<string, long long int> &result) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    file.seekg(0, ios::end);
    long long int size = file.tellg();
    
    unordered_map<string, long long int> counter[N_THREADS];
    long long int chunk = size / N_THREADS;
    thread threads[N_THREADS];

    long long int last_cut = 0;
    long long int cut = chunk;
    for (long long int i = 0; i < N_THREADS - 1; i++) {
        file.seekg(cut, ios::beg);
        while (file.get() != ' ') {
            cut++;
        }
        threads[i] = thread(process_chunk, filename, last_cut, cut, ref(counter[i]));
        last_cut = cut;
        cut += chunk;
    }
    threads[N_THREADS - 1] = thread(process_chunk, filename, last_cut, size, ref(counter[N_THREADS - 1]));
    file.close();

    for (int i = 0; i < N_THREADS; i++) {
        threads[i].join();
    }

    for (int i = 0; i < N_THREADS; i++) {
        for (auto it = counter[i].begin(); it != counter[i].end(); it++) {
            result[it->first] += it->second;
        }
    }
}

void process_chunk(string filename, long long int start, long long int end, unordered_map<string, long long int> &counter) {
    ifstream file(filename);
    char buffer[BUFFER_SIZE];
    string word;
    file.seekg(start, ios::beg);

    while (file.tellg() < end) {
        long long int bytes_to_read = min((long long int)BUFFER_SIZE, end - file.tellg());
        file.read(buffer, bytes_to_read);

        for (long long int i = 0; i < bytes_to_read; ++i) {
            if (buffer[i] == ' ' || buffer[i] == '\0') {
                if (!word.empty()) {
                    counter[word]++;
                    word.clear();
                }
            } else {
                word += buffer[i];
            }
        }
    }
    if (!word.empty()) {
        counter[word]++;
    }
    file.close();
}