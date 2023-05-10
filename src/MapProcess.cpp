#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

#define MAX_SIZE 4096

using namespace std;

map<string, int> getMapOfWords(string fileName)
{
    map<string, int> words;
    ifstream file("./testcases/" + fileName);
    string line, word;
    getline(file, line);
    stringstream ss(line);
    while (getline(ss, word, ','))
    {
        if (words.find(word) == words.end())
            words[word] = 1;
        else
            words[word]++;
    }
    return words;
}

string toString(map<string, int> words)
{
    string result = "";
    for (auto w: words)
    {
        result += w.first + "," + to_string(w.second) + ",";
    }
    return result;
}

int main(int argc, char *argv[])
{
    string fileName = argv[1];
    map<string, int> words = getMapOfWords(fileName);
    string result = toString(words);

    //write key-value pairs to named pipe(map process <=> reduce process)
    string fifo = "./fifo" + fileName.substr(0, fileName.size() - 4);
    int fd = open(fifo.c_str(), O_WRONLY);
    write(fd, result.c_str(), MAX_SIZE);
    close(fd);
    return 0;
}