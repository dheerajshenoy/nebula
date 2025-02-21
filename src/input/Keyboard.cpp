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

    Output *output = (Output*) cursor()->output();

    const bool LEFT_META  { isKeyCodePressed(KEY_LEFTMETA)  };
    const bool LEFT_SHIFT { isKeyCodePressed(KEY_LEFTSHIFT) };
    const bool LEFT_ALT   { isKeyCodePressed(KEY_LEFTALT)   };
    const bool LEFT_CTRL  { isKeyCodePressed(KEY_LEFTCTRL)  };

    const std::string &terminal = "alacritty";

    // Launch terminal
    if (LEFT_META && LEFT_SHIFT && event.keyCode() == KEY_ENTER && event.state() == LKeyboardKeyEvent::Pressed) {
        LLauncher::launch(terminal);
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

    // focus window

    if (LEFT_META && event.keyCode() == KEY_J && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->focusNextWindow();
        return;
    }

    if (LEFT_META && event.keyCode() == KEY_K && event.state() == LKeyboardKeyEvent::Pressed) {
        output->layoutManager()->focusPrevWindow();
        return;
    }

    /* The AuxFunc flag adds the Ctrl + Shift + ESC shortcut to quit, ensure
     * to add a way to exit if you remove it */
    G::scene().handleKeyboardKeyEvent(event, LScene::WaylandEvents | LScene::PointerConstraints);

}
