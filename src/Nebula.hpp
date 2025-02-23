#pragma once

#include <LLauncher.h>
#include <LLog.h>
#include <unistd.h>
#include "sol/sol.hpp"

class Nebula {

public:
    Nebula();
    ~Nebula();

void init_env() noexcept;
int event_loop() noexcept;
void focusNextMonitor() noexcept;
void focusPrevMonitor() noexcept;
void init_configuration() noexcept;

int monitorIndex() const noexcept { return m_monitorIndex; }

private:
    sol::state m_lua;
    int m_monitorIndex;
};
