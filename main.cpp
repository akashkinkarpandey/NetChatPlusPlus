#include <iostream>   // Include input-output stream library
#include <WinSock2.h> // Include Windows Sockets 2 library
#include <WS2tcpip.h> // Include Windows Sockets 2 TCP/IP library
#include <tchar.h>    // Include tchar.h header file
using namespace std;  // Use the standard namespace

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib library

// Function to initialize WinSock
bool Initialize()
{
    WSADATA data; // Structure to store WinSock data
    // Initialize WinSock version 2.2
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

int main()
{
    // Check if WinSock initialization was successful
    if (!Initialize())
    {
        cout << "winsock initialization failed" << endl; // Print error message
        return 1;                                        // Exit program
    }

    cout << "Hello Socket Programming!"; // Print success message
    // Create a TCP socket for IPv4 addresses
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "socket creation failed" << endl; // Print error message
        WSACleanup();                             // Clean up Winsock
        return 1;                                 // Exit program
    }
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;//address should be IPv4
    serveraddr.sin_port = htons(12345);//port number being converted to network byte order
    
    
    return 0; // Exit program
}