/**********************
* Firestick Interactive
*
* DESC: 
* A great game for kids of all ages.
* 
* NOTES:
* - The code is divided up into different sketches which represent
* components of the Firestick Interactive.
* - There's one model of energy used throughout the experience
* which unlocks 'stages' and triggers associated events.
* - To tweak the experience, you should first look at modifying
* the 'difficulty' variable, the difference between energy stages in
* the 'energy_levels' array, or the 'cooling_amount'.
*
* AUTHOR:
* Michael Borthwick
* Julien de-Sainte-Croix
* Robert Laine-Wyllie
* And all the wonderful coders who built libraries for us.
*
* CHANGES:
* DATE        COMMENT
* 12.03.2018  Added a debugging parameter to skip the encoder stage.
* 29.12.2017  Finishing condition no longer requires period of inactivity.
* 01.01.2045  Singularity renders code redundant.
*
*/

const boolean skippingChopper = true; 	// Start with boosted energy skipping the encoder phase
const int skippingSpeed = 4; 			// Energy increases each 'X'th of a second
const boolean debugging = true; 		// Change to false to disable debugging

/*
/ Configuration variables.
/ You'll want to play with these to get the experience running correctly on different hardware.
*/
#define difficulty 5         // An overall difficulty modifier, which adjusts range for the energy level thresholds.
#define wind_rate 3          // The speed at which energy is generated from blowing. Higher makes blowing easier
#define inactivity_limit 10  // Reset the experience (and values) after x seconds.

int cooling_amount = 150;    // The exact energy rate at which energy decays each second.
int energy_levels[] = { 5 * difficulty,        // Okay, start spinning!
                        250 * difficulty,      // That's it, keep it up...
                        1000 * difficulty,      // You're doing well, keep spinning.
                        2000 * difficulty,      // Faster now, don't stop.                        
                        3000 * difficulty,     // EMBER LOW: You've got an ember.
                        4000 * difficulty,     // EMBER HIGH: Blow harder.
                        5000 * difficulty,     // FIRE LOW: Keep going.
                        6250 * difficulty };  // FIRE HIGH: You've got it.

// Some QOL defines.
#define energy_emberlow energy_levels[4]
#define energy_emberhigh energy_levels[5]
#define energy_firelow energy_levels[6]
#define energy_firehigh energy_levels[7]

int position  = 0;
int distance = 0;
int energy = 0;

int new_position;                
int new_distance;
int new_energy;

int last_second_energy = 0;

static unsigned long ts = 0;        // Our timer in ms.
#define sample_interval 1000        // Frequency to test the energy level in ms.

unsigned long inactivity_count = 0; // How many seconds of accumulative decay?
boolean recently_reset = false;     // Prevent cascading resets.

boolean nonono_playable = true;     // Can we play the audio warning?

/*
/ These are used for keeping track of our audio queues, to prevent duplicate plays.
/ It also doubles up as a "stage checker" as we progress in energy level these booleans invert.
/ They're also used to determine whether certain events should be introduced, like examining the "wind" value.
*/
const int stage_count = 8;
boolean stage[stage_count] = {};
// stage[0] welcome/teaser stage
// ** firestick
// stage[1]
// stage[2]
// stage[3]
// ** wind & ember
#define state_emberlow stage[4]
#define state_emberhigh stage[5]
// ** wind & flame
#define state_firelow stage[6]
#define state_firehigh stage[7]

/*
/ setup
/ Just some housekeeping.
*/
void setup()
{
  Serial.begin(9600);
  
  setupAudio();          // Pins written for audio.
  setupEmber();   
  cleanLEDs();           // Initialisation of the NeoPixel controlled lights.
  
  // There's 8 stages in these experience, listed above.
  // We'll start by declaringly them all valid.
  // And tick them off as the energy increases.
  for (int x = 0; x < stage_count; x++ ) {
   stage[x] = true; 
  }
}

/*
/ loop
/ The main Arduino loop.
/ We'll do a number of things here...
/ 1. Update stages, which has it's own consequences (see stageUpdate())
/ 2. Update energy levels.
/ 3. Monitor activity.
*/
void loop()
{    
  new_position = 0;
  new_distance = 0;
  new_energy = 0;
  
  if (skippingChopper == true) {
  	skipChopperPhase();
  }
  stageUpdate();
  
  // We either update energy according to wind...
  if(!stage[4]) {
      burn();
      measureWind();
      if (windBlowing()) {
        simulateChopper();
      }
      rampEmber();      
  }
  else {
   // ...or we update according to the chopper (stick).
   readChopper(); 
  }
 
  // If there's a change in the position (wind or chopper) we'll update energy.
  if ( (new_position != position) ) {
    energyUpdate();
    inactivity_count = 0;  // reset decay, could add an 'if value not greater' than to this
  }
 
  // Every 1000ms we'll decay the energy levels.
  if (millis() > ts + sample_interval) {
 
    if (energy >= cooling_amount) {
       energyDecay();
    }
    // We'll update the decay, and if we've had X seconds of inactivity, reset the experience.  
    if ((inactivity_count > inactivity_limit) && (!recently_reset)) {
      reset(); 
    }
    else if (!stage[7])
    {
      rampDownFire();
      reset(); 
    }
    
    inactivity_count++;
    ts = millis();
  }
}

/*
/ stageUpdate
/ As the energy levels increase, we'll unlock various "stages" each
/ with their own actions, and audio queues.
/ Once a stage has been unlocked, a boolean is flipped until a global reset occurs.
*/
void stageUpdate(){
  // We trigger the final audio queue, and set the fire to high.
  if(energy > energy_levels[7] && stage[7]){
    audioCue(7);  // "You've got it!"
    stage[7] = false;
    maxIntensity();  // Flame is high intensity.
  }
  // The flame appears at this point, ember has reached maximum brightness.
  else if(energy > energy_levels[6] && stage[6]){
    audioCue(6);  // "Keep going, hands getting hot!"
    stage[6] = false;
    intensity(60); // Flame is low intensity, but on for the first time.
  }
  // The ember will ramp up to maximum brightness during this stage.
  else if(energy > energy_levels[5] && stage[5]){
    audioCue(5); // "Now blow harder!"
    stage[5] = false;
  }
  // We swap to wind energy generation at this point.
  // The ember also lights, does a little trick,
  // and then updates according to energy.
  else if(energy > energy_levels[4] && stage[4]) {
    audioCue(4); // "You've got an ember
    stage[4] = false; 
    nonono_playable = false;
    intensity(0);
    digitalWrite(3,HIGH); // Ember LED
    flickerEmber(); // A 1-2 brightness adjustment.
  }
  // The final "stick" related audio queue before we swap to wind.
  else if(energy > energy_levels[3] && stage[3]) {
    audioCue(3);
    sampleWind();
    stage[3] = false;
    nonono_playable = false; // We don't want to risk getting a nonono that interrupts the prompt to blow.
//    cooling_amount = difficulty * cooling_amount;
  }
  else if(energy > energy_levels[2] && stage[2]) {
    audioCue(2);
    sampleWind();
    stage[2] = false;
    nonono_playable = true;
//    cooling_amount = difficulty * cooling_amount;    
  }
  else if(energy > energy_levels[1] && stage[1]) {
    audioCue(1);
    sampleWind();
    stage[1] = false;
    nonono_playable = true;
//    cooling_amount = difficulty * cooling_amount;
  }
  // This triggers on basically anybody touching the stick.
  // It's a sort of motion detector to encourage use.
  else if(energy > energy_levels[0] && stage[0]) {
    audioCue(0);
    sampleWind();
    stage[0] = false;
    recently_reset = false;  
  }
}

/*
/ skipChopperPhase
/ A short debugging function to skip the early phases.
*/
void skipChopperPhase()
{
	if (millis() > ts + (sample_interval / skippingSpeed)) {
		if (energy < energy_levels[4]) {
			energy = energy + cooling_amount;
		}
	}
}

/*
/ reset
/ A cleanup for the next user
*/
void reset()
{
  recently_reset = true;
  
  // Reset time.
  ts = 0;
  
  // Reset out decay counter, the cause for this kerfuffle.
  inactivity_count = 0;
  
  // Zero out our variables for calculations.
  position = 0;
  distance = 0;
  energy = 0;
  
  // Zero out our chopper.
  resetChopper();
  
  // Turn off the big red LED.
  emberOff();

  // Reduce the brightness of all of our dancing LEDs.
  cleanLEDs();
  
  // Reset wind
  voidWind();
  
  // Reset all the booleans in our "state machine"
  for (int x = 0; x < stage_count; x++ ) {
   stage[x] = true; 
  }
  
  // Reprimand enabled.
  nonono_playable = true;
  
  // Reminder of existential threat.
  Serial.println("There is no spoon..."); 
}
