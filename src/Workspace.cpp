#include "Workspace.hpp"
#include "Output.h"
#include "utils/Global.h"

Workspace::Workspace(Output *output,
                     const size_t &index) : LLayerView(G::scene().mainView()),
    m_output(output),
    m_layoutManager(new LayoutManager(output)),
    m_index(index) {
    setPos(0, 0);
    setSize(output->availableGeometry().size());
}

Workspace::~Workspace() {
    delete m_layoutManager;
}

void Workspace::show(const bool &state) noexcept {

    if (state == (parent() != nullptr))
        return;

    if (state)
        setParent(&m_output->workspacesContainer);
    else
        setParent(nullptr);

}

void Workspace::stealChildren() noexcept {

}
