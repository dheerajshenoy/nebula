#include "Global.h"
#include "../Compositor.h"
#include "../Surface.h"

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

std::list<Surface*>& G::surfaces() noexcept
{
    return (std::list<Surface*>&)compositor()->surfaces();
}

Systemd *G::systemd() noexcept
{
    return compositor()->systemd.get();
}

G::Cursors &G::cursors() noexcept
{
    return xCursors;
}

Output *G::mostIntersectedOuput(LView *view) noexcept
{
    LBox box = view->boundingBox();
    LRect rect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1);
    Output *bestOutput = nullptr;
    Int32 bestArea = 0;
    LBox extents;

    for (Output *o : outputs())
        {
            LRegion reg;
            reg.addRect(rect);
            reg.clip(o->rect());
            extents = reg.extents();
            Int32 area = (extents.x2 - extents.x1) * (extents.y2 - extents.y1);

            if (area > bestArea)
                {
                    bestArea = area;
                    bestOutput = o;
                }
        }

    return bestOutput;
}

const std::vector<Output*>& G::outputs() noexcept {
    return (const std::vector<Output*>&)compositor()->outputs();
}

void G::enableClippingChildren(LView *parent, bool enabled) noexcept
{
    for (LView *child : parent->children())
        {
            child->enableClipping(enabled);
            enableClippingChildren(child, enabled);
        }
}
