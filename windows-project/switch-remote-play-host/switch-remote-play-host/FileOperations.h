#ifndef __FILEOPERATIONS_H__
#define __FILEOPERATIONS_H__

#include <string>
#include <vector>

bool readFileAsText(std::string path, std::string& data);

bool saveTextToFile(std::string path, std::string const data);

#endif
