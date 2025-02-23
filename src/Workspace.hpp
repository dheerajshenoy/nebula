#pragma once

#include <LLayerView.h>
#include "LayoutManager.hpp"

using namespace Louvre;

class Workspace : public LLayerView {

public:
    Workspace(Output *output, const size_t &index);
    ~Workspace();
    LLayerView background { this };
    LLayerView surfaces { this };
    LLayerView overlay { this };
    inline LayoutManager* layoutManager() noexcept { return m_layoutManager; }
    inline size_t index() const noexcept { return m_index; }
    void stealChildren() noexcept;
    void returnChildren() noexcept;
    void clipChildren() noexcept;
    void show(const bool &state) noexcept;

private:
    Output *m_output { nullptr };
    LayoutManager* m_layoutManager;
    size_t m_index;
};
