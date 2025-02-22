#include "LayoutManager.hpp"
#include "roles/ToplevelRole.h"
#include "Output.h"
#include "util.hpp"

LayoutManager::LayoutManager(Output *output) : m_output(output) {
    m_availGeo.setSize(output->availableGeometry().size());
}

void LayoutManager::addSurface(Surface *surface) noexcept {
    if (!surface)
        return;

    if (m_surfaces.empty()) {
        m_focus_index = 0; // First window, set focus
    } else {
        m_focus_index = m_surfaces.size();
    }

    m_surfaces.push_back(surface);

    focusWindow(surface);

    updateLayout();
}

void LayoutManager::removeSurface(Surface *surface) noexcept {
    if (!surface)
        return;

    auto it = std::find(m_surfaces.begin(), m_surfaces.end(), surface);
    if (it != m_surfaces.end()) {
        bool wasFocused = (*it == m_surfaces[m_focus_index]);
        m_surfaces.erase(it);

        if (wasFocused) {
            if (!m_surfaces.empty()) {
                m_focus_index = m_surfaces.size() - 1;
                focusWindow(m_surfaces[m_focus_index]);
            } else {
                m_focus_index = -1;
            }
        } else if (m_focus_index > std::distance(m_surfaces.begin(), it)) {
            m_focus_index--;
        }

        updateLayout();
    }
}

void LayoutManager::updateLayout() noexcept {
    if (m_surfaces.empty()) return;

    LSize containerSize = m_availGeo.size();
    LSize containerPos = m_output->pos();

    // TODO: Filter floating windows correctly
    std::vector<Surface*> tiledWindows;
    for (auto *s : m_surfaces) {
        if (!s->isFloating()) {
            tiledWindows.push_back(s);
        }
    }

    int count = tiledWindows.size();

    if (count == 0)
        return;

    // If only one window, make it fullscreen
    if (count == 1) {
        Surface *surface = tiledWindows.at(0);

        auto tl = surface->tl();

        LSize extra = {
            tl->extraGeometry().left + tl->extraGeometry().right,
            tl->extraGeometry().top + tl->extraGeometry().bottom
        };

        if (surface->mapped() && surface->tl()) {
            surface->setPos({m_gap + containerPos.x(), m_gap + containerPos.y()});
            tl->configureSize({
                containerSize.width() - extra.width() - 2 * m_gap,
                containerSize.height() - extra.height() - 2 * m_gap
            });
        }
        return;
    }

    int masterWidth = static_cast<int>(containerSize.width() * m_masterRatio) - m_gap;
    int stackWidth = containerSize.width() - masterWidth - m_gap;
    int stackHeight = (containerSize.height() - (count - 1) * m_gap) / (count - 1);

    for (size_t i = 0; i < count; ++i) {
        Surface *surface = tiledWindows.at(i);

        if (!surface->mapped() || surface->minimized())
            return;


        if (surface->mapped() && surface->tl()) {
            auto tl = surface->tl();

            LSize extra = {
                tl->extraGeometry().left + tl->extraGeometry().right,
                tl->extraGeometry().top + tl->extraGeometry().bottom
            };

            if (i == 0) {
                // Master window takes the left portion
                surface->setPos({m_gap + containerPos.x(), m_gap + containerPos.y()});
                tl->configureSize({
                    masterWidth - extra.width(),
                    containerSize.height() - extra.height() - 2 * m_gap
                });
            } else {
                // Stack windows are arranged on the right
                surface->setPos({
                    containerPos.x() + masterWidth + m_gap,
                    containerPos.y() + (i - 1) * (stackHeight + m_gap) + m_gap
                });

                tl->configureSize({
                    stackWidth - extra.width(),
                    stackHeight - extra.height() - m_gap
                });
            }
        }
    }
}

void LayoutManager::focusPrevWindow() noexcept {
    if (m_focus_index <= 0) {
        if (m_cycle_window_focus) {
            m_focus_index = m_surfaces.size();
        } else {
            return;
        }
    }

    m_focus_index--;

    Surface *surface = m_surfaces[m_focus_index];

    if (!surface)
        return;

    auto seat = compositor()->seat();
    seat->pointer()->setFocus(surface);
    seat->keyboard()->setFocus(surface);

    auto tl = surface->tl();
    if (tl && !tl->activated()) {
        tl->configureState(tl->state() | ToplevelRole::Activated);
    }
}

void LayoutManager::focusNextWindow() noexcept {
    if (m_focus_index >= m_surfaces.size() - 1) {
        if (m_cycle_window_focus) {
            m_focus_index = -1;
        } else {
            return;
        }
    }

    m_focus_index++;

    Surface *surface = m_surfaces[m_focus_index];

    if (!surface)
        return;

    auto seat = compositor()->seat();
    seat->pointer()->setFocus(surface);
    seat->keyboard()->setFocus(surface);

    auto tl = surface->tl();
    if (tl && !tl->activated()) {
        tl->configureState(tl->state() | ToplevelRole::Activated);
    }
}

void LayoutManager::focusWindow(Surface *surface) noexcept {
    if (!surface || !surface->mapped()) return;

    // Find the index of the window in the surface list
    auto it = std::find(m_surfaces.begin(), m_surfaces.end(), surface);
    if (it == m_surfaces.end()) return;  // Surface not found

    m_focus_index = std::distance(m_surfaces.begin(), it);

    auto seat = compositor()->seat();
    seat->pointer()->setFocus(surface);
    seat->keyboard()->setFocus(surface);

    auto tl = surface->tl();
    if (tl && !tl->activated()) {
        tl->configureState(tl->state() | ToplevelRole::Activated);
    }
}

void LayoutManager::setFocusIndex(const int &index) noexcept {
    m_focus_index = index;

    Surface *surface = m_surfaces[m_focus_index];

    if (!surface)
        return;

    auto seat = compositor()->seat();
    seat->pointer()->setFocus(surface);
    seat->keyboard()->setFocus(surface);

    auto tl = surface->tl();
    if (tl && !tl->activated()) {
        tl->configureState(tl->state() | ToplevelRole::Activated);
    }
}

void LayoutManager::setAvailableGeometry(const LRect &rect) noexcept {
    m_availGeo = rect;
    updateLayout();
}

void LayoutManager::increaseMasterWidth(const float &dw) noexcept {
    if (m_masterRatio + dw < 1.0f) {
        m_masterRatio += dw;
    }

    updateLayout();
}

void LayoutManager::decreaseMasterWidth(const float &dw) noexcept {
    if (m_masterRatio - dw > 0.0f) {
        m_masterRatio -= dw;
    }

    updateLayout();
}

void LayoutManager::swapMaster() noexcept {
    if (m_surfaces.empty() || m_focus_index == 0)
        return;

    auto count = m_surfaces.size();

    if (count == 1)
        return;

    util::fast_swap(m_surfaces, 0, m_focus_index);
    m_focus_index = 0;
    updateLayout();
}

void LayoutManager::moveWindowUp() noexcept {
    if (m_surfaces.empty())
        return;

    auto count = m_surfaces.size();

    if (count == 1)
        return;

    if (m_focus_index == 0)
        return;

    util::fast_swap(m_surfaces, m_focus_index, m_focus_index - 1);
    m_focus_index--;
    updateLayout();
}

void LayoutManager::moveWindowDown() noexcept {

    if (m_surfaces.empty())
        return;

    auto count = m_surfaces.size();

    if (count == 1)
        return;

    if (m_focus_index == count - 1)
        return;

    util::fast_swap(m_surfaces, m_focus_index, m_focus_index + 1);
    m_focus_index++;
    updateLayout();
}

Surface* LayoutManager::focusedWindow() const noexcept {
    return m_surfaces.at(m_focus_index);
}

// Focuses previously focused window
// Used when switching to a monitor
void LayoutManager::focusOld() noexcept {
    if (m_surfaces.empty() || m_focus_index < 0 || m_focus_index > m_surfaces.size())
        return;

    focusWindow(m_surfaces.at(m_focus_index));
}
