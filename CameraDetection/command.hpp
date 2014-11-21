#pragma once
#include "comm_if.hpp"
#include <netinet/in.h>

class command_udp_socket: public command_if
{
public:
    command_udp_socket(int port);
    ~command_udp_socket();
    error set_target(double roll, double yaw, double pitch) noexcept override;

private:
    int sock_;
    struct sockaddr_in address_;
};
