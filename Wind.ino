/* Modern Device Wind Sensor Sketch for Rev C Wind Sensor
 This sketch is only valid if the wind sensor if powered from 
 a regulated 5 volt supply. An Arduino or Modern Device BBB, RBBB
 powered from an external power supply should work fine. Powering from
 USB will also work but will be slightly less accurate in our experience.
 
When using an Arduino to power the sensor, an external power supply is better. Most Arduinos have a 
 polyfuse which protects the USB line. This fuse has enough resistance to reduce the voltage
 available to around 4.7 to 4.85 volts, depending on the current draw. 
 
 The sketch uses the on-chip temperature sensing thermistor to compensate the sensor
 for changes in ambient temperature. Because the thermistor is just configured as a
 voltage divider, the voltage will change with supply voltage. This is why the 
 sketch depends upon a regulated five volt supply.
 
 Other calibrations could be developed for different sensor supply voltages, but would require
 gathering data for those alternate voltages, or compensating the ratio.
 
 Hardware Setup: 
 Wind Sensor Signals    Arduino
 GND                    GND
 +V                     5V
 RV                     A1    // modify the definitions below to use other pins
 TMP                    A0    // modify the definitions below to use other pins
 
 Paul Badger 2014
 
 Hardware setup:
 Wind Sensor is powered from a regulated five volt source.
 RV pin and TMP pin are connected to analog innputs.
 
 */

#define analogPinForRV    A2   // change to pins you the analog pins are using
#define analogPinForTMP   A3

// to calibrate your sensor, put a glass over it, but the sensor should not be
// touching the desktop surface however.
// adjust the zeroWindAdjustment until your sensor reads about zero with the glass over it. 

const float zeroWindAdjustment =  -.6; // negative numbers yield smaller wind speeds and vice versa.

int TMP_Therm_ADunits;  //temp termistor value from wind sensor
float RV_Wind_ADunits;    //RV output from wind sensor 
float RV_Wind_Volts;
unsigned long lastMillis;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;


/*
/ measureWind
/ Use STEM to calculate the Wind Speed.
*/
void measureWind() {
  if (millis() - lastMillis > 200) {      // read every 200 ms - printing slows this down further
  
    RV_Wind_Volts = (analogRead(analogPinForRV) *  0.0048828125);
    
    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors
    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.
      
    // Uncomment for debugging.
    if(debugging && stage[7]) {
      Serial.print("Wind V: ");
      Serial.print((float)RV_Wind_Volts);
      Serial.print("vs. Sampled AVG: ");
      Serial.println(windAverage());
    }
    lastMillis = millis(); 
    
  } 

}

/*
  Sample the RV for windspeed to baseline off of an average.
*/
float rv_total = 0;
float rv_avg = 0;
float rv_sample_count = 0;
void sampleWind() {
    measureWind();
    rv_total += RV_Wind_Volts;
    rv_sample_count++;
}

float windAverage() {
  rv_avg =  rv_total / rv_sample_count;
  return rv_avg;
}

boolean windBlowing() {
  if (RV_Wind_Volts > 1.02 * windAverage() ) {
    return true;
  }  
  return false;
}

// Set average and counter to 0
void voidWind() {
 rv_avg = 0;
 rv_sample_count = 0;
 rv_total = 0; 
}
