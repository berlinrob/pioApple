#ifndef cf_lib_JoyStick
#define cf_lib_JoyStick

class cf_joystick{
    public:
        // constructor
        cf_joystick(int pinX, int pinY, int pinSW);

        // properties
        int pinX;
        int pinY;
        int pinSW;

        int prevX;
        int prevY;
        int prevSW;

        int analogX;
        int analogY;

        // methods
        void begin();
        void loop();
        int getAnalogX();
        int getAnalogY();

    private:


};

#endif