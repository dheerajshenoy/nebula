#ifndef GLOBAL_H
#define GLOBAL_H

#include <LNamespaces.h>

using namespace Louvre;

class Compositor;
class Output;
class Scene;
class LayerView;
class Assets;
class Systemd;
class Seat;

/* This is just a utility for quick access to the Compositor members and
 * to prevent type casting all the time */
class G {
public:

enum ViewHint
{
    SSDEdge = 1
};

static Compositor *compositor() noexcept
{
    return reinterpret_cast<Compositor*>(Louvre::compositor());
}

static Seat *seat() noexcept
{
    return reinterpret_cast<Seat*>(Louvre::seat());
}

static Scene &scene() noexcept;
static LayerView *layers() noexcept;
static Assets *assets() noexcept;
static Systemd *systemd() noexcept;

struct Cursors
{
    LXCursor *arrow = nullptr;
    LXCursor *hand2 = nullptr;
    LXCursor *top_left_corner = nullptr;
    LXCursor *top_right_corner = nullptr;
    LXCursor *bottom_left_corner = nullptr;
    LXCursor *bottom_right_corner = nullptr;
    LXCursor *left_side = nullptr;
    LXCursor *top_side = nullptr;
    LXCursor *right_side = nullptr;
    LXCursor *bottom_side = nullptr;
    LXCursor *move = nullptr;
};

static Cursors &cursors() noexcept;

};

#endif // GLOBAL_H
