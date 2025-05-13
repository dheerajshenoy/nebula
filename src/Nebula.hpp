#pragma once

#include <LLauncher.h>
#include <LLog.h>
#include <unistd.h>
#include "sol/sol.hpp"
#include "ipc.hpp"

class Compositor;

class Nebula {

public:
    Nebula();
    ~Nebula();

void init_env() noexcept;
int event_loop() noexcept;
void focusNextMonitor() noexcept;
void focusPrevMonitor() noexcept;
void init_configuration() noexcept;
void init_ipc() noexcept;

int monitorIndex() const noexcept { return m_monitorIndex; }

private:
    Compositor *m_compositor { nullptr };
    sol::state m_lua;
    int m_monitorIndex;
};
