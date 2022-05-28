#include "TCPSocket.h"

#include <iostream>


using namespace std;

TCPSocket::TCPSocket()
{}

TCPSocket::~TCPSocket()
{
    close(m_socket);
    if (m_refSocket != -1)
    {
        close(m_refSocket);
    }
}

void TCPSocket::connectToServer(const std::string& ipAddress, uint16_t port, int socketTimeoutMs)
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == -1)
    {
        throw system_error(errno, system_category(), "Error while creating socket");
    }
    LOG_F(INFO, "Socket created");
    m_port = port;

    if (socketTimeoutMs > 0)
    {
        LOG_F(INFO, "Set timeout on socket");
        time_t socketTimeoutS = socketTimeoutMs / 1000;
        time_t socketTimeoutUs = (socketTimeoutMs % 1000) * 1000;
        struct timeval tv;
        tv.tv_sec = socketTimeoutS;
        tv.tv_usec = socketTimeoutUs;
        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    }

    // Create address connexion
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    m_serverAddr.sin_port = htons(port);

    LOG_F(INFO, "Try to connect to %s", ipAddress.c_str());
    int connectResult = connect(m_socket, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
    if (connectResult != 0)
    {
        throw system_error(errno, system_category(), "Error while connecting to server");
    }
    LOG_F(INFO, "Connected to the server");

    m_correspondantAddr.sin_family = AF_INET;
    m_correspondantAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    m_correspondantAddr.sin_port = htons(port);
}

void TCPSocket::waitConnection(uint16_t port)
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == -1)
    {
        throw system_error(errno, system_category(), "Error while creating socket");
    }
    LOG_F(INFO, "Socket created");
    m_port = port;

    // Create address connexion
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(port);

    LOG_F(INFO, "Bind port %d", port);
    int bindResult = bind(m_socket, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
    if (bindResult != 0)
    {
        throw system_error(errno, system_category(), "Error while biding port");
    }

    // This is here where we allow only one client at the time
    LOG_F(INFO, "Start listening on socket");
    int listenResult = listen(m_socket, 1);
    if (listenResult == -1)
    {
        throw system_error(errno, system_category(), "Error while listening on socket");
    }

    socklen_t clientAddrSize = sizeof(m_clientAddr);
    int newSocket = accept(m_socket, (struct sockaddr*)&m_clientAddr, &clientAddrSize);
    if (newSocket == -1)
    {
        throw system_error(errno, system_category(), "Error while accepting incomming connection");
    }
    // Now the socket where to send is the new socket
    m_refSocket = m_socket;
    m_socket = newSocket;

    auto cAddr = ((struct sockaddr_in*)&m_clientAddr);
    auto clientIpAddress = inet_ntoa(cAddr->sin_addr);
    auto clientPort = ntohs(cAddr->sin_port);
    LOG_F(INFO, "Client connected (incomming ip address : %s & port %d)", clientIpAddress, clientPort);

    m_correspondantAddr.sin_family = AF_INET;
    m_correspondantAddr.sin_addr.s_addr = cAddr->sin_addr.s_addr;
    m_correspondantAddr.sin_port = cAddr->sin_port;
}

void TCPSocket::sendMessage(const void* msg, int msgLength, int flags)
{
    if (sendto(m_socket, msg, msgLength, flags, (struct sockaddr*)&m_correspondantAddr, sizeof(m_correspondantAddr)) < 0)
    {
        throw system_error(errno, system_category(), "Cannot send message");
    }
    // LOG_F(INFO, "Message sent to %s", ipAddress.c_str());
}

ssize_t TCPSocket::receiveMessage(void* buffer, int bufferLength, int flags)
{
    size_t bytesRead = recvfrom(m_socket, buffer, bufferLength, flags, NULL, NULL);
    if (bytesRead < 0)
    {
        throw system_error(errno, system_category(), "Cannot receive message");
    }
    // LOG_F(INFO, "Message received from %s", inet_ntoa(sender.sin_addr));
    return bytesRead;
}

void sendBigMessage(void* buffer, ssize_t messageLength, int flags = 0);

void TCPSocket::sendMessage(const string& message, int flags)
{
    sendMessage(message.data(), message.size(), flags);
}

string TCPSocket::receiveMessage(int flags)
{
    char buffer[BUFFER_LENGTH];
    ssize_t bytesRead = receiveMessage(buffer, BUFFER_LENGTH, flags);
    string message(buffer, bytesRead);
    return message;
}

uint16_t TCPSocket::getPort()
{
    return m_port;
}

int TCPSocket::getSocket()
{
    return m_socket;
}

struct sockaddr_in TCPSocket::getCorrespondantAddr()
{
    return m_correspondantAddr;
}

struct sockaddr_in TCPSocket::getServerAddr()
{
    return m_serverAddr;
}
