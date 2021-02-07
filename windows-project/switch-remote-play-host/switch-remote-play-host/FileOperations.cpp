#include "FileOperations.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>

bool readFileAsText(std::string path, std::string& data)
{
	#ifdef USE_EXCEP
    std::ifstream fileStream;
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try{
		fileStream.open(path);
		std::stringstream fileString;
		fileString << fileStream.rdbuf();
		fileStream.close();

		return fileString.str();
	} 
	catch(std::exception const& e)
	{
		std::cout << "Error occured with exception:\n" << e.what() << std::endl;
	}
	return "";
	#endif
	#ifndef USE_EXCEP
    std::ifstream fileStream;
	fileStream.open(path);
    if(fileStream.good())
    {
        std::stringstream fileString;
        fileString << fileStream.rdbuf();
        fileStream.close();
        data = fileString.str();
        return true;
    }
    else
    {
        fileStream.close();
        return false;
    }
	#endif
}

bool saveTextToFile(std::string path, std::string const data)
{
    std::ofstream fileStream;
	fileStream.open(path);
    if(fileStream.is_open())
    {
        fileStream.write(data.c_str(), data.length());
        fileStream.close();
        return true;
    }
    else
    {
        fileStream.close();
        return false;
    }
    return false;
}
