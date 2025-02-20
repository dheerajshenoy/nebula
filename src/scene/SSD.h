#ifndef TOPLEVELDECORATIONVIEW_H
#define TOPLEVELDECORATIONVIEW_H

#include <LToplevelRole.h>
#include <LLayerView.h>
#include <LSolidColorView.h>
#include <LWeak.h>
#include "SSDTouchable.h"

using namespace Louvre;

class ToplevelRole;

/* Container for toplevel server-side decoration */

class SSD final : public LLayerView {
public:
    SSD(ToplevelRole *toplevel) noexcept;
    ~SSD();

void updateGeometry() noexcept;

const LPoint &nativePos() const noexcept override;
bool nativeMapped() const noexcept override;

LWeak<ToplevelRole> toplevel;
LBitset<LToplevelRole::State> prevState;

// SSDTouchable close      { this, SSDTouchable::Close, 0, &titlebar };
// SSDTouchable maximize   { this, SSDTouchable::Maximize, 0, &titlebar };
// SSDTouchable minimize   { this, SSDTouchable::Minimize, 0, &titlebar };

// Edges / corners
SSDTouchable titlebar   { this, SSDTouchable::Edge, LEdgeTop, this };
SSDTouchable L  { this, SSDTouchable::Edge, LEdgeLeft, this };
SSDTouchable BL { this, SSDTouchable::Edge, LEdgeLeft | LEdgeBottom, this };
SSDTouchable B  { this, SSDTouchable::Edge, LEdgeBottom, this };
SSDTouchable BR { this, SSDTouchable::Edge, LEdgeRight | LEdgeBottom, this };
SSDTouchable R  { this, SSDTouchable::Edge, LEdgeRight, this };

// Border for active/inactive window

private:
    bool m_titlebar_shown = false;
    int m_border_width = 3; // TODO: Add this somewhere

};

#endif // TOPLEVELDECORATIONVIEW_H
