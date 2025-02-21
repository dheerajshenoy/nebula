#ifndef LAYOUTMANAGER_HPP
#define LAYOUTMANAGER_HPP

#include <LLayerView.h>
#include <vector>
#include "Surface.h"
#include "scene/LayerView.h"

using namespace Louvre;

class LayoutManager {
public:

LayoutManager(const LSize &size);
void addSurface(Surface *surface) noexcept;
void removeSurface(Surface *surface) noexcept;
void updateLayout() noexcept;
LayerView* layerView() { return m_layerView; }

void focusNextWindow() noexcept;
void focusPrevWindow() noexcept;
void focusWindow(Surface *surface) noexcept;
void setFocusIndex(const int &index) noexcept;
inline int focusIndex() const noexcept { return m_focus_index; }

private:
    LayerView *m_layerView = new LayerView();
    std::vector<Surface *> m_surfaces;
    int m_focus_index = 0;
    int m_gap = 0;
};

#endif // LAYOUTMANAGER_HPP
