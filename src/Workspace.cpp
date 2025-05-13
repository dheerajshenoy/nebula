#include "Workspace.hpp"
#include "Output.h"
#include "utils/Global.h"

static void clipChildrenViews(LView *view, const LRect &rect);


Workspace::Workspace(Output *output,
                     const size_t &index) : LLayerView(this),
    m_output(output),
    m_layoutManager(new LayoutManager(output)),
    m_index(index) {
    setPos(0, 0);
    setSize(output->availableGeometry().size());
    enableClipping(true);
}

Workspace::~Workspace() {
    delete m_layoutManager;
}

void Workspace::show(const bool &state) noexcept {
    if (state == (parent() != nullptr))
        return;

    if (state)
        setParent(&G::compositor()->workspacesLayer);
    else
        setParent(nullptr);
}

static void clipChildrenViews(LView *view, const LRect &rect)
{
    for (LView *child : view->children())
    {
        child->enableClipping(true);
        child->setClippingRect(rect);
        clipChildrenViews(child, rect);
    }
}

void Workspace::clipChildren() noexcept {

    LRect rect;

    rect.setY(0);
    rect.setH(m_output->size().h());

    if (pos().x() + m_output->pos().x() < m_output->pos().x()) {
        rect.setX(m_output->pos().x());
        rect.setW(m_output->size().w() + pos().x());
        if (rect.w() < 0)
            rect.setWidth(0);

    } else if (pos().x() + m_output->pos().x() > m_output->pos().x() + m_output->size().w()) {
        rect.setX(m_output->size().w());
        rect.setW(0);
    } else {
        rect.setX(m_output->pos().x() + pos().x());
        rect.setW(m_output->size().w() - pos().x());
    }

    setSize(m_output->size());

    clipChildrenViews(this, rect);

}

void Workspace::stealChildren() noexcept {
    LView *surfView;

    for (class Surface *surf : G::surfaces())
        {
            surfView = surf->getView();

            if (surfView && surfView->parent() == &G::compositor()->surfacesLayer && !surf->cursorRole())
                {
                    if (G::mostIntersectedOuput(surfView) == m_output)
                        {
                            surfView->setParent(&surfaces);
                            surfView->enableParentOffset(true);
                        }
                }
        }
}

void Workspace::returnChildren() noexcept
{
    G::enableClippingChildren(this, false);
    // m_output->wallpaper.setParent(&G::compositor()->backgroundLayer);
    // m_output->wallpaper.enableParentOffset(false);

    LView *v;

    while (!surfaces.children().empty()) {
        v = surfaces.children().front();
        v->setParent(&G::compositor()->surfacesLayer);
        v->enableParentOffset(false);
    }

    std::list<class Surface*>surfaces = G::surfaces();

    for (class Surface *s : surfaces)
        s->raise();

    // m_output->topbar.setParent(&G::compositor()->overlayLayer);
    // m_output->topbar.enableParentOffset(false);

    m_output->repaint();
}
