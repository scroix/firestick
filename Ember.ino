const int redPin =  3; // the ember
const int always_on_Pin =  5; // plug the ember wire into this always on pin to aid in alignment when re-fitting resin cover

const int analog_start = 25;
const int analog_end = 255; 

boolean ember_lock = false; // Is the ember currently available for calls?

int pseudo_energy = 255;

/*
/ setupEmber()
/ Initalise and turn off the main LED, representing an ember.
*/
void setupEmber() {
  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, LOW);
  pinMode(always_on_Pin, OUTPUT);
  digitalWrite(always_on_Pin, HIGH);
}

/*
/ lightEmber()
/ Pass a value to the ember's pin between 0 to 255.
*/
void lightEmber(int intensity) {
  analogWrite(redPin, intensity);
}

/*
/ unlightEmber()
/ Just turn it off.
*/
void emberOff() {
  analogWrite(redPin, 0);
  pseudo_energy = 255;
}

/*
/ emberLightable()
/ Just a check on the boolean state of the ember.
*/
boolean getEmberLock() {
  return ember_lock;
}

/*
/ convertEnergy()
/ Converts energy level to a value within the range of the analog write.
*/
double slope;
double coversion_output = 0;
int convertEnergy(int energy) {
  slope = 1.0 * (analog_end - analog_start) / (energy_emberhigh - energy_emberlow);
  coversion_output = analog_start + slope * (energy - energy_emberlow);
  return (int) coversion_output;
}

void rampEmber() {
   // should the ember be adjusted?
  if ( (energy > energy_levels[4] && energy < energy_levels[5]) && (!getEmberLock()) ) {
    lightEmber(convertEnergy(energy));
  }
  else if ( (energy > energy_levels[6]) && (pseudo_energy > -15)) {
    lightEmber(pseudo_energy);
    pseudo_energy = pseudo_energy - 15;
  }
}

/*
/ flickerEmber()
/ A little flicker effect when it initially lights.
*/
void flickerEmber() {
  // Lock the ember up.
  ember_lock = true;
  // We'll just play around with some magic numbers.
  // Start bright!
  analogWrite(redPin, 35);
  delay(50);
  // Fade down....
  for (int x = 35; x > 10; x-- ) {
    analogWrite(redPin, x);
    delay(5);
  }
  // Unlock light for use in normal pattern.
  ember_lock = false;
}
