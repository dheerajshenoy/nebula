#include "Surface.h"
#include "utils/Global.h"
#include "scene/LayerView.h"
#include "roles/ToplevelRole.h"

LView *Surface::getView() noexcept
{
    if (tl() && tl()->ssd)
        return tl()->ssd.get();

    return &view;
}

ToplevelRole *Surface::closestToplevelParent() const noexcept
{
    const LSurface *surface { this };

    while (surface->parent())
        {
            if (surface->parent()->toplevel())
                return static_cast<ToplevelRole*>(surface->parent()->toplevel());
            surface = surface->parent();
        }

    return nullptr;
}

bool Surface::hasMappedChildSurface() const noexcept
{
    for (LSurface *child : children())
        if (child->mapped())
            return true;

    return false;
}

void Surface::roleChanged()
{
    /* When a client assigns the LCursorRole to a surface, we use LCursor to render it
     * instead of the scene, so we hide the view here.
     * See Pointer::setCursorRequest() and the documentation for LClientCursor and LCursor. */
    if (cursorRole())
        view.setVisible(false);
}

/* Louvre already manages the Z-axis ordering of all surfaces for us, ensuring that the order specified
 * in each role protocol is respected. See LCompositor::surfaces() and LCompositor::layer().
 * Since we are using the scene system, we need to update the order of our views manually.
 * This can be accomplished as shown in both event listeners below.
 * Note: You're of course free to not follow this order, but you probably should. */

void Surface::layerChanged()
{
    getView()->setParent(&G::layers()[layer()]);
}

void Surface::orderChanged()
{
    /* Previous surface in LCompositor::surfaces() */
    Surface *prev { static_cast<Surface*>(prevSurface()) };

    /* Re-insert the view only if there is a previous surface within the same layer */
    getView()->insertAfter((prev && prev->layer() == layer()) ? prev->getView() : nullptr);
}

/*
  void Surface::mappingChanged()
  {
  /* Refer to the default implementation in the documentation.
  // LSurface::mappingChanged();
  compositor()->repaintAllOutputs();

    Output* activeOutput { (Output*) cursor()->output() };

    if (!activeOutput)
    return;

    if (mapped() && toplevel()) {
    setPos(10, 10);
    }

}
 */

void Surface::mappingChanged()
{
    compositor()->repaintAllOutputs();

    Output *activeOutput { (Output*)cursor()->output() };
    if (!activeOutput)
        return;

    if (mapped() && toplevel())
    {
        static std::vector<Surface *> tiledWindows;

        // Add the window if it's not already in the list
        if (std::find(tiledWindows.begin(), tiledWindows.end(), this) == tiledWindows.end())
        {
            tiledWindows.push_back(this);
        }

        // Get available screen space
        const LSize availSize = activeOutput->availableGeometry().size();
        const LSize availPos = activeOutput->pos() + activeOutput->availableGeometry().pos();

        int numWindows = tiledWindows.size();
        if (numWindows == 0) return;

        if (numWindows == 1) {
            Surface *win = tiledWindows[0];
            auto tl = win->tl();
            tl->configureSize(availSize.width(), availSize.height());
            win->setPos(availPos.x(), availPos.y());
            return;
        }

        int masterWidth = availSize.width() * 0.6; // Master gets 60% width
        int stackWidth = availSize.width() - masterWidth;


        // Set master window position (first window gets master area)
        if (numWindows >= 1)
        {
            Surface *master = tiledWindows[0];
            auto tl = master->tl();
            tl->configureSize(masterWidth, availSize.height());
            master->setPos(availPos.x(), availPos.y());
        }

        // Stack remaining windows vertically
        if (numWindows > 1)
        {
            int stackHeight = availSize.height() / (numWindows - 1);
            for (int i = 1; i < numWindows; i++)
            {
                Surface *win = tiledWindows[i];
                int y = availPos.y() + (i - 1) * stackHeight;
                auto tl = win->tl();
                tl->configureSize(stackWidth, stackHeight);
                win->setPos({availPos.x() + masterWidth, y});
            }
        }
    }
}


void Surface::minimizedChanged()
{
    if (minimized())
        {
            if (tl())
                tl()->configureState(tl()->pendingConfiguration().state & ~LToplevelRole::Activated);

            view.setVisible(false);

            if (getView() != &view)
                getView()->setVisible(false);
        }
    else
        {
            if (tl())
                tl()->configureState(tl()->pendingConfiguration().state | LToplevelRole::Activated);

            view.setVisible(true);

            if (getView() != &view)
                getView()->setVisible(true);

            raise();
        }
}
