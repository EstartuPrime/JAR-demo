/* This demo program shows many features of the Zumo 32U4.

It uses the buttons, LCD, and buzzer to provide a user interface.
It presents a menu to the user that lets the user select from
several different demos.

To use this demo program, you will need to have the LCD connected
to the Zumo 32U4.  If you cannot see any text on the LCD,
try rotating the contrast potentiometer. */

#include <Wire.h>
#include <Zumo32U4.h>
#include <jarButton.h>
#include <jarMenu.h>

JarButton jb;

Zumo32U4LCD lcd;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
LSM303 compass;
L3G gyro;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;

// A couple of simple tunes, stored in program space.
const char beepBrownout[] PROGMEM = "<c8";
const char beepWelcome[] PROGMEM = ">g32>>c32";
const char beepThankYou[] PROGMEM = ">>c32>g32";

// Custom characters for the LCD:

// This character is a back arrow.
const char backArrow[] PROGMEM = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This character is two chevrons pointing up.
const char forwardArrows[] PROGMEM = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This character is two chevrons pointing down.
const char reverseArrows[] PROGMEM = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This character is two solid arrows pointing up.
const char forwardArrowsSolid[] PROGMEM = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This character is two solid arrows pointing down.
const char reverseArrowsSolid[] PROGMEM = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  lcd.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  // es wird nur der start-index übergeben und ab da werden immer 8 "linien" gelesen
  lcd.loadCustomCharacter(levels + 0, 0);  // 1 bar : levels[0- 7]-> [00000001]111111
  lcd.loadCustomCharacter(levels + 1, 1);  // 2 bars: levels[1- 8]-> 0[00000011]11111
  lcd.loadCustomCharacter(levels + 2, 2);  // 3 bars: levels[2- 9]-> 00[00000111]1111
  lcd.loadCustomCharacter(levels + 3, 3);  // 4 bars: levels[3-10]-> 000[00001111]111
  lcd.loadCustomCharacter(levels + 4, 4);  // 5 bars: levels[4-11]-> 0000[00011111]11
  lcd.loadCustomCharacter(levels + 5, 5);  // 6 bars: levels[5-12]-> 00000[00111111]1
  lcd.loadCustomCharacter(levels + 6, 6);  // 7 bars: levels[6-13]-> 000000[01111111]
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  lcd.loadCustomCharacter(forwardArrows, 0);
  lcd.loadCustomCharacter(reverseArrows, 1);
  lcd.loadCustomCharacter(forwardArrowsSolid, 2);
  lcd.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  lcd.clear();
  lcd.gotoXY(0,1);
  lcd.print(F("\7B"));
  lcd.gotoXY(0,0);
}

// Blinks all three LEDs in sequence.
void ledDemo()
{
  displayBackArrow();

  uint8_t state = 3;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (jb.monitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 4) { state = 0; }

      switch (state)
      {
      case 0:
        jb.buzzer.play("c32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Red   "));
        ledRed(1);
        ledGreen(0);
        ledYellow(0);
        break;

      case 1:
        jb.buzzer.play("e32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Green"));
        ledRed(0);
        ledGreen(1);
        ledYellow(0);
        break;

      case 2:
        jb.buzzer.play("g32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Yellow"));
        ledRed(0);
        ledGreen(0);
        ledYellow(1);
        break;
      }
    }
  }

  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, -128};
  lcd.print(barChars[height]);
}

// Display line sensor readings. Holding button C turns off
// the IR emitters.
void lineSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();
  lcd.gotoXY(6, 1);
  lcd.print('C');

  unsigned int lineSensorValues[3];

  while (jb.monitor() != 'B')
  {
    bool emittersOff = jb.bIsPressed();

    if (emittersOff)
    {
      lineSensors.read(lineSensorValues, QTR_EMITTERS_OFF);
    }
    else
    {
      lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
    }

    lcd.gotoXY(1, 0);
    for (uint8_t i = 0; i < 3; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 2000, 0, 8);
      printBar(barHeight);
      lcd.print(' ');
    }

    // Display an indicator of whether emitters are on or
    // off.
    lcd.gotoXY(7, 1);
    if (emittersOff)
    {
      lcd.print('\xa5');  // centered dot
    }
    else
    {
      lcd.print('*');
    }
  }
}

// Display proximity sensor readings.
void proxSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();

  while (jb.monitor() != 'B')
  {
    bool proxLeftActive = proxSensors.readBasicLeft();
    bool proxFrontActive = proxSensors.readBasicFront();
    bool proxRightActive = proxSensors.readBasicRight();
    proxSensors.read();

    lcd.gotoXY(0, 0);
    printBar(proxSensors.countsLeftWithLeftLeds());
    printBar(proxSensors.countsLeftWithRightLeds());
    lcd.print(' ');
    printBar(proxSensors.countsFrontWithLeftLeds());
    printBar(proxSensors.countsFrontWithRightLeds());
    lcd.print(' ');
    printBar(proxSensors.countsRightWithLeftLeds());
    printBar(proxSensors.countsRightWithRightLeds());

    // On the last 3 characters of the second line, display
    // basic readings of the sensors taken without sending
    // IR pulses.
    lcd.gotoXY(5, 1);
    printBar(proxLeftActive);
    printBar(proxFrontActive);
    printBar(proxRightActive);
  }
}

// Starts I2C and initializes the inertial sensors.
void initInertialSensors()
{
  Wire.begin();
  compass.init();
  compass.enableDefault();
  gyro.init();
  gyro.enableDefault();
}

// Given 3 readings for axes x, y, and z, prints the sign
// and axis of the largest reading unless it is below the
// given threshold.
void printLargestAxis(int16_t x, int16_t y, int16_t z, uint16_t threshold)
{
  int16_t largest = x;
  char axis = 'X';

  if (abs(y) > abs(largest))
  {
    largest = y;
    axis = 'Y';
  }
  if (abs(z) > abs(largest))
  {
    largest = z;
    axis = 'Z';
  }

  if (abs(largest) < threshold)
  {
    lcd.print("  ");
  }
  else
  {
    bool positive = (largest > 0);
    lcd.print(positive ? '+' : '-');
    lcd.print(axis);
  }
}

// Print the direction of the largest rotation rate measured
// by the gyro and the up direction based on the
// accelerometer's measurement of gravitational acceleration
// (assuming gravity is the dominant force acting on the
// Zumo).
void inertialDemo()
{
  displayBackArrow();

  lcd.gotoXY(3, 0);
  lcd.print(F("Rot"));
  lcd.gotoXY(4, 1);
  lcd.print(F("Up"));

  while (jb.monitor() != 'B')
  {
    compass.read();
    gyro.read();

    lcd.gotoXY(6, 0);
    printLargestAxis(gyro.g.x, gyro.g.y, gyro.g.z, 2000);
    lcd.gotoXY(6, 1);
    printLargestAxis(compass.a.x, compass.a.y, compass.a.z, 200);
  }
}

// Provides an interface to test the motors. Holding button A or C
// causes the left or right motor to accelerate; releasing the
// button causes the motor to decelerate. Tapping the button while
// the motor is not running reverses the direction it runs.
//
// If the showEncoders argument is true, encoder counts are
// displayed on the first line of the LCD; otherwise, an
// instructional message is shown.
void motorDemoHelper(bool showEncoders)
{
  loadCustomCharactersMotorDirs();
  lcd.clear();
  lcd.gotoXY(1, 1);
  lcd.print(F("A \7B C"));

  int16_t leftSpeed = 0, rightSpeed = 0;
  int8_t leftDir = 1, rightDir = 1;
  uint16_t lastUpdateTime = millis() - 100;
  uint8_t btnCountA = 0, btnCountC = 0, instructCount = 0;

  int16_t encCountsLeft = 0, encCountsRight = 0;
  char buf[4];

  while (jb.monitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    // Update the LCD and motors every 50 ms.
    if ((uint16_t)(millis() - lastUpdateTime) > 50)
    {
      lastUpdateTime = millis();

      lcd.gotoXY(0, 0);
      if (showEncoders)
      {
        sprintf(buf, "%03d", encCountsLeft);
        lcd.print(buf);
        lcd.gotoXY(5, 0);
        sprintf(buf, "%03d", encCountsRight);
        lcd.print(buf);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          lcd.print("Hold=run");
        }
        else if (instructCount == 40)
        {
          lcd.print("Tap=flip");
        }
        if (++instructCount == 80) { instructCount = 0; }
      }

      if (jb.aIsPressed())
      {
        if (btnCountA < 4)
        {
          btnCountA++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          leftSpeed += 15;
        }
      }
      else
      {
        if (leftSpeed == 0 && btnCountA > 0 && btnCountA < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          // leftDir = -leftDir;

          // start the motor and stop when the encoders read a summized value >900
          // this is a test if the value 900 is a complete rotation.
          encCountsLeft = 0;
          motors.setLeftSpeed(50);
          while(encCountsLeft < 900) {
            encCountsLeft += encoders.getCountsAndResetLeft();

            lcd.gotoXY(0, 0);
            sprintf(buf, "%03d", encCountsLeft);
            lcd.print(buf);
          };
          motors.setLeftSpeed(0);
        }
        btnCountA = 0;
        leftSpeed -= 30;
      }

      if (jb.cIsPressed())
      {
        if (btnCountC < 4)
        {
          btnCountC++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          rightSpeed += 15;
        }
      }
      else
      {
        if (rightSpeed == 0 && btnCountC > 0 && btnCountC < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          // rightDir = -rightDir;

          encCountsRight = 0;
          motors.setRightSpeed(50);
          while(encCountsRight < 900) {
            encCountsRight += encoders.getCountsAndResetRight();

            lcd.gotoXY(5, 0);
            sprintf(buf, "%03d", encCountsRight);
            lcd.print(buf);
          };
          motors.setRightSpeed(0);
        }
        btnCountC = 0;
        rightSpeed -= 30;
      }

      leftSpeed = constrain(leftSpeed, 0, 400);
      rightSpeed = constrain(rightSpeed, 0, 400);

      motors.setSpeeds(leftSpeed * leftDir, rightSpeed * rightDir);

      lcd.gotoXY(1,1);
      lcd.print(btnCountA);
      lcd.gotoXY(6,1);
      lcd.print(btnCountC);

      // Display arrows pointing the appropriate direction
      // (solid if the motor is running, chevrons if not).
      lcd.gotoXY(0, 1);
      if (leftSpeed == 0)
      {
        lcd.print((leftDir > 0) ? '\0' : '\1');
      }
      else
      {
        lcd.print((leftDir > 0) ? '\2' : '\3');
      }
      lcd.gotoXY(7, 1);
      if (rightSpeed == 0)
      {
        lcd.print((rightDir > 0) ? '\0' : '\1');
      }
      else
      {
        lcd.print((rightDir > 0) ? '\2' : '\3');
      }
    }
  }
  motors.setSpeeds(0, 0);
}


// Motor demo with instructions.
void motorDemo()
{
  motorDemoHelper(false);
}

// Motor demo with encoder counts.
void encoderDemo()
{
  motorDemoHelper(true);
}

/*
T|  4|--c-c-c-c----------c---c-
A|  3|-----------G---A-----A---
B|  
N|  4|--c---c---D-D-d-D-d--d-c---------
A|  3|----A---A----------------A-G-A-g-
B|  
B|  4|--c---c---D-D-d-D-d--d-c---c-D-f-
E|  3|----A---A----------------A-------
R|  
--c-c-c-c--<G---<A---c-<A-c---c-<A-c-<A-D-D-d-D-d--d-c-<A-<G-<A-<g---c-<A-c-<A-D-D-d-D-d--d-c-<A-c-D-f-
*/
const char fugue[] PROGMEM =
  "! V10T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugueTitle[] PROGMEM =
  "       Fugue in D Minor - by J.S. Bach       ";

// Play a song on the buzzer and display its title.
void musicDemo()
{
  displayBackArrow();

  uint8_t fugueTitlePos = 0;
  uint16_t lastShiftTime = millis() - 2000;

  while (jb.monitor() != 'B')
  {
    // Shift the song title to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      lcd.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = pgm_read_byte(fugueTitle + fugueTitlePos + i);
        lcd.print(c);
      }
      fugueTitlePos++;

      if (fugueTitlePos + 8 >= strlen(fugueTitle))
      {
        fugueTitlePos = 0;
      }
    }

    if (!jb.buzzer.isPlaying())
    {
      jb.buzzer.playFromProgramSpace(fugue);
    }
  }
}

// Display the the battery (VIN) voltage and indicate whether USB
// power is detected.
void powerDemo()
{
  displayBackArrow();

  uint16_t lastDisplayTime = millis() - 2000;
  char buf[6];

  while (jb.monitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      bool usbPower = usbPowerPresent();

      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      lcd.gotoXY(0, 0);
      sprintf(buf, "%5d", batteryLevel);
      lcd.print(buf);
      lcd.print(F(" mV"));
      lcd.gotoXY(3, 1);
      lcd.print(F("USB="));
      lcd.print(usbPower ? 'Y' : 'N');
    }
  }
}

JarMenuItem mainMenuItems[] = {
  { "Encoders", encoderDemo },
  { "LEDs", ledDemo },
  { "LineSens", lineSensorDemo },
  { "ProxSens", proxSensorDemo },
  { "Inertial", inertialDemo },
  { "Motors", motorDemo },
  { "Music", musicDemo },
  { "Power", powerDemo },
};
JarMenu mainMenu(mainMenuItems, 8, &lcd);

void setup()
{
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  initInertialSensors();

  loadCustomCharacters();

  // The brownout threshold on the ATmega32U4 is set to 4.3
  // V.  If VCC drops below this, a brownout reset will
  // occur, preventing the AVR from operating out of spec.
  //
  // Note: Brownout resets usually do not happen on the Zumo
  // 32U4 because the voltage regulator goes straight from 5
  // V to 0 V when VIN drops too low.
  //
  // The bootloader is designed so that you can detect
  // brownout resets from your sketch using the following
  // code:
  bool brownout = MCUSR >> BORF & 1;
  MCUSR = 0;

  if (brownout)
  {
    // The board was reset by a brownout reset
    // (VCC dropped below 4.3 V).
    // Play a special sound and display a note to the user.

    jb.buzzer.playFromProgramSpace(beepBrownout);
    lcd.clear();
    lcd.print(F("Brownout"));
    lcd.gotoXY(0, 1);
    lcd.print(F(" reset! "));
    delay(1000);
  }
  else
  {
    jb.buzzer.playFromProgramSpace(beepWelcome);
  }

  lcd.clear();
  lcd.print(F("  Zumo"));
  lcd.gotoXY(2, 1);
  lcd.print(F("32U4"));

  delay(1000);

  // lcd.clear();
  // lcd.print(F("Demo"));
  // lcd.gotoXY(0, 1);
  // lcd.print(F("Program"));
  // delay(1000);

  // lcd.clear();
  // lcd.print(F("Use B to"));
  // lcd.gotoXY(0, 1);
  // lcd.print(F("select."));
  // delay(1000);

  // lcd.clear();
  // lcd.print(F("Press B"));
  // lcd.gotoXY(0, 1);
  // lcd.print(F("-try it!"));

  // while (jb.monitor() != 'B'){}

  // jb.buzzer.playFromProgramSpace(beepThankYou);
  // lcd.clear();
  // lcd.print(F(" Thank"));
  // lcd.gotoXY(0, 1);
  // lcd.print(F("  you!"));
  // delay(1000);
}

// This function prompts the user to choose something from the
// main menu, runs their selection, and then returns.
void mainMenuSelect()
{
  lcd.clear();
  lcd.print(F("  Main"));
  lcd.gotoXY(0, 1);
  lcd.print(F("  Menu"));
  delay(1000);
  mainMenu.select();
}

void loop()
{
  mainMenuSelect();
}
