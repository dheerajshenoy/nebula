#include <LDND.h>
#include <LCursor.h>
#include <LClientCursor.h>
#include <LClient.h>
#include "Pointer.h"
#include "../utils/Global.h"
#include "../utils/Settings.h"
#include "../scene/Scene.h"
#include "../utils/Assets.h"

Pointer::Pointer(const void *params) noexcept : LPointer(params)
{
    enableNaturalScrollingX(false);
    enableNaturalScrollingY(false);

}

void Pointer::pointerMoveEvent(const LPointerMoveEvent &event)
{
    G::scene().handlePointerMoveEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);

    if (seat()->dnd()->dragging() && seat()->dnd()->triggeringEvent().type() != LEvent::Type::Touch)
        {
            if (seat()->dnd()->action() == LDND::Copy)
                cursor()->setCursor(G::assets()->cursors.copy.get());
            else if (seat()->dnd()->action() == LDND::Move || seat()->dnd()->action() == LDND::Ask)
                cursor()->setCursor(G::assets()->cursors.drag.get());
            else if (seat()->dnd()->action() == LDND::NoAction)
                cursor()->setCursor(G::assets()->cursors.denied.get());

            /*
             * Some clients update the cursor during DND sessions, but not always
             * cursor()->setCursor(seat()->dnd()->origin()->client()->lastCursorRequest()); */

            return;
        }


    /* Normally, we should restore the cursor if no surface is focused. However, during
     * toplevel move or resize sessions, clients typically update the cursor to indicate
     * the edge or corner being resized, but the cursor position might move outside the surface,
     * causing it to lose focus.*/

    if (!seat()->toplevelMoveSessions().empty() || !seat()->toplevelResizeSessions().empty())
        return;

    if (!G::scene().pointerFocus().empty() && G::scene().pointerFocus().front()->userData() == G::SSDEdge)
        return;

    if (m_focus_on_hover) {
        Surface* surfaceUnderCursor = (Surface*)surfaceAt(cursor()->pos());
        if (surfaceUnderCursor) {
            if (surfaceUnderCursor->layer() != Louvre::LLayerBackground) {
                setFocus(surfaceUnderCursor);
                seat()->pointer()->setFocus(surfaceUnderCursor);
                seat()->keyboard()->setFocus(surfaceUnderCursor);
                // surfaceUnderCursor->raise();
                auto tl = surfaceUnderCursor->tl();
                if (tl && !tl->activated()) {
                    tl->configureState(tl->pendingConfiguration().state | LToplevelRole::Activated);
                }
                // m_cursorOwner = surfaceUnderCursor->views();
            }
        }
    }


    if (focus() && focus()->layer() != Louvre::LLayerBackground) {
        cursor()->setCursor(focus()->client()->lastCursorRequest());
    }
    else {
        cursor()->useDefault();
        cursor()->setVisible(true);
    }

}

void Pointer::pointerButtonEvent(const LPointerButtonEvent &event)
{
    if (maybeMoveOrResize(event))
        G::scene().handlePointerButtonEvent(event, 0);
    else
        G::scene().handlePointerButtonEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerScrollEvent(const LPointerScrollEvent &event)
{
    G::scene().handlePointerScrollEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerSwipeBeginEvent(const LPointerSwipeBeginEvent &event)
{
    G::scene().handlePointerSwipeBeginEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerSwipeUpdateEvent(const LPointerSwipeUpdateEvent &event)
{
    G::scene().handlePointerSwipeUpdateEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerSwipeEndEvent(const LPointerSwipeEndEvent &event)
{
    G::scene().handlePointerSwipeEndEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerPinchBeginEvent(const LPointerPinchBeginEvent &event)
{
    G::scene().handlePointerPinchBeginEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerPinchUpdateEvent(const LPointerPinchUpdateEvent &event)
{
    G::scene().handlePointerPinchUpdateEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerPinchEndEvent(const LPointerPinchEndEvent &event)
{
    G::scene().handlePointerPinchEndEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerHoldBeginEvent(const LPointerHoldBeginEvent &event)
{
    G::scene().handlePointerHoldBeginEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::pointerHoldEndEvent(const LPointerHoldEndEvent &event)
{
    G::scene().handlePointerHoldEndEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);
}

void Pointer::setCursorRequest(const LClientCursor &clientCursor)
{
    if (!seat()->toplevelMoveSessions().empty() || !seat()->toplevelResizeSessions().empty())
        return;

    if (seat()->dnd()->dragging() && seat()->dnd()->triggeringEvent().type() != LEvent::Type::Touch)
        return;

    /* Allows to set the cursor only if one of its surfaces has pointer focus */
    if (focus() && focus()->client() == clientCursor.client())
        cursor()->setCursor(clientCursor);
}

bool Pointer::maybeMoveOrResize(const LPointerButtonEvent &event) noexcept
{
    if (!focus()
        || event.state() != LPointerButtonEvent::Pressed
        || !seat()->keyboard()->isKeyCodePressed(KEY_LEFTMETA)
        || !(event.button() == LPointerButtonEvent::Left || event.button() == LPointerButtonEvent::Right))
        return false;

    ToplevelRole *toplevel { focus()->toplevel() ?
        static_cast<ToplevelRole*>(focus()->toplevel()) :
        static_cast<Surface*>(focus())->closestToplevelParent() };

    if (!toplevel || toplevel->fullscreen() || toplevel->resizeSession().isActive() || toplevel->moveSession().isActive())
        return false;

    const LPointF mpos { cursor()->pos() };
    LView *view { toplevel->surf()->getView() };
    LBitset<LEdge> anchor { 0 };
    LXCursor *cursor { G::cursors().move };

    // Meta + Right Click to resize window
    if (event.button() == LPointerButtonEvent::Right)
        {
            LPointF vpos { view->pos() };
            LSizeF vsz { view->size() };

            if (mpos.x() >= vpos.x() + 0.0000f * vsz.x() && mpos.x() < vpos.x() + 0.3333f * vsz.x() &&
                mpos.y() >= vpos.y() + 0.0000f * vsz.y() && mpos.y() < vpos.y() + 0.3333f * vsz.y())
                {
                    anchor = LEdgeTop | LEdgeLeft;
                    cursor = G::cursors().top_left_corner;
                }
            else if (mpos.x() >= vpos.x() + 0.3333f * vsz.x() && mpos.x() < vpos.x() + 0.6666f * vsz.x() &&
                mpos.y() >= vpos.y() + 0.0000f * vsz.y() && mpos.y() < vpos.y() + 0.3333f * vsz.y())
                {
                    anchor = LEdgeTop;
                    cursor = G::cursors().top_side;
                }
            else if (mpos.x() >= vpos.x() + 0.6666f * vsz.x() && mpos.x() < vpos.x() + 1 * vsz.x() &&
                mpos.y() >= vpos.y() + 0.0000f * vsz.y() && mpos.y() < vpos.y() + 0.3333f * vsz.y())
                {
                    anchor = LEdgeTop | LEdgeRight;
                    cursor = G::cursors().top_right_corner;
                }
            else if (mpos.x() >= vpos.x() + 0.0000f * vsz.x() && mpos.x() < vpos.x() + 0.3333f * vsz.x() &&
                mpos.y() >= vpos.y() + 0.3333f * vsz.y() && mpos.y() < vpos.y() + 0.6666f * vsz.y())
                {
                    anchor = LEdgeLeft;
                    cursor = G::cursors().left_side;
                }
            else if (mpos.x() >= vpos.x() + 0.6666f * vsz.x() && mpos.x() < vpos.x() + 1 * vsz.x() &&
                mpos.y() >= vpos.y() + 0.3333f * vsz.y() && mpos.y() < vpos.y() + 0.6666f * vsz.y())
                {
                    anchor = LEdgeRight;
                    cursor = G::cursors().right_side;
                }
            else if (mpos.x() >= vpos.x() + 0.0000f * vsz.x() && mpos.x() < vpos.x() + 0.3333f * vsz.x() &&
                mpos.y() >= vpos.y() + 0.6666f * vsz.y() && mpos.y() < vpos.y() + 1 * vsz.y())
                {
                    anchor = LEdgeBottom | LEdgeLeft;
                    cursor = G::cursors().bottom_left_corner;
                }
            else if (mpos.x() >= vpos.x() + 0.3333f * vsz.x() && mpos.x() < vpos.x() + 0.6666f * vsz.x() &&
                mpos.y() >= vpos.y() + 0.6666f * vsz.y() && mpos.y() < vpos.y() + 1 * vsz.y())
                {
                    anchor = LEdgeBottom;
                    cursor = G::cursors().bottom_side;
                }
            else if (mpos.x() >= vpos.x() + 0.6666f * vsz.x() && mpos.x() < vpos.x() + 1 * vsz.x() &&
                mpos.y() >= vpos.y() + 0.6666f * vsz.y() && mpos.y() < vpos.y() + 1 * vsz.y())
                {
                    anchor = LEdgeBottom | LEdgeRight;
                    cursor = G::cursors().bottom_right_corner;
                }
        }

    Louvre::cursor()->setCursor(cursor);
    m_cursorOwner = view;

    if (anchor)
        toplevel->startResizeRequest(event, anchor);
    else
        toplevel->startMoveRequest(event);

    toplevel->configureState(toplevel->pendingConfiguration().state | LToplevelRole::Activated);
    toplevel->surface()->raise();
    seat()->dismissPopups();

    return true;
}
