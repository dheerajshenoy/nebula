#ifndef TOPLEVELROLE_H
#define TOPLEVELROLE_H

#include <LToplevelRole.h>
#include "../scene/SSD.h"

using namespace Louvre;

class Surface;

/*
 * This represents a standard desktop window that supports typical operations like minimizing, maximizing, setting fullscreen, etc.
 */
class ToplevelRole final : public LToplevelRole
{
public:
    ToplevelRole(const void *params) noexcept;
    Surface *surf() noexcept { return reinterpret_cast<Surface*>(surface()); }
    std::unique_ptr<SSD> ssd;
    const LPoint &rolePos() const override;
    void configureRequest() override;
    void startMoveRequest(const LEvent &triggeringEvent) override;
    void startResizeRequest(const LEvent &triggeringEvent, LBitset<LEdge> edges) override;
    void setMaximizedRequest() override;
    void unsetMaximizedRequest() override;
    void setFullscreenRequest(LOutput *output) override;
    void unsetFullscreenRequest() override;
    void setMinimizedRequest() override;
    void showWindowMenuRequest(const LEvent &triggeringEvent, Int32 x, Int32 y) override;
    void closeRequest() override;
    void atomsChanged(LBitset<AtomChanges> changes, const Atoms &prev) override;
    void titleChanged() override;
    void appIdChanged() override;
    void preferredDecorationModeChanged() override;
    void decorationModeChanged() noexcept;
};

#endif // TOPLEVELROLE_H
