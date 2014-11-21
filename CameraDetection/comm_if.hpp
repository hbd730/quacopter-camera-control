#pragma once
#include <functional>

enum class error;

class command_if
{
public:
    command_if() noexcept = default;
    command_if(const command_if&) noexcept = delete;
    command_if& operator = (const command_if&) = delete;
    command_if(command_if&&) noexcept = delete;
    command_if& operator = (command_if&&) = delete;
    virtual ~command_if() noexcept = default;

    virtual error set_target(double roll, double yaw, double pitch) noexcept = 0;
};
