    /*
     Demonstration sketch for Adafruit LCD backpack
     using MCP23008 I2C expander and Maxbotic LV-EZ1 Ultrasonic Sensor
       (other pin compatible Maxbotix sensors should also work)
     Tested with the 5 volt Trinket mini microcontroller at 8 MHz
     The ultrasonic sensor and pin use should be Gemma and Trinket 3V compatible
     
     This sketch reads the LV-EZ1 by pulse count and prints the distance to the LCD
     
     The circuit:
     * 5V to Arduino 5V pin, I2C Backpack 5V and EZ1 +5
     * GND to Arduino GND pin, I2C Backpack GND and EZ1 GND
     * Display I2C Backpack CLK to Trinket GPIO #2 
     * Display I2C backpack DAT to Trinket GPIO #0 
     * LV-EZ1 Ultrasonic Sensor PW pin to Trinket GPIO #1 
     
       Portions of code provided free use on http://playground.arduino.cc/Main/MaxSonar
       by Bruce Allen and Bill Gentles
     
     Version 2.0 Adds Arduino IDE 1.6.7 and greater Wire support 
                 Mike Barela for Adafruit Industries
    */
     
    // include the library code
    #include <Wire.h>
    #include "Adafruit_DRV2605.h"
     
    #define EZ1pin 1               // Trinket GPIO #1   

    Adafruit_DRV2605 drv;
     
    // These values are for calculating a mathematical median for a number of samples as
    // suggested by Maxbotix instead of a mathematical average
    int8_t arraysize = 9; // quantity of values to find the median (sample size). Needs to be an odd number
    //declare an array to store the samples. not necessary to zero the array values here, it just makes the code clearer
    uint16_t rangevalue[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t modE;        // calculated median distance
     
    void setup() {
      pinMode(EZ1pin, INPUT); // Sey ultrasonic sensor pin as input
      drv.begin();
      drv.selectLibrary(1);
      // I2C trigger by sending 'go' command 
      // default, internal trigger when sending GO command
      drv.setMode(DRV2605_MODE_INTTRIG); 
    }

     
    void loop()
    {
      int16_t pulse;  // number of pulses from sensor
      int i=0;
      
      while( i < arraysize )
      {                    
        pulse = pulseIn(EZ1pin, HIGH);  // read in time for pin to transition
        rangevalue[i]=pulse/58;         // pulses to centimeters (use 147 for inches)
        if( rangevalue[i] < 645 && rangevalue[i] >= 15 ) i++;  // ensure no values out of range
        delay(10);                      // wait between samples
      }
      isort(rangevalue,arraysize);        // sort samples
      modE = mode(rangevalue,arraysize);  // get median

      Hfeedback(modE);

      delay(10);  
    }
     
    // Sorting function (Author: Bill Gentles, Nov. 12, 2010)
    void isort(uint16_t *a, int8_t n){
      for (int i = 1; i < n; ++i)  {
        uint16_t j = a[i];
        int k;
        for (k = i - 1; (k >= 0) && (j < a[k]); k--) {
          a[k + 1] = a[k];
        }
        a[k + 1] = j;
      }
    }


    void Hfeedback(uint16_t distance)
    {
      uint16_t myint;
      uint16_t set_size = 20; //magic number
      myint =  -(float)distance/60 + (set_size+1);

      //uint16_t index = 0;
      
        for (int i=0; i <= myint; i++)
        {
          drv.setWaveform(0, 24);  // play effect 
          
        } 
              // set the effect to play

        
        drv.setWaveform(1, 0);       // end waveform
      
        // play the effect!
        drv.go();
    }
     
    // Mode function, returning the mode or median.
    uint16_t mode(uint16_t *x,int n){
      int i = 0;
      int count = 0;
      int maxCount = 0;
      uint16_t mode = 0;
      int bimodal;
      int prevCount = 0;
      while(i<(n-1)){
        prevCount=count;
        count=0;
        while( x[i]==x[i+1] ) {
          count++;
          i++;
        }
        if( count > prevCount & count > maxCount) {
          mode=x[i];
          maxCount=count;
          bimodal=0;
        }
        if( count == 0 ) {
          i++;
        }
        if( count == maxCount ) {      //If the dataset has 2 or more modes.
          bimodal=1;
        }
        if( mode==0 || bimodal==1 ) {  // Return the median if there is no mode.
          mode=x[(n/2)];
        }
        return mode;
      }
    }
