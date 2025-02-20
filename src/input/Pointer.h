#ifndef POINTER_H
#define POINTER_H

#include <LPointer.h>
#include "../roles/ToplevelRole.h"
#include "../Surface.h"
#include <LLauncher.h>
#include <iostream>

using namespace Louvre;


class Pointer final : public LPointer {
public:
    Pointer(const void *params) noexcept;

inline void set_focus_on_hover(const bool &state) noexcept { m_focus_on_hover = state; }

void pointerMoveEvent(const LPointerMoveEvent &event) override;
void pointerButtonEvent(const LPointerButtonEvent &event) override;
void pointerScrollEvent(const LPointerScrollEvent &event) override;

void pointerSwipeBeginEvent(const LPointerSwipeBeginEvent &event) override;
void pointerSwipeUpdateEvent(const LPointerSwipeUpdateEvent &event) override;
void pointerSwipeEndEvent(const LPointerSwipeEndEvent &event) override;

void pointerPinchBeginEvent(const LPointerPinchBeginEvent &event) override;
void pointerPinchUpdateEvent(const LPointerPinchUpdateEvent &event) override;
void pointerPinchEndEvent(const LPointerPinchEndEvent &event) override;

void pointerHoldBeginEvent(const LPointerHoldBeginEvent &event) override;
void pointerHoldEndEvent(const LPointerHoldEndEvent &event) override;

void setCursorRequest(const LClientCursor &clientCursor) override;

bool maybeMoveOrResize(const LPointerButtonEvent &event) noexcept;

private:
    LWeak<LView> m_cursorOwner;
    bool m_focus_on_hover = false;
};

#endif // POINTER_H
