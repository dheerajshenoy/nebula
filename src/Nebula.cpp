#include "Nebula.hpp"
#include "Compositor.h"

Nebula::Nebula() {
    init_env();
    LLauncher::startDaemon();
    init_configuration();
    init_ipc();
}

Nebula::~Nebula() {
}

void Nebula::init_env() noexcept {
    /* Enable Louvre fatal logs */
    setenv("LOUVRE_DEBUG", "1", 0);

    /* Enable SRM fatal logs */
    setenv("SRM_DEBUG", "1", 0);

    /* If not set, the iGPU will be used as the primary GPU. */
    // setenv("SRM_ALLOCATOR_DEVICE", "/dev/dri/card1", 0);

    /* Enable triple buffering when using the DRM backend (provides a smoother experience but consumes more CPU) */
    // setenv("SRM_RENDER_MODE_ITSELF_FB_COUNT", "3", 0);

    /* Same but for non-primary GPUs */
    // setenv("SRM_RENDER_MODE_PRIME_FB_COUNT", "3", 0);
    // setenv("SRM_RENDER_MODE_DUMB_FB_COUNT", "3", 0);
    // setenv("SRM_RENDER_MODE_CPU_FB_COUNT", "3", 0);

    /* Force OpenGL buffer allocation instead of GBM when using the DRM backend */
    // setenv("SRM_FORCE_GL_ALLOCATION", "1", 0);

    /* The legacy DRM API supported async hw cursor updates */
    // setenv("SRM_FORCE_LEGACY_API", "1", 0);

    /* Enable Wayland for Firefox */
    setenv("MOZ_ENABLE_WAYLAND", "1", 1);

    /* Enable Wayland for Qt apps */
    setenv("QT_QPA_PLATFORM", "wayland-egl", 1);

    /* Enable Wayland for GTK apps */
    setenv("GDK_BACKEND", "wayland", 1);

    /* The Wayland socket that apps started with LLauncher will listen to */
    setenv("LOUVRE_WAYLAND_DISPLAY", "wayland-2", 0);

}

int Nebula::event_loop() noexcept {
    m_compositor = new Compositor();

    if (!m_compositor->start()) {
        LLog::fatal("Failed to start compositor.");
        return EXIT_FAILURE;
    }

    /* Main thread loop, use LCompositor::fd() to get a pollable fd if needed. */
    while (m_compositor->state() != LCompositor::Uninitialized)
        m_compositor->processLoop(-1);

    return EXIT_SUCCESS;
}

void Nebula::init_configuration() noexcept {
    m_lua.open_libraries(sol::lib::base, sol::lib::package);

    try {
        m_lua.script_file("/home/neo/Gits/nebula/src/config.lua");
    } catch (sol::error &e) {
        LLog::log("%s", e.what());
        std::exit(1);
    }

    auto modkey = m_lua["modkey"].get_or<std::string>("mod4");

    auto keybind = m_lua["keybind"].get<sol::table>();

    for (const auto &pair : keybind) {
        std::string key = pair.first.as<std::string>();
        sol::table value = pair.second.as<sol::table>();
        std::cout << key;
    }
}


void Nebula::init_ipc() noexcept {
    IPCServer server;
    if (!server.start()) {
        std::cerr << "Could not initialize ipc!\n";
        return;
    }
    server.handleClients();
}
