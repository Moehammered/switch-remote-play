//
//  Process.h
//  switch-remote-play-host
//
//  Created by Mouhamad Abdallah on 9/1/21.
//

#ifndef __Process_h__
#define __Process_h__

#include <string>
#include <vector>
#include <sys/types.h>

class Process
{
public:
    Process();
    Process(std::string command, std::vector<std::string> args);
    Process(std::string command, std::vector<std::string> args, bool removeStdOut);
    ~Process();
    
    bool Running() const;
    
    bool Stop();
    
    void Wait();
    
private:
    pid_t processID;
    
    pid_t CreateProcess(std::string command, std::vector<std::string> args, bool removeStdOut);
};


#endif /* Process_h */
