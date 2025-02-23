#pragma once

#include <LLayerView.h>
#include "LayoutManager.hpp"

using namespace Louvre;

class Workspace : public LLayerView {

public:
    Workspace(Output *output);
    ~Workspace();

inline LayoutManager* layoutManager() noexcept { return m_layoutManager; }

private:
    LayoutManager *m_layoutManager;
};
