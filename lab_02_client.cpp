#include <map>
#include <sstream>
#include <iostream>
#include <windows.h>

using namespace std;

const size_t command_size = 64;

int main()
{

    string path_name;
    cout << "Enter pipe name (without `\\.\pipe\'): ";
    cin >> path_name;
    string path = "\\\\.\\pipe\\" + path_name;
    HANDLE bCreateFile = CreateFile(path.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (bCreateFile == INVALID_HANDLE_VALUE)
        if (bCreateFile == FALSE)
        {
            cout<<"Error- "<<GetLastError()<<endl;
        }

    DWORD numberOfBytes;
    string command (64, '\0');
    while (true)
    {
        cout << "> ";
        getline(cin >> ws, command);
        if (WriteFile(bCreateFile, command.c_str(), command.size(), &numberOfBytes, nullptr) == FALSE)
        {
            cout<<"Error- "<<GetLastError()<<endl;
        }


        if (command == "quit")
        {
            if (CloseHandle(bCreateFile) == FALSE)
        {
            cout<<"Error- "<<GetLastError()<<endl;
        }
            break;
        }
        else
        {
            std::string response (64, '\0');
            if (ReadFile(bCreateFile, &response[0], response.size(), &numberOfBytes, nullptr) == FALSE)
            {
                cout<<"Error- "<<GetLastError()<<endl;
            }
        }
        command.replace(0, command.size(), command.size(), '\0');
    }
}
