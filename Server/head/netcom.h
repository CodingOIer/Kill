#include <cstring>
#include <iostream>
#include <winsock2.h>
/**
 *@Send函数实现向ip的port的端口发送msg字符串
 *@Listen函数实现监听来自port的端口的数据并返回，阻塞式
 *@ListenNoWait函数实现监听来自port的端口的数据并返回，等待100ms
 */
namespace net
{
bool Send(const char *ip, int port, const char *msg)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    u_long mode = 1;
    if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0)
    {
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    int result = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(clientSocket, &writeSet);
        timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(0, NULL, &writeSet, NULL, &timeout);
        if (result <= 0)
        {
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }
    }
    int bytesSent = send(clientSocket, msg, strlen(msg), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    closesocket(clientSocket);
    WSACleanup();
    return true;
}
char *Listen(int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return nullptr;
    }
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return nullptr;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    if (listen(listenSocket, 1) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    char *buffer = new char[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        delete[] buffer;
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    buffer[bytesReceived] = '\0';
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return buffer;
}
char *ListenNoWait(int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return nullptr;
    }
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return nullptr;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    if (listen(listenSocket, 1) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(listenSocket, &readSet);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    int selectResult = select(0, &readSet, NULL, NULL, &timeout);
    if (selectResult == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    else if (selectResult == 0)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    char *buffer = new char[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        delete[] buffer;
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return nullptr;
    }
    buffer[bytesReceived] = '\0';
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return buffer;
}
} // namespace net