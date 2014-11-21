#include "command.hpp"
#include "errors.hpp"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

command_udp_socket::command_udp_socket(int port)
{
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = inet_addr("127.0.0.1");
    address_.sin_port = htons(port);

    sock_ = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
}

command_udp_socket::~command_udp_socket()
{
    if (sock_ != -1)
    {
        close(sock_);
    }
}

error command_udp_socket::set_target(double roll, double yaw, double pitch) noexcept
{
    char buffer[27] = {0};
    sprintf(buffer, "%0+8.3f,%0+8.3f,%0+8.3f", roll, yaw, pitch);
    int sent = sendto(sock_, buffer, sizeof(buffer), 0,
                      (const struct sockaddr*) &address_,
                      sizeof(address_));
    if (sent == -1)
    {
        // failure
        switch (errno)
        {
        case EAGAIN:
            // buffer completely full
            return error::buffer_full;
        default:
            return error::unknown;
        }
    }
    else if (sent != sizeof(buffer))
    {
        return error::buffer_full;
    }
    else
    {
        //success
    }
    return error::success;
}
