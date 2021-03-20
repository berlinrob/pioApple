#include "cf_lib_JoyStick.h"
#include <stdio.h>
#include <Arduino.h>

cf_joystick::cf_joystick(int X, int Y, int SW){
    // things to do on instantination
    pinX = X;
    pinY = Y;
    pinSW = SW;
}

void cf_joystick::setup()
{
    //printf("inside JoyStick class ::begin");
}

void cf_joystick::loop()
{
    //printf("inside JoyStick class ::loop");
}

int cf_joystick::getAnalogX() //0-1023 for 10 bits or 0-4095 for 12 bits). Data type: int.
{
    prev_mapX = mapX;
    analogX = analogRead(cf_joystick::pinX);

    return analogX;
}

int cf_joystick::getAnalogY()
{
    prev_mapY = mapY;
    analogY = analogRead(cf_joystick::pinY);
    return analogY;
}

void cf_joystick::setMapX(int p_mapX)
{
    cf_joystick::mapX = p_mapX;
}

void cf_joystick::setMapY(int p_mapY)
{
    cf_joystick::mapY = p_mapY;
}