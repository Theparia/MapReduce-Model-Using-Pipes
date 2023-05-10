#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define MAX_SIZE 4096

using namespace std;

vector<string> split(char keyValuePairs[])
{
    vector<string> tokens;
    string token;
    stringstream ss(keyValuePairs);
    while (getline(ss, token, ','))
    {
        tokens.push_back(token);
    }
    return tokens;
}

map<string, int> getMapOfWords(vector<string> tokens, map<string, int> words)
{
    for (int i = 1; i < tokens.size(); i += 2)
    {
        if (words.find(tokens[i - 1]) == words.end())
            words[tokens[i - 1]] = atoi(tokens[i].c_str());
        else
            words[tokens[i - 1]] += atoi(tokens[i].c_str());
    }
    return words;
}

string toString(map<string, int> words){
    string result = "";
    for (auto w : words)
    {
        result += w.first + "," + to_string(w.second) + "\n"; 
    }
    return result;
}

int main(int argc, char *argv[])
{
    int fileCount = atoi(argv[1]);
    map<string, int> words;
    char keyValuePairs[MAX_SIZE];

    //read key-value pairs from named pipes(map process <=> reduce process)
    for (int i = 0; i < fileCount; i++)
    {
        string fifo = "./fifo" + to_string(i + 1);
        int fd = open(fifo.c_str(), O_RDONLY);
        read(fd, keyValuePairs, MAX_SIZE);
        vector <string> tokens = split(keyValuePairs);
        words = getMapOfWords(tokens, words);
        close(fd);
    }

    //write final output to unnamed pipe(reduce process <=> main process)
    string result = toString(words);
    int fd = atoi(argv[2]);
    write(fd, result.c_str(), MAX_SIZE);
    close(fd);
    return 0;
}