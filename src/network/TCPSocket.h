#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

/**
 * A simple TCP socket that can send and receive messages
 *
 * @author Aldric Vitali Silvestre
 */

#include <loguru/loguru.hpp>

#include <system_error>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <vector>
#include <unistd.h>
#include <chrono>

/**
 * A base TCP handler permitting to send and receive messages more easily.
 * 
 * This class is designed to communicate with only one client/server.
 */
class TCPSocket
{
private:
    static inline constexpr ssize_t BUFFER_LENGTH = 4096;

    /**
     * The address used to send and receive messages
     * (the others are informational)
     */
    struct sockaddr_in m_correspondantAddr;

    struct sockaddr_in m_serverAddr;

    /**
     * Unused when on "client" mode
     */
    sockaddr_storage m_clientAddr;

    uint16_t m_port;

    int m_socket;
    /**
     * In "server" mode, this socket is the first created on init.
     * In client mode, this attribute is never used
     */
    int m_refSocket = -1;

public:
    TCPSocket();
    ~TCPSocket();

    /**
     * This is intended to be used when the process is the client
     * Tries to connect to a server waiting for connections.
     * 
     * In "client" mode, a timeout will be set when waiting for reception.
     */
    void connectToServer(const std::string& ipAddress, uint16_t port, int socketTimeoutMs = 0);

    /**
     * This is intended to be used when the process is the server.
     * 
     * Wait for a single client to connect on the specified port.
     * This port will also be defined on the method call.
     */
    void waitConnection(uint16_t port);

    void sendMessage(const void* message, int length, int flags = 0);
    ssize_t receiveMessage(void* message, int length, int flags = 0);
    
    /**
     * Receive a message in multiple parts, until we have the number of bytes wanted.
     * If the timeoutMs is set and above 0, this will check 
     * that the operation does not take too long, and return an empty vector if timeout exceed.
     * 
     * Note : send does not require an equivalent function, as tcp handle the splitting itself.
     */
    template<typename T>
    std::vector<T> receiveBigMessage(ssize_t nbBytesWanted, int timeoutMs = 0, int flags = 0);

    /**
     * Higher level send message method
     */
    void sendMessage(const std::string& message, int flags = 0);
    
    /**
     * Higher level receive message method
     */
    std::string receiveMessage(int flags = 0);

    // ==== GETTERS ====
    uint16_t getPort();
    int getSocket();
    struct sockaddr_in getCorrespondantAddr();
    struct sockaddr_in getServerAddr();
};

// As we use a template, we must define it in the header for for mermitting discovery on compilation
template<typename T>
std::vector<T> TCPSocket::receiveBigMessage(ssize_t nbBytesWanted, int timeoutMs, int flags)
{
    T tmpBuffer[BUFFER_LENGTH];

    std::vector<T> buffer;
    // "reserve" does not change the "size" attribute of the vector 
    buffer.reserve(nbBytesWanted);
    auto start = std::chrono::steady_clock::now();
    while(buffer.size() < nbBytesWanted)
    {
        auto bytesRead = receiveMessage(tmpBuffer, BUFFER_LENGTH);
        buffer.insert(buffer.end(), &tmpBuffer[0], &tmpBuffer[bytesRead]);

        auto checkpoint = std::chrono::steady_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(checkpoint - start).count();
        if (timeoutMs > 0 && elapsedMs > timeoutMs)
        {
            LOG_F(ERROR, "Timeout exeeded !");
            buffer.clear();
            break;
        }
    }
    return buffer;
}

#endif // __TCPSOCKET_H__