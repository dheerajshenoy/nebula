#include "Output.h"
#include "LayoutManager.hpp"

#include <LScreenshotRequest.h>
#include <LSessionLockRole.h>
#include <LAnimation.h>
#include "Compositor.h"
#include "Surface.h"
#include "utils/Global.h"
#include "Workspace.hpp"



Output::Output(const void *params) : LOutput(params),
    m_workspace_index(0) {
    zoomScene.enableParentOffset(false);
    zoomView.enableDstSize(true);
    zoomView.setTranslucentRegion(&LRegion::EmptyRegion());

    workspacesContainer.enableParentOffset(false);
    workspacesContainer.setPos(0, 0);
    workspacesContainer.setParent(&G::compositor()->workspacesLayer);
}

Output::~Output() {
    for (const auto &w : m_workspaces) {
        delete w;
    }
    m_workspaces.clear();
    LOutput::~LOutput();
}

void Output::initializeGL()
{
    G::scene().handleInitializeGL(this);

    m_workspaces.reserve(9);

    for (size_t i=0; i < 9; i++) {
        auto workspace = new Workspace(this, i);
        m_workspaces.push_back(workspace);
    }

    LWeak<Output> weakRef { this };
    fadeInView.insertAfter(&G::layers()[LLayerOverlay]);
    fadeInView.setOpacity(0.f);

    LAnimation::oneShot(1000, // Ms
    [weakRef](LAnimation *anim) // On Update
    {
        // Oops the output was probably unplugged!
        if (!weakRef)
        {
            anim->stop();
            return;
        }

        weakRef->fadeInView.setPos(weakRef->pos());
        weakRef->fadeInView.setSize(weakRef->size());
        weakRef->fadeInView.setOpacity(1.f - powf(anim->value(), 5.f));
        weakRef->repaint();
    },
    [weakRef](LAnimation *) // On Finish
    {
        // Remove it from the scene
        if (weakRef)
            weakRef->fadeInView.setParent(nullptr);
    });
}

void Output::paintGL()
{
    Surface *fullscreenSurface { searchFullscreenSurface() };

    if (fullscreenSurface)
    {
        /*
         * Content Type
         *
         * This hint can be passed to outputs to optimize content display.
         * For example, if the content type is "Game", a TV plugged in via HDMI may try to reduce latency.
         */
        setContentType(fullscreenSurface->contentType());

        /*
         * V-SYNC
         *
         * Clients can suggest enabling or disabling v-sync for specific surfaces using LSurface::preferVSync().
         * For instance, you may choose to enable or disable it when displaying a toplevel surface in fullscreen mode.
         * Refer to LOutput::enableVSync() for more information.
         */
        enableVSync(fullscreenSurface->preferVSync());

        /*
         * Oversampling:
         *
         * When you assign a fractional scale to an output, oversampling is enabled by default to mitigate aliasing artifacts,
         * but this may impact performance.
         * For instance, you might want to switch off oversampling when displaying a fullscreen surface. Refer to enableFractionalOversampling().
         *
         * Note: Oversampling is unnecessary and always disabled when using integer scales. Therefore, it's recommended to stick with integer scales
         * and select an appropriate LOutputMode that suits your requirements.
         */
        enableFractionalOversampling(false);

        /*
         * Direct Scanout
         *
         * Directly scanning fullscreen surfaces reduces GPU consumption and latency. However, there are several conditions
         * to consider before doing so, such as ensuring there is no overlay content like subsurfaces, popups,
         * notifications (as these won't be displayed), or screenshot requests which are always denied.
         * Refer to LOutput::setCustomScanoutBuffer() for more information.
         */
        if (tryDirectScanout(fullscreenSurface))
            return; // On success, avoid doing any rendering.
    }
    else
    {
        setContentType(LContentTypeNone);
        enableFractionalOversampling(true);
    }

    const bool zoomed { zoom < 1.f && cursor()->output() == this };

    if (zoomed)
        zoomedDrawBegin();
    else
        cursor()->enable(this, true);

    /* Let our scene do its magic */
    G::scene().handlePaintGL(this);

    if (zoomed)
        zoomedDrawEnd();

    /* Screen capture requests for this single frame */
    for (LScreenshotRequest *req : screenshotRequests())
        req->accept(true);
}

void Output::moveGL()
{
    G::scene().handleMoveGL(this);
}

void Output::resizeGL()
{
    G::scene().handleResizeGL(this);
}

void Output::uninitializeGL()
{
    G::scene().handleUninitializeGL(this);
}

void Output::setGammaRequest(LClient *client, const LGammaTable *gamma)
{
    L_UNUSED(client)
    setGamma(gamma);
}

void Output::availableGeometryChanged()
{
    /* Refer to the default implementation in the documentation. */
    const LRect availGeo { pos() + availableGeometry().pos(), availableGeometry().size() };
    layoutManager()->setAvailableGeometry(availGeo);
}

Surface *Output::searchFullscreenSurface() const noexcept
{
    /*
     * When the session is locked the locking client creates a fullscreen
     * surface for each initialized output
     */
    if (sessionLockRole() && sessionLockRole()->surface()->mapped())
        return static_cast<Surface*>(sessionLockRole()->surface());

    /*
     * Louvre moves fullscreen toplevels to the top layer.
     * The default implementation of LToplevelRole (which is partially used by our ToplevelRole)
     * stores the output in its exclusiveOutput() property
     */
    for (auto it = compositor()->layer(LLayerTop).rbegin();it != compositor()->layer(LLayerTop).rend(); it++)
        if ((*it)->mapped() && (*it)->toplevel() && (*it)->toplevel()->fullscreen() && (*it)->toplevel()->exclusiveOutput() == this)
            return static_cast<Surface*>(*it);

    // No fullscreen surfaces on this output
    return nullptr;
}

bool Output::tryDirectScanout(Surface *surface) noexcept
{
    if (!surface || !surface->mapped() ||

        /* Ensure there is no overlay content.
         * TODO: Check if there are overlay layer-shell surfaces
         * such as notifications */
        surface->hasMappedChildSurface() ||

        /* Screenshot requests are denied when there is a custom scanout buffer */
        !screenshotRequests().empty() ||

        /* Hardware cursor planes can still be displayed on top */
        (cursor()->visible() && !cursor()->hwCompositingEnabled(this)))
        return false;

    const bool ret { setCustomScanoutBuffer(surface->texture()) };

    /* Ask the surface to continue updating */
    if (ret)
        surface->requestNextFrame();

    return ret;
}

LayoutManager* Output::layoutManager() noexcept {
    return m_workspaces.at(m_workspace_index)->layoutManager();
}

void Output::setWorkspace(const size_t &n) noexcept {
    if (n >= m_workspaces.size() || m_workspace_index == n)
        return;

    m_workspaces.at(m_workspace_index)->show(false);
    this->repaint();
    // enableVSync(true);
    m_workspaces.at(n)->show(true);
    m_workspace_index = n;

    m_workspaces.at(n)->stealChildren();
}

void Output::zoomedDrawBegin() noexcept
{
    /* Set the zone to capture */
    G::scene().enableAutoRepaint(false);
    zoomScene.setParent(G::scene().mainView());
    zoomScene.setVisible(true);
    zoomScene.setSizeB(sizeB() * zoom);
    zoomScene.setScale(scale());

    const LPointF outputRelativeCursorPos { cursor()->pos() - LPointF(pos()) };
    const LPointF outputNormalizedCursorPos { outputRelativeCursorPos / LSizeF(size()) };
    LPoint newPos { cursor()->pos() - (LSizeF(zoomScene.nativeSize()) * outputNormalizedCursorPos) };

    /* Prevent capturing stuff outside the output */

    if (newPos.x() < pos().x())
        newPos.setX(pos().x());
    else if (newPos.x() + zoomScene.nativeSize().w() > pos().x() + size().w())
        newPos.setX(pos().x() + size().w() - zoomScene.nativeSize().w());

    if (newPos.y() < pos().y())
        newPos.setY(pos().y());
    else if (newPos.y() + zoomScene.nativeSize().h() > pos().y() + size().h())
        newPos.setY(pos().y() + size().h() - zoomScene.nativeSize().h());

    zoomScene.setPos(newPos);

    /* Render views and the cursor manually into the zoom scene */
    G::compositor()->rootView.setParent(&zoomScene);
    zoomScene.render();

    /* Use an LTextureView to show the result scaled to the output size */
    zoomView.setDstSize(size());
    zoomView.setPos(pos());
    zoomView.setTexture(zoomScene.texture());
    zoomView.setParent(G::scene().mainView());
    zoomScene.setVisible(false);
}

void Output::zoomedDrawEnd() noexcept
{
    /* Return everything to its place */
    G::compositor()->rootView.setParent(G::scene().mainView());
    zoomView.setParent(nullptr);
    zoomScene.setParent(nullptr);
    G::scene().enableAutoRepaint(true);
}

void Output::setZoom(Float32 newZoom) noexcept
{
    const Float32 prevZoom { zoom };
    zoom = newZoom;

    if (zoom > 1.f)
        zoom = 1.f;
    else if (newZoom < 0.25f)
        zoom = 0.25f;

    if (prevZoom != zoom)
        repaint();
}
