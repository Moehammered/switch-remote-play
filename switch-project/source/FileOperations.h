#ifndef __FILEOPERATIONS_H__
#define __FILEOPERATIONS_H__

#include <string>
#include <vector>

/*
    Need to add directory validation
    ability to create missing directories when writing
*/

bool readFileAsText(std::string path, std::string& data);

bool saveTextToFile(std::string path, std::string const data);

std::string getApplicationDirectory();

std::vector<std::string> readApplicationDirectory();

std::vector<std::string> readRootDirectory();

bool directoryExists(std::string parentDirectory, std::string directoryName);

bool directoryExists(std::string fullPath);

bool createDirectory(std::string parentDirectory, std::string directoryName);

#endif
