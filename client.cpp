#include <iostream>   // Include input-output stream library
#include <WinSock2.h> // Include Windows Sockets 2 library
#include <WS2tcpip.h> // Include Windows Sockets 2 TCP/IP library
using namespace std;  // Use the standard namespace
//  g++ client.cpp -o client.exe -lws2_32
//  Link with Windows Socket library - Required for socket programming in Windows
#pragma comment(lib, "ws2_32.lib")
// g++ client.cpp -o client.exe -lws2_32
//  Function to initialize WinSock
//  Returns true if initialization succeeds, false otherwise
bool Initialize()
{
    WSADATA data; // Structure to store WinSock data including version info and system status
    // Initialize WinSock version 2.2 - Returns 0 on success
    // MAKEWORD(2,2) requests version 2.2 of the Windows Sockets specification
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
int main(){
    if (!Initialize())
    {
        cout << "winsock initialization failed" << endl; // Print error message
        return 1;                                        // Exit program with error code
    }

    cout << "Hello Socket Programming from client program!\n"; // Indicate successful initialization

    // Create a TCP socket (SOCK_STREAM) for IPv4 (AF_INET)
    // The third parameter 0 lets the system choose the appropriate protocol (TCP)
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        cout << " socket creation failed " << endl; // Print error message
        WSACleanup();                             // Clean up Winsock to free resources
        return 1;                                 // Exit program with error code
    }
    string serveraddress = "127.0.0.1";
    // Define port number for the server to listen on
    int port = 12345; // Using port above 1024 as lower ports are privileged

    // Configure server address structure
    sockaddr_in serveraddr;            // IPv4 address structure
    serveraddr.sin_family = AF_INET;   // Set address family to IPv4
    serveraddr.sin_port = htons(port); // Convert port to network byte order (big-endian)

    // Convert string IP address "0.0.0.0" to binary form
    // 0.0.0.0 means the server will listen on all available network interfaces
    // if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
    // {
    //     cout << "setting address structure failed" << endl;
    //     closesocket(s); // Close the socket
    //     WSACleanup();              // Clean up WinSock
    //     return 1;                  // Exit with error
    // }
    // inet_pton(AF_INET,serveraddress.c_str(), &serveraddr.sin_addr);
    serveraddr.sin_addr.s_addr = inet_addr(serveraddress.c_str());
    if (serveraddr.sin_addr.s_addr == INADDR_NONE)
    {
        cout << "Invalid IP address format " << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    if (connect(s, reinterpret_cast<sockaddr *>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
    {
        cout<<" not able to connect to server "<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    cout<<"successfully connected to server "<<endl;
    //send/receive message 
    string message=" hello there ";
    if(send(s,message.c_str(),message.length(),0)==SOCKET_ERROR){
        cout<<"send failed"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    };
    closesocket(s);
    WSACleanup();
}