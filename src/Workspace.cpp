#include "Workspace.hpp"
#include "Output.h"
#include "utils/Global.h"

Workspace::Workspace(Output *output) : LLayerView(G::scene().mainView()),
    m_layoutManager(new LayoutManager(output)) {
    setPos(0, 0);
    setSize(output->availableGeometry().size());
}

Workspace::~Workspace() {

}
