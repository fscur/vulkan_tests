#pragma once

#include <string>
#include <fstream>

class shaderLoader
{
private:
    shaderLoader();
public:
    static std::string load(std::string filepath)
    {
        std::string fileString = std::string();
        std::string line = std::string();

        std::ifstream file(filepath);

        if (file.is_open())
        {
            while (!file.eof())
            {
                getline(file, line);
                fileString.append(line);
                fileString.append("\n");
            }

            file.close();
        }

        return fileString;
    }
};

