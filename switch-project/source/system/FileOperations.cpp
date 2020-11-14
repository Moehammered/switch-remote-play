#include "FileOperations.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <dirent.h>
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

std::string getApplicationDirectory()
{
    auto dir = opendir("");
    if(dir != nullptr)
    {
        auto entry = readdir(dir);
        if(entry != nullptr)
        {
            closedir(dir);
            return entry->d_name;
        }
        else
        {
            closedir(dir);
            return std::string{};
        }
    }
    else
        return std::string{};
}

std::vector<std::string> readApplicationDirectory()
{
    auto dir = opendir("");
    if(dir != nullptr)
    {
        auto entry = readdir(dir);
        auto entries = std::vector<std::string>{};
        if(entry != nullptr)
        {
            do
            {
                entries.push_back(entry->d_name);
                entry = readdir(dir);
            } while (entry != nullptr);
            
            closedir(dir);
            return entries;
        }
        else
        {
            closedir(dir);
            return std::vector<std::string>{};
        }
    }
    else
        return std::vector<std::string>{};
}

std::vector<std::string> readRootDirectory()
{
    auto dir = opendir("sdmc:/");
    if(dir != nullptr)
    {
        auto entry = readdir(dir);
        auto entries = std::vector<std::string>{};
        if(entry != nullptr)
        {
            do
            {
                entries.push_back(entry->d_name);
                entry = readdir(dir);
            } while (entry != nullptr);
            
            closedir(dir);
            return entries;
        }
        else
        {
            closedir(dir);
            return std::vector<std::string>{};
        }
    }
    else
        return std::vector<std::string>{};
}

bool directoryExists(std::string parentDirectory, std::string directoryName)
{
    auto dir = opendir(parentDirectory.c_str());
    if(dir != nullptr)
    {
        auto entry = readdir(dir);
        do
        {
            if(entry->d_type == DT_DIR && entry->d_name == directoryName)
            {
                closedir(dir);
                return true;
            }
            entry = readdir(dir);
        } while (entry != nullptr);
    }

    closedir(dir);
    return false;
}

bool directoryExists(std::string fullPath)
{
    auto dir = opendir(fullPath.c_str());
    if(dir != nullptr)
    {
        closedir(dir);
        return true;
    }
    else
        return false;
}

bool createDirectory(std::string parentDirectory, std::string directoryName)
{
    auto fullPath = parentDirectory + "/" + directoryName;
    return mkdir(fullPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) >= 0;
}
