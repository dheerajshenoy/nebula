#ifndef LAYOUTMANAGER_HPP
#define LAYOUTMANAGER_HPP

#include <LLayerView.h>
#include <vector>
#include "Surface.h"
#include "scene/LayerView.h"

class Output;

using namespace Louvre;

class LayoutManager {
public:

LayoutManager(Output *output);
void addSurface(Surface *surface) noexcept;
void removeSurface(Surface *surface) noexcept;
void updateLayout() noexcept;
void focusNextWindow() noexcept;
void focusPrevWindow() noexcept;
void focusWindow(Surface *surface) noexcept;
void setFocusIndex(const int &index) noexcept;
inline int focusIndex() const noexcept { return m_focus_index; }

void setAvailableGeometry(const LRect &rect) noexcept;

void increaseMasterWidth(const float &dw = 1) noexcept;
void decreaseMasterWidth(const float &dw = 1) noexcept;

private:
    std::vector<Surface *> m_surfaces;
    int m_focus_index = 0;
    int m_gap = 0;
    LRect m_availGeo;
    float m_masterRatio = 0.6f;
};

#endif // LAYOUTMANAGER_HPP
