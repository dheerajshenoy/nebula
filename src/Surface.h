#ifndef SURFACE_H
#define SURFACE_H

#include <LLauncher.h>
#include <LSurface.h>
#include "scene/SurfaceView.h"
#include <LCursor.h>
#include <LLog.h>
#include "Compositor.h"
#include "Output.h"

#include "Workspace.hpp"

using namespace Louvre;

class ToplevelRole;

class Surface final : public LSurface {
public:
    Surface(const void *params);

SurfaceView view;

ToplevelRole *tl() noexcept
{
    return reinterpret_cast<ToplevelRole*>(toplevel());
}

LView *getView() noexcept;
bool hasMappedChildSurface() const noexcept;

void roleChanged() override;
void layerChanged() override;
void orderChanged() override;
void mappingChanged() override;
void minimizedChanged() override;
void parentChanged() override;

ToplevelRole *closestToplevelParent() const noexcept;

void setFloating(const bool &state) noexcept;
inline bool isFloating() noexcept { return m_floating; }
void toggleFloating() noexcept;

void setFullscreen(const bool &state) noexcept;
inline bool isFullscreen() noexcept { return m_fullscreen; }
void toggleFullscreen() noexcept;

private:
    bool m_floating = false;
    bool m_fullscreen = false;
};

#endif // SURFACE_H
