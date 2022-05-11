#ifndef __UDPSOCKET_H__
#define __UDPSOCKET_H__
/**
 * A simple UDP socket that can send and receive messages
 *
 * @author Aldric Vitali Silvestre
 */


#include <loguru/loguru.hpp>

#include <system_error>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

class UDPSocket
{
public:
    UDPSocket();
    ~UDPSocket();

    void bindPort(uint16_t port);
    void sendMessage(const std::string& ipAddress, uint16_t port, const char* buffer, int length, int flags = 0);
    int receiveMessage(char* buffer, int length, sockaddr_in& sender, int flags = 0);

    // ==== GETTERS ====
    uint16_t getPort();
    int getSocket();

private:
    uint16_t m_port;
    int m_socket;
};

#endif // __UDPSOCKET_H__