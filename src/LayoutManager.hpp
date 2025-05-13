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
    void removeSurface(Surface *surface, const bool &focusPrevious = true) noexcept;
    inline Workspace* workspace() { return m_output->workspace(); }
    void updateLayout() noexcept;
    void focusNextWindow() noexcept;
    void focusPrevWindow() noexcept;
    void focusWindow(Surface *surface) noexcept;
    void removeFocusFromWindow(Surface *surface) noexcept;
    void focusOld() noexcept;
    void setFocusIndex(const int &index) noexcept;
    inline int focusIndex() const noexcept { return m_focus_index; }
    void setAvailableGeometry(const LRect &rect) noexcept;
    void increaseMasterWidth(const float &dw = 1) noexcept;
    void decreaseMasterWidth(const float &dw = 1) noexcept;
    void moveWindowDown() noexcept;
    void moveWindowUp() noexcept;
    void swapMaster() noexcept;
    Surface* focusedWindow() const noexcept;
    void moveWindowNextMonitor() noexcept;
    void moveWindowPrevMonitor() noexcept;
    void moveWindowToMonitor(Surface *surface, Output *output) noexcept;
    inline std::vector<Surface*> surfaces() const noexcept { return m_surfaces; }

private:
    std::vector<Surface *> m_surfaces;
    int m_focus_index = 0;
    int m_gap = 10;
    float m_masterRatio = 0.6f;
    Output *m_output;
    bool m_cycle_window_focus = true;
    // Layout m_layout; // TODO: Holds the layout of the output
};

#endif // LAYOUTMANAGER_HPP
