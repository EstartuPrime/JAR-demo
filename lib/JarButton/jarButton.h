#ifndef BUTTON_H
#define BUTTON_H

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
