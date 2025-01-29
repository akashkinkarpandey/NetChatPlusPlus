#include <iostream>     // Include input-output stream library
#include <WinSock2.h>   // Include Windows Sockets 2 library
using namespace std;    // Use the standard namespace

#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib library

// Function to initialize WinSock
bool Initialize()
{
    WSADATA data;  // Structure to store WinSock data
    // Initialize WinSock version 2.2
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

int main()
{
    // Check if WinSock initialization was successful
    if (!Initialize())
    {
        cout << "winsock initialization failed" << endl;  // Print error message
        return 0;  // Exit program
    }

    cout << "Hello Socket Programming!";  // Print success message

    WSACleanup();  // Clean up WinSock
    return 0;  // Exit program
}