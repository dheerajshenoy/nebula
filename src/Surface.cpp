#include "Surface.h"
#include "utils/Global.h"
#include "scene/LayerView.h"
#include "roles/ToplevelRole.h"
#include "LayoutManager.hpp"
#include <iostream>

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

void Surface::mappingChanged()
{
    compositor()->repaintAllOutputs();

    Output *activeOutput { (Output*) cursor()->output() };
    if (!activeOutput)
        return;

    LayoutManager* layoutManager = activeOutput->layoutManager();

    if (toplevel()) {
        if (mapped()) {
            layoutManager->addSurface(this);
        }
        else {
            layoutManager->removeSurface(this);
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

void Surface::setFloating(const bool &state) noexcept {
    m_floating = state;
    Output *activeOutput { (Output*)cursor()->output() };
    auto lm = activeOutput->layoutManager();

    lm->updateLayout();
    if (state) {
        raise();
    }
}

void Surface::toggleFloating() noexcept {
    setFloating(!this->isFloating());
}

void Surface::setFullscreen(const bool &state) noexcept {
    m_fullscreen = state;
    Output *activeOutput { (Output*) cursor()->output() };

    auto tl = this->tl();
    if (state) {
        tl->setFullscreenRequest(activeOutput);
    } else {
        tl->unsetFullscreenRequest();
        activeOutput->layoutManager()->updateLayout();
    }

}

void Surface::toggleFullscreen() noexcept {
    setFullscreen(!this->isFullscreen());
}
