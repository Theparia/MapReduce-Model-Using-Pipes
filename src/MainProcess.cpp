#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <string>
#include <stdlib.h>
#include <wait.h>
#include <fstream>

#define READ_END 0
#define WRITE_END 1
#define MAX_SIZE 4096

using namespace std;

int getNumberOfCSVFiles()
{
    DIR *d = opendir("./testcases");
    struct dirent *dir;
    int fileCount = 0;
    while ((dir = readdir(d)) != NULL)
    {
        if (dir->d_type == DT_REG)
        {
            fileCount++;
        }
    }
    closedir(d);
    return fileCount;
}

int main(int argc, char *argv[])
{
    int fileCount = getNumberOfCSVFiles();
    int pipes[fileCount + 1][2];
    string fifos[fileCount];

    //create pipes
    for (int i = 0; i <= fileCount; i++)
    {
        //named pipe
        if (i < fileCount)
        {
            fifos[i] = "./fifo" + to_string(i + 1);
            mkfifo(fifos[i].c_str(), 0666);
        }
        //unnamed pipe
        if (pipe(pipes[i]) < 0)
        {
            cout << "Pipe Failed!\n";
            return 0;
        }
    }

    //create map processes
    for (int i = 0; i < fileCount; i++)
    {
        string fileName = to_string(i + 1) + ".csv";
        int pid = fork();
        if (pid < 0)
        {
            cout << "Fork Failed!\n";
            return 0;
        }
        else if (pid == 0) //child
        {
            //read name of CSV file from unnamed pipe(main process <=> map processor) and send it to map processor
            close(pipes[i][WRITE_END]);
            char CSVFile[MAX_SIZE];
            read(pipes[i][READ_END], CSVFile, MAX_SIZE);
            string exec = "./MapProcess.out";
            char *args[] = {(char *)exec.c_str(), CSVFile, NULL};
            close(pipes[i][READ_END]);
            execvp(args[0], args);
        }
        else //parent
        {
            //write name of CSV file to unnamed pipe(main process <=> map processor)
            close(pipes[i][READ_END]);
            write(pipes[i][WRITE_END], fileName.c_str(), MAX_SIZE);
            close(pipes[i][WRITE_END]);
        }
    }

    //create reduce process
    int pid = fork();
    if (pid < 0)
    {
        cout << "Fork Failed\n";
        return 0;
    }
    else if (pid == 0) //child
    {
        //send number of csv files and write-end of unnamed pipe(reduce process <=> main process) to reduce processor
        string exec = "./ReduceProcess.out";
        close(pipes[fileCount][READ_END]);
        char *args[] = {(char *)exec.c_str(), (char *)to_string(fileCount).c_str(),
                        (char *)to_string(pipes[fileCount][WRITE_END]).c_str(), NULL};
        execvp(args[0], args);
    }
    else //parent
    {
        //read final output from unnamed pipe(reduce process <=> main process)
        waitpid(pid, NULL, 0);
        close(pipes[fileCount][WRITE_END]);
        char result[MAX_SIZE] = {0};
        read(pipes[fileCount][READ_END], result, MAX_SIZE);
        fstream file;
        file.open("output.csv", ios::out | ios::app);
        file << result;
        close(pipes[fileCount][READ_END]);
    }

    return 0;
}