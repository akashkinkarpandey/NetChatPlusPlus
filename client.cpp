#include <iostream>   // Include input-output stream library
#include <WinSock2.h> // Include Windows Sockets 2 library
#include <WS2tcpip.h> // Include Windows Sockets 2 TCP/IP library
#include <thread>
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
void sendMessage(SOCKET s)
{
    //s is client socket holding server's IP address and port 
    //we are sending message from current client socket s
    //all other client sockets and the server itself receive the message as well
    cout << "enter your own name :" << endl;
    string name;
    getline(cin, name);
    string message;
    // loop keeps running until there is any Socket error or user types 'exit'
    while (true)
    {
        cout << "Enter your new message or Type exit to exit chat" << endl;
        getline(cin, message);
        string msgToBeSent = name + " : " + message;
        int bytesent = send(s, msgToBeSent.c_str(), msgToBeSent.length(), 0);
        if (bytesent == static_cast<SOCKET>(SOCKET_ERROR))
        {
            cout << "error sending message" << endl;
            break;
        }
        if (message == "exit")
        {
            //user wants to exit chat
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}
void receiveMessage(SOCKET s)
{
    //when some client sends a message 
    //server receives the message
    //then the servers sends the message to other clients
    //so this socket s here is a client socket 
    //it listens to server for any incoming message which server may have got from other client
    char buffer[4096];
    int bytesrecvd;
    string msg = "";
    while (true)
    {
        bytesrecvd = recv(s, buffer, sizeof(buffer), 0);
        if (bytesrecvd > 0)
        {
            string message(buffer, bytesrecvd);
            cout << message << endl;
        }
        else if (bytesrecvd == 0)
        {
            cout << "Client disconnected." << endl;
            break; // to break out of loop
            // ... handle client disconnection (e.g., close the socket) ...
        }
        else if (bytesrecvd == static_cast<SOCKET>(SOCKET_ERROR))
        {
            cerr << "Error receiving data: " << WSAGetLastError() << endl; // Get specific error code
            // ... handle error (e.g., close the socket) ...
            closesocket(s);
            WSACleanup(); // Clean up WinSock
            return;       // to return back to main
        }
    }
    closesocket(s);
    WSACleanup();
}
int main()
{
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
        WSACleanup();                               // Clean up Winsock to free resources
        return 1;                                   // Exit program with error code
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
        cout << " not able to connect to server " << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    cout << "successfully connected to server " << endl;
    // send/receive message
    //  string message=" hello there ";
    //  if(send(s,message.c_str(),message.length(),0)==static_cast<SOCKET>(SOCKET_ERROR)){
    //      cout<<"send failed"<<endl;
    //      closesocket(s);
    //      WSACleanup();
    //      return 1;
    //  };

    //2 threads will independently send/receive messages
    thread senderthread(sendMessage, s);
    thread receiverthread(receiveMessage, s);
    senderthread.join();
    receiverthread.join();
    //main thread reaches here only after above 2 threads have finished executing
    //cleaning up resources
    closesocket(s);
    WSACleanup();
}