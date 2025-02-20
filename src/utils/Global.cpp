#include "Global.h"
#include "../Compositor.h"

static G::Cursors xCursors;

Scene &G::scene() noexcept
{
    return compositor()->scene;
}

LayerView *G::layers() noexcept
{
    return scene().layers;
}

Assets *G::assets() noexcept
{
    return compositor()->assets.get();
}

Systemd *G::systemd() noexcept
{
    return compositor()->systemd.get();
}

G::Cursors &G::cursors() noexcept
{
    return xCursors;
}
