#ifndef cf_lib_JoyStick
#define cf_lib_JoyStick

class cf_joystick
{
public:
    // constructor
    cf_joystick(int pinX, int pinY, int pinSW);

    // properties
    int pinX;
    int pinY;
    int pinSW;

    int mapX;
    int mapY;

    int prev_mapX;
    int prev_mapY;

    int analogX;
    int analogY;

    // methods
    void setup();
    void loop();
    int getAnalogX();
    int getAnalogY();
    
    void setMapX(int);
    void setMapY(int);

private:
};

#endif