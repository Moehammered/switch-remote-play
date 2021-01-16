//
//  Process.cpp
//  switch-remote-play-host
//
//  Created by Mouhamad Abdallah on 9/1/21.
//

#include <stdio.h>
#include "Process.h"
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

Process::Process()
{
    processID = -1;
}

Process::Process(std::string command, std::vector<std::string> args)
    : processID(-1)
{
    processID = CreateProcess(command, args, false);
}

Process::Process(std::string command, std::vector<std::string> args, bool removeStdOut)
{
    processID = CreateProcess(command, args, removeStdOut);
}

Process::~Process()
{
//    if(processID > 0)
//        Stop();
}

bool Process::Running() const
{
    return processID > 0;
}

bool Process::Stop()
{
    return kill(processID, SIGTERM) == 0;
}

void Process::Wait()
{
    int status = 0;
    waitpid(processID, &status, 0);
}

pid_t Process::CreateProcess(std::string command, std::vector<std::string> args, bool removeStdOut)
{
    auto process = fork();
    if(process == 0)
    {
        //subprocess
        //ffmpeg args
        //./ffmpeg-arm -f avfoundation -i "1:0" output.mkv
        //char * arg[] = {"ffmpeg-arm", "-f", "avfoundation", "-i", "1",  "output.mkv", (char *)0};
        if(removeStdOut)
        {
            auto nullfd = open("/dev/null", O_WRONLY);
            if(nullfd == -1)
                std::cout << "Faile to open null fd\n\n";
            else
            {
                dup2(nullfd, STDOUT_FILENO);
                dup2(nullfd, STDERR_FILENO);
                close(nullfd);
            }
        }
        std::vector<char*> cstrings{};

        for(auto& string : args)
            cstrings.push_back(&string.front());
        
        cstrings.push_back(nullptr);

        auto result = execv(command.c_str(), &cstrings[0]);
//        auto args = " -f avfoundation -i \"1:0\" output.mkv";
//        auto result = execl(path.c_str(),path.c_str() , args, 0);
        int processStatus = 0;
        wait(&processStatus);
        std::cout << "Result of exec: " << result << " exited with status " << processStatus << "\n";
        exit(1);
    }
    else if(process < 0)
    {
        //error occured
        std::cout << "Failed to open: " << command << "\n\n";
    }
    else
    {
        //we got a process handle here
        std::cout << "Process - " << command << " - with args (";
        for(auto& arg : args)
            std::cout << arg << " ";
        std::cout << ") start with PID " << process << "\n";
    }
    
    return process;
}
