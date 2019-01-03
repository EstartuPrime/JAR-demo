#include <Zumo32U4.h>
#include <jarButton.h>

#ifndef BUTTON_CPP
#define BUTTON_CPP

const char beepButtonA[] PROGMEM = "!c32";
const char beepButtonB[] PROGMEM = "!e32";
const char beepButtonC[] PROGMEM = "!g32";

Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

char JarButton::monitor()
{

    if (buttonA.getSingleDebouncedPress())
    {
        buzzer.playFromProgramSpace(beepButtonA);
        return 'A';
    }

    if (buttonB.getSingleDebouncedPress())
    {
        buzzer.playFromProgramSpace(beepButtonB);
        return 'B';
    }

    if (buttonC.getSingleDebouncedPress())
    {
        buzzer.playFromProgramSpace(beepButtonC);
        return 'C';
    }

    return 0;
}

bool JarButton::aIsPressed()
{
    return buttonA.isPressed();
}
bool JarButton::bIsPressed()
{
    return buttonB.isPressed();
}
bool JarButton::cIsPressed()
{
    return buttonC.isPressed();
}

#endif
