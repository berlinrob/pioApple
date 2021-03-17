#ifndef cf_lib_OLED128x64
#define cf_lib_OLED128x64

// #include <Wire.h>
// #include <Adafruit_SSD1306.h>

class cf_oled128x64{
    public:
        // constructor
        cf_oled128x64();

        // attributes
        int screenWidth = 128;
        int screenHeight = 64;
        int oled_reset = 4;
        unsigned char screen_address = 0x3C;    ///< See datasheet for Address
        //Adafruit_SSD1306 display(int sw, int sh, int myWire, int reset);


};

#endif