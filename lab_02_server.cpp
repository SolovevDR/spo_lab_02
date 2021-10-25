#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

const size_t command_size = 64;

int main()
{
    string path_name;
    string path;

    BOOL bConnectNamePipe;

    cout << "Enter pipe name (without `\\.\pipe\'): ";
    cin >> path_name;

    path = "\\\\.\\pipe\\" + path_name;

    HANDLE hcreateNamePipe = CreateNamedPipe(path.c_str(),
                                        PIPE_ACCESS_DUPLEX,
                                        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                        PIPE_UNLIMITED_INSTANCES,
                                        64, 64, 0, nullptr);

    if (hcreateNamePipe == INVALID_HANDLE_VALUE)
    {
        cout<<"NamedPipe Creation Failed & Error No - "<<GetLastError()<<endl;
    }
    else
    {
            cout << "NamedPipe Creation Success" << endl;
    }


    bConnectNamePipe = ConnectNamedPipe(hcreateNamePipe, NULL);
    if (bConnectNamePipe == FALSE)
    {
        cout<<"Connection Failed & Error No - "<<GetLastError()<<endl;
    }
    else
    {
        cout<<"Connection Success"<<endl;
        cout<<"Waiting for a command";
    }


    DWORD numberOfBytes;
    while (true)
    {
        cout << "Awaiting for client-to-pipe connection...\n";

            if (bConnectNamePipe == FALSE)
            {
                cout<<"Error- "<<GetLastError()<<endl;
            }
            else
            {
                cout<<"Connection Success"<<endl;
                cout<<"Waiting for a command";
            }

        string command (command_size, '\0');
        string keyword, key, value, response {};
        map < string,
        string> data {};

        while (true)
        {
            cout << "Awaiting for client command...\n";


            if (ReadFile(hcreateNamePipe, &command[0], command.size(), &numberOfBytes, nullptr) == FALSE)
            {
                cout<<"Error - "<<GetLastError()<<endl;
            }

            command.resize(command.find('\0'));

            std::istringstream parser {command};
            parser >> ws >> keyword;

            if (keyword == "set")
            {
                parser >> key >> value;
                data[key] = value;
                response = "acknowledged";
            }
            else if (keyword == "get")
            {
                parser >> key;
                if (data.find(key) != data.end())
                    response = "found " + data[key];
                else
                    response = "missing";
            }
            else if (keyword == "list")
            {
                for (auto i = data.begin(); i != data.end(); ++ i)
                    response += i->first + " ";
            }
            else if (keyword == "delete")
            {
                parser >> key;
                auto del = data.find(key);
                if (del != data.end())
                {
                    data.erase(del);
                    response = "deleted";
                }
                else
                    response = "missing";
            }
            else if (keyword == "quit")
            {

                if (DisconnectNamedPipe(hcreateNamePipe) == FALSE)
                {
                    cout<<"Error - "<<GetLastError()<<endl;
                }
                command.replace(0, command.size(), command.size(), '\0');
                command.resize(command_size, '\0');
                break;

            }
            else
            {
                cerr << "Incorrect command! (command: \"" << command << "\")\n";
                response = "incorrect command";
            }


                if (WriteFile(hcreateNamePipe, response.c_str(), response.size(), &numberOfBytes, nullptr) == FALSE)
                {
                    cout<<"Error - "<<GetLastError()<<endl;
                }

            command.replace(0, command.size(), command.size(), '\0');
            command.resize(command_size, '\0');
            response.clear();
            keyword.clear();
            key.clear();
            value.clear();
        }

        char kill;
        bool exit = false;
        cout << "Do you want to destroy pipe \"" << path_name << "\" (y/n)?: ";
        while (cin >> kill)
        {
            if (kill == 'y')
            {

                if (CloseHandle(hcreateNamePipe) == FALSE)
                {
                    cout<<"Error - "<<GetLastError()<<endl;
                }

                exit = true;
                break;
            }
            else if (kill == 'n')
                break;
            else
            {
                std::cout << "(y/n): ";
                continue;
            }
        }
        if (exit)
            break;
    }
}
