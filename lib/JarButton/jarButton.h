#ifndef BUTTON_H
#define BUTTON_H

const char beepButtonA[] PROGMEM = "!c32";
const char beepButtonB[] PROGMEM = "!e32";
const char beepButtonC[] PROGMEM = "!g32";

class JarButton
{
public:
  static Zumo32U4Buzzer buzzer;
  static char monitor();
  static bool aIsPressed();
  static bool bIsPressed();
  static bool cIsPressed();

private:
};

#endif
