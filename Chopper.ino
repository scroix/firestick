#include <Encoder.h>
 
Encoder chopper(7, 8);            // Change these pin numbers to the pins connected to your encoder.       
                                                         
const int chopper_limit = 100;    // Spin range +- This is a good config variable.
int chopper_val;

/*
/ readChopper()
/ Updates the chopper value, used to calculate distance, and hence energy.
*/
void readChopper()
{
  chopper_val = chopper.read();
  // Chopper hasn't been spun too far in one direction or the other.
  if ( (chopper_val < chopper_limit) && (chopper_val > chopper_limit * -1) ) {
    new_position = chopper_val;
  }
  // Chopper exceeded limit, so we'll reset it to the upper bound.
  else if (chopper_val > chopper_limit) {
    chopper.write(chopper_limit); 
  }
  // Chopper exceeded limit, so we'll reset to the lower bound.
  else if (chopper_val < chopper_limit * -1) {
    chopper.write(chopper_limit * -1);
  }
}

/*
/ resetChopper()
/ Just a nice clean reset.
*/
void resetChopper()
{
  chopper.write(0);
}

/*
/ simulateChopper()
/ We're just going to pretend somebody is spinning the chopper!.. Erratically.
*/
void simulateChopper()
{
  new_position = random(chopper_limit * wind_rate); // It turns out this is pretty quick.
}
