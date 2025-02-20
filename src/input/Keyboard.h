#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <LKeyboard.h>
#include <LCursor.h>

using namespace Louvre;

class Keyboard final : public LKeyboard
{
public:
    Keyboard(const void *params) noexcept;
    void keyEvent(const LKeyboardKeyEvent &event) override;

private:
};

#endif // KEYBOARD_H
