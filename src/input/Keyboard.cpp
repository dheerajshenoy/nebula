#include <LKeyboardKeyEvent.h>
#include <LSessionLockManager.h>
#include <LLauncher.h>
#include "Keyboard.h"
#include "../utils/Global.h"
#include "../scene/Scene.h"
#include "../utils/Settings.h"
#include <LCompositor.h>
#include "../Output.h"
#include "../LayoutManager.hpp"

Keyboard::Keyboard(const void *params) noexcept : LKeyboard(params)
{
    /* Key press repeat rate */
    setRepeatInfo(32, 500);

    /* Keymap sent to clients and used by the compositor, check the LKeyboard class or XKB doc */
    setKeymap(
        nullptr,  // Rules
        nullptr,  // Model
        nullptr,  // Layout
        nullptr); // Options
}

void Keyboard::keyEvent(const LKeyboardKeyEvent &event)
{

    /* Launches a session lock client */
    if (sessionLockManager()->state() != LSessionLockManager::Locked
        && event.keyCode() == KEY_POWER
        && event.state() == LKeyboardKeyEvent::Released)
        LLauncher::launch(SETTINGS_SESSION_LOCK_CLIENT);

    Output *output = static_cast<Output*>(cursor()->output());

    const bool LEFT_META  { isKeyCodePressed(KEY_LEFTMETA)  };
    const bool LEFT_SHIFT { isKeyCodePressed(KEY_LEFTSHIFT) };
    const bool LEFT_ALT   { isKeyCodePressed(KEY_LEFTALT)   };
    const bool LEFT_CTRL  { isKeyCodePressed(KEY_LEFTCTRL)  };

    const std::string &terminal = "alacritty";

    // Launch terminal
    if (LEFT_META && event.keyCode() == KEY_ENTER && event.state() == LKeyboardKeyEvent::Pressed) {
        if (LEFT_SHIFT) {
            LLauncher::launch(terminal);
        } else {
            output->layoutManager()->swapMaster();
        }
    }

    // Kill focused window
    if (LEFT_META && LEFT_SHIFT && event.keyCode() == KEY_Q && event.state() == LKeyboardKeyEvent::Pressed) {
        if (LSurface* focusedSurface = focus()) {
            if (focusedSurface->toplevel()) {
                focusedSurface->toplevel()->closeRequest();
            }
        }
    }

    // Rofi
    if (LEFT_META && event.keyCode() == KEY_P && event.state() == LKeyboardKeyEvent::Pressed) {
        LLauncher::launch("rofi -show drun");
        return;
    }

    // Kill nebula

    if (LEFT_META && LEFT_SHIFT && event.keyCode() == KEY_C && event.state() == LKeyboardKeyEvent::Pressed) {
        compositor()->finish();
        return;
    }

    // togglefloating
    if (LEFT_META && event.keyCode() == KEY_T && event.state() == LKeyboardKeyEvent::Pressed) {
        if (Surface* focusedSurface = static_cast<Surface*>(focus())) {
            focusedSurface->toggleFloating();
        }
        return;
    }

    if (LEFT_META && event.keyCode() == KEY_COMMA && event.state() == LKeyboardKeyEvent::Pressed) {
        static_cast<Compositor*>(compositor())->focusPrevMonitor();
        return;
    }


    if (LEFT_META && event.keyCode() == KEY_DOT && event.state() == LKeyboardKeyEvent::Pressed) {
        static_cast<Compositor*>(compositor())->focusNextMonitor();
        return;
    }

    if (LEFT_META && event.keyCode() == KEY_F && event.state() == LKeyboardKeyEvent::Pressed) {
        if (Surface* focusedSurface = static_cast<Surface*>(focus())) {
            focusedSurface->toggleFullscreen();
        }
        return;
    }

    if (LEFT_META && event.keyCode() == KEY_L && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->increaseMasterWidth(0.05);
        return;
    }

    if (LEFT_META && event.keyCode() == KEY_H && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->decreaseMasterWidth(0.05);
        return;
    }

    // focus next window
    if (LEFT_META && event.keyCode() == KEY_J && event.state() == LKeyboardKeyEvent::Pressed) {
        if (LEFT_SHIFT) {
            output->layoutManager()->moveWindowDown();
        } else {
            output->layoutManager()->focusNextWindow();
        }
        return;
    }

    // focus previous window
    if (LEFT_META && event.keyCode() == KEY_K && event.state() == LKeyboardKeyEvent::Pressed) {
        if (LEFT_SHIFT) {
            output->layoutManager()->moveWindowUp();
        } else {
            output->layoutManager()->focusPrevWindow();
        }
        return;
    }

    // Move window to next monitor
    if (LEFT_META && event.keyCode() == KEY_O && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->moveWindowNextMonitor();
        return;
    }

    // Move window to prev monitor
    if (LEFT_META && event.keyCode() == KEY_I && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->moveWindowPrevMonitor();
        return;
    }

    G::scene().handleKeyboardKeyEvent(event, LScene::WaylandEvents | LScene::PointerConstraints);

}
