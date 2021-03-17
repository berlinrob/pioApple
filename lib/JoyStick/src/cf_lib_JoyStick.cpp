#include "cf_lib_JoyStick.h"
#include <stdio.h>
#include <Arduino.h>

cf_joystick::cf_joystick(int X, int Y, int SW){
    // things to do on instantination
    pinX = X;
    pinY = Y;
    pinSW = SW;
}

void cf_joystick::begin()
{
    //printf("inside JoyStick class ::begin");
}

void cf_joystick::loop()
{
    //printf("inside JoyStick class ::loop");
}

int cf_joystick::getAnalogX() //0-1023 for 10 bits or 0-4095 for 12 bits). Data type: int.
{
    return analogRead(cf_joystick::pinX);
}

int cf_joystick::getAnalogY()
{
    return analogRead(cf_joystick::pinY);
}