#pragma once

#include <LLauncher.h>
#include <LLog.h>
#include <unistd.h>

class Nebula {

public:
    Nebula();
    ~Nebula();

void init_env() noexcept;
int event_loop() noexcept;
void focusNextMonitor() noexcept;
void focusPrevMonitor() noexcept;

int monitorIndex() const noexcept { return m_monitorIndex; }

private:
    int m_monitorIndex;
};
