#define DEBUG 0

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <string>

#define MAKEFILE_NOT_FOUND -1
#define SUCCESS             0
//Makefile build options
#define BUILD 1
#define RUN   2

int LoadMakeFile(std::ifstream&);
void GetDefaultCommand(std::ifstream&, std::string&);
void LoadFlagMap(std::ifstream&, std::unordered_map<std::string, std::string>&);

void RemoveTab(std::string& str)
{
    if(str[0] == '\t')
        str.erase(str.begin());
    else
    {
        str.erase(str.begin());
        str.erase(str.begin());
        str.erase(str.begin());
        str.erase(str.begin());
    }
}

int main(int argc, char const *argv[])
{

    #if DEBUG
    argc = 2;
    char* argv[] = {"make.exe", "BUILD"};
    #endif

    //Create a file stream to store the makefile
    std::ifstream makefile;
    //Load the makefile
    auto status = LoadMakeFile(makefile);
    //Check if the file was opened
    if(status == MAKEFILE_NOT_FOUND)
    {
        fprintf(stderr, "Makefile not found in this directory!\n");
        exit(EXIT_FAILURE);
    }


    //If no flag is assigned
    if(argc == 1)
    {
        //Run the first flag -- it will be considered the default one, to change the action the user will have to change the file(makefile)
        std::string command;
        GetDefaultCommand(makefile, command);
        printf("%s\n", command.c_str());
        status = system(command.c_str());
    }
    else
    {
        //Load all the flags specified inside the makefile in an unordered_map, and also the associated string content, which is the command to execute
        std::unordered_map<std::string, std::string> commandMap;
        LoadFlagMap(makefile, commandMap);
        std::string_view command;

        //Stop reading the makefile
        makefile.close();
        
        //Run the flags passed by the user one by one
        int i = 1;
        while(i < argc)
        {
            command = commandMap[argv[i]]; 
            if(command.length())
            {
                printf("%d:>> %s\n", i, command.data());
                status = system(command.data());
            }    
            else
                fprintf(stderr, "[Error: $(%s)] flag not found in makefile\n", argv[i]);
            i++;
        }
    }
    
    if(makefile.is_open())
        makefile.close();
    return status;
}

int LoadMakeFile(std::ifstream& makefile)
{
    makefile.open("Makefile");
    if(!makefile.is_open())
    {
        makefile.close();
        return MAKEFILE_NOT_FOUND;
    }

    return SUCCESS;
}

void GetDefaultCommand(std::ifstream& makefile, std::string& command)
{
    std::string temp;
    while(getline(makefile, temp))
    {
        //if the line is a flagline (has a $ on it)
        if(temp[0] == '$')
            continue;
        
        //Check if the string is not empty --> else it means the end of command
        if(temp.size())
        {
            //delete the first character which is  '\t' --> 4spaces
            RemoveTab(temp);
            command += temp + ' ';
        }
        else
            break;
        
    }

    //remove the last reduntant space
    command.erase(command.end() - 1);

}

void LoadFlagMap(std::ifstream& file, std::unordered_map<std::string, std::string>& commandMap)
{
    std::string temp, command;
    std::string currentFlag;
    while(getline(file, temp))
    {
        //If it is a flagline then read the flag and set the currentFlag 
        if(temp[0] == '$')
        {
            temp.erase(temp.begin());
            temp.erase(temp.begin());
            temp.erase(temp.end() - 1);

            currentFlag = temp;
            continue;
        }
        else if(temp.size())  //if the line read is not a blank line
        {
            //delete the first character which is \t -->4spaces
            RemoveTab(temp);

            command += temp + ' ';
        }
        else
        {
            //remove the last reduntant space
            command.erase(command.end() - 1);
            commandMap[currentFlag] = command;
            command.clear();
        }
    }

    // //Map the last command after file reading closed
    // while(ss)
    // {
    //     ss >> temp;
    //     command += temp + ' ';
    // }
    
    //remove the last reduntant space
    command.erase(command.end() - 1);
    commandMap[currentFlag] = command;
}