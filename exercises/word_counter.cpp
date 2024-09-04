#include <fstream>
#include <string>
#include <unordered_map>
#include <thread>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <locale>

using namespace std;

string filename = "file.txt";
const long long int N_THREADS = 32; // thread::hardware_concurrency();
const long long int BUFFER_SIZE = 512 * 1024;

void word_counter(unordered_map<string, long long int> &result);
void process_chunk(long long int start, long long int end, unordered_map<string, long long int> &counter);

int main() {
    setlocale(LC_ALL, "es_CO.UTF-8");
    unordered_map<string, long long int> result;

    auto start = chrono::high_resolution_clock::now();
    word_counter(result);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;

    for (auto it = result.begin(); it != result.end(); it++) {
        cout << it->first << ": " << it->second << endl;
    }
}

void word_counter(unordered_map<string, long long int> &result) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    file.seekg(0, ios::end);
    long long int size = file.tellg();
    cout << "File size: " << size << " bytes" << endl;
    
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
        threads[i] = thread(process_chunk, last_cut, cut, ref(counter[i]));
        last_cut = cut;
        cut += chunk;
    }
    threads[N_THREADS - 1] = thread(process_chunk, last_cut, size, ref(counter[N_THREADS - 1]));
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

void process_chunk(long long int start, long long int end, unordered_map<string, long long int> &counter) {
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