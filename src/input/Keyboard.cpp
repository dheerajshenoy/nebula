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

    if (event.state() == LKeyboardKeyEvent::Pressed) {

        // Launch terminal
        if (LEFT_META && event.keyCode() == KEY_ENTER) {
            if (LEFT_SHIFT) {
                LLauncher::launch(terminal);
            } else {
                output->layoutManager()->swapMaster();
            }
        }

        // Kill focused window
        if (LEFT_META && LEFT_SHIFT && event.keyCode() == KEY_Q) {
            if (LSurface* focusedSurface = focus()) {
                if (focusedSurface->toplevel()) {
                    focusedSurface->toplevel()->closeRequest();
                }
            }
        }

        // Rofi
        if (LEFT_META && event.keyCode() == KEY_P) {
            LLauncher::launch("rofi -show drun");
            return;
        }

        // Kill nebula

        if (LEFT_META && LEFT_SHIFT && event.keyCode() == KEY_C) {
            compositor()->finish();
            return;
        }

        // togglefloating
        if (LEFT_META && event.keyCode() == KEY_T) {
            if (Surface* focusedSurface = static_cast<Surface*>(focus())) {
                focusedSurface->toggleFloating();
            }
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_COMMA) {
            static_cast<Compositor*>(compositor())->focusPrevMonitor();
            return;
        }


        if (LEFT_META && event.keyCode() == KEY_DOT) {
            static_cast<Compositor*>(compositor())->focusNextMonitor();
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_F) {
            if (Surface* focusedSurface = static_cast<Surface*>(focus())) {
                focusedSurface->toggleFullscreen();
            }
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_L) {
            output->layoutManager()->increaseMasterWidth(0.05);
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_H) {
            output->layoutManager()->decreaseMasterWidth(0.05);
            return;
        }

        // focus next window
        if (LEFT_META && event.keyCode() == KEY_J) {
            if (LEFT_SHIFT) {
                output->layoutManager()->moveWindowDown();
            } else {
                output->layoutManager()->focusNextWindow();
            }
            return;
        }

        // focus previous window
        if (LEFT_META && event.keyCode() == KEY_K) {
            if (LEFT_SHIFT) {
                output->layoutManager()->moveWindowUp();
            } else {
                output->layoutManager()->focusPrevWindow();
            }
            return;
        }

        // Move window to next monitor
        if (LEFT_META && event.keyCode() == KEY_O) {
            output->layoutManager()->moveWindowNextMonitor();
            return;
        }

        // Move window to prev monitor
        if (LEFT_META && event.keyCode() == KEY_I) {
            output->layoutManager()->moveWindowPrevMonitor();
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_1)  {
            output->setWorkspace(0);
            return;
        }

        if (LEFT_META && event.keyCode() == KEY_2)  {
            output->setWorkspace(1);
            return;
        }
    }


    G::scene().handleKeyboardKeyEvent(event, LScene::WaylandEvents | LScene::PointerConstraints);

}
