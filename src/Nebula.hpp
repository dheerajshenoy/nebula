#pragma once

#include <LLauncher.h>
#include <LLog.h>
#include <unistd.h>

#include "Compositor.h"

class Nebula {

public:
    Nebula();
    ~Nebula();

void init_env() noexcept;
int event_loop() noexcept;


private:


};
