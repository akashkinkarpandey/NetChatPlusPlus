#include <iostream>   // Include input-output stream library
#include <WinSock2.h> // Include Windows Sockets 2 library
#include <WS2tcpip.h> // Include Windows Sockets 2 TCP/IP library
#include <tchar.h>    // Include tchar.h header file for Unicode/ANSI string handling
#include <thread>
#include <vector>
using namespace std;  // Use the standard namespace
//  g++ server.cpp -o server.exe -lws2_32
//  Link with Windows Socket library - Required for socket programming in Windows
#pragma comment(lib, "ws2_32.lib")

// Function to initialize WinSock
// Returns true if initialization succeeds, false otherwise
bool Initialize()
{
    WSADATA data; // Structure to store WinSock data including version info and system status
    // Initialize WinSock version 2.2 - Returns 0 on success
    // MAKEWORD(2,2) requests version 2.2 of the Windows Sockets specification
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void InteractWithClient(SOCKET clientSocket,vector<int>&allClientSockets)
{
    // send/recv client
    char buffer[4096];
    while (1)
    {
        int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesrecvd > 0)
        {
            string message(buffer, bytesrecvd);
            cout << "Message from client: " << message << endl;
            for(auto &client:allClientSockets){
                if(client!=clientSocket){
                    send(client,message.c_str(),message.length(),0);
                }
            }
        }
        else if (bytesrecvd == 0)
        {
            cout << "Client disconnected." << endl;
            break;//to break out of loop
            // ... handle client disconnection (e.g., close the socket) ...
        }
        else if (bytesrecvd == SOCKET_ERROR)
        {
            cerr << "Error receiving data: " << WSAGetLastError() << endl; // Get specific error code
            // ... handle error (e.g., close the socket) ...
            closesocket(clientSocket);
            WSACleanup(); // Clean up WinSock
            return;       // to return back to main
        }
    }
    auto it = find(allClientSockets.begin(), allClientSockets.end(), clientSocket);
    if (it != allClientSockets.end()){
        allClientSockets.erase(it);
    }
    closesocket(clientSocket);
}
int main()
{
    // Initialize Windows Sockets API before any socket operations
    if (!Initialize())
    {
        cout << "winsock initialization failed" << endl; // Print error message
        return 1;                                        // Exit program with error code
    }

    cout << "Hello Socket Programming! from server program"; // Indicate successful initialization

    // Create a TCP socket (SOCK_STREAM) for IPv4 (AF_INET)
    // The third parameter 0 lets the system choose the appropriate protocol (TCP)
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "socket creation failed" << endl; // Print error message
        WSACleanup();                             // Clean up Winsock to free resources
        return 1;                                 // Exit program with error code
    }

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
    //     closesocket(listenSocket); // Close the socket
    //     WSACleanup();              // Clean up WinSock
    //     return 1;                  // Exit with error
    // }
    serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (serveraddr.sin_addr.s_addr == INADDR_NONE)
    {
        cout << "Invalid IP address format" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Bind the socket to the specified address and port
    // Cast serveraddr to struct sockaddr* as bind() expects this type
    if (bind(listenSocket, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
        cout << "bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup(); // Clean up WinSock
        return 1;
    }

    // Start listening for client connections
    // SOMAXCONN specifies the maximum length for the queue of pending connections
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "listen failed" << endl;
        closesocket(listenSocket);
        WSACleanup(); // Clean up WinSock
        return 1;
    }

    cout << " Server is listening on port " << port << endl;

    // Accept a connection from a client
    // nullptr parameters mean we're not interested in client's address information
    // This call blocks until a client connects
    // Main server loop to accept multiple clients
    int count=0;
    vector<SOCKET>allClientSockets;
    while (true)
    {
        // Accept a connection
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == static_cast<SOCKET>(SOCKET_ERROR))
        {
            cout << "invalid client socket" << endl;
            continue;
        }
        count++;
        allClientSockets.push_back(clientSocket);
        cout << "Client "<<count<<" connected! " << endl;
        // Create a new thread to handle the client
        thread t1(InteractWithClient, clientSocket, ref(allClientSockets));
        t1.detach(); // Detach the thread to allow it to run independently
    }
    // Clean up Windows Sockets API resources
    closesocket(listenSocket);
    WSACleanup();
    return 0; // Exit program successfully
}