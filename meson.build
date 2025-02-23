project(
    'Nebula',
    'cpp',
    version : '0.1.0',
    meson_version: '>= 0.60.0',
    default_options: [
        'warning_level=2',
        'cpp_std=c++20',
    ]
)

louvre_dep = dependency('Louvre', version : '>=2.4.0')
pixman_dep = dependency('pixman-1')
libinput_dep = dependency('libinput')
sdbus = dependency(['libsystemd', 'libelogind', 'basu'])
glesv2_dep = dependency('glesv2')
wayland_server_dep = dependency('wayland-server')
fontconfig_dep = dependency('fontconfig')
# freetype_dep = dependency('freetype2')
icuuc_dep = dependency('icu-uc')

# List C/C++ source files stored in ./src
sources = files(
    'src/LayoutManager.cpp',
    'src/Client.cpp',
    'src/Compositor.cpp',
    'src/main.cpp',
    'src/Nebula.cpp',
    'src/Nebula.hpp',
    'src/Output.cpp',
    'src/SessionLockManager.cpp',
    'src/Surface.cpp',
    'src/util.hpp',
  'src/Workspace.cpp',

    # Input
    'src/input/Clipboard.cpp',
    'src/input/DND.cpp',
    'src/input/Keyboard.cpp',
    'src/input/Pointer.cpp',
    'src/input/Seat.cpp',
    'src/input/Touch.cpp',

    # Roles
    'src/roles/CursorRole.cpp',
    'src/roles/DNDIconRole.cpp',
    'src/roles/LayerRole.cpp',
    'src/roles/PopupRole.cpp',
    'src/roles/SessionLockRole.cpp',
    'src/roles/SubsurfaceRole.cpp',
    'src/roles/ToplevelRole.cpp',

    # Scene
    'src/scene/LayerView.cpp',
    'src/scene/Scene.cpp',
    'src/scene/SSD.cpp',
    'src/scene/SSDTouchable.cpp',
    'src/scene/SurfaceView.cpp',

    # Utils
    'src/utils/Assets.cpp',
    'src/utils/Global.cpp',
    'src/utils/Settings.cpp',
    'src/utils/Systemd.cpp',
)

executable(
    'nebula',
    sources : [sources],
    dependencies : [
        louvre_dep,
        pixman_dep,
        libinput_dep,
        sdbus,
        wayland_server_dep,
        # glesv2_dep,
        # fontconfig_dep,
        # freetype_dep,
        # icuuc_dep
    ],
    install : true)
