#define AUDIO_CNT 8                           // Count of cues.
int audio_pins[] = {24,25,27,29,31,32,33,30}; // Pin outs for audio.
int warning_pin = 26;
const int lockout_period = 3500;              // Time in ms to wait before new cue is allowed.
static unsigned long cue_time = 0;

/*
/ readChopper()
/ Updates the chopper value, used to calculate distance, and hence energy.
*/
void audioCue(int audio_pin)
{
  // We'll only trigger if the lockout period has passed, or it's the first track!
  if ((millis() > cue_time + lockout_period) || (audio_pin == 0)) {
    // Trigger pin
    digitalWrite(audio_pins[audio_pin], LOW);
    delay(150); // The minimum delay to actually trigger the audio.
    digitalWrite(audio_pins[audio_pin], HIGH);
    // Notify debugger
    Serial.print("Playing Track: ");
    Serial.print(audio_pin);
    Serial.println();
    // Stamp time of cue
    cue_time = millis();
  }
  else {
    Serial.print("Skipping Track: "); 
    Serial.print(audio_pin);
    Serial.println();
  }
}

/*
/ setupAudio
/ Let's fire up all our pins in preparation for some sweet juicy audio.
*/
void setupAudio()
{
   for(int i = 0; i < AUDIO_CNT;i++){
      pinMode(audio_pins[i], OUTPUT);
      digitalWrite(audio_pins[i], HIGH);
  } 
  pinMode(warning_pin, OUTPUT);
  digitalWrite(warning_pin, HIGH);
}

void encouragement() 
{
   if ((millis() > cue_time + lockout_period) ) {
      digitalWrite(warning_pin, LOW);
      delay(50); 
      digitalWrite(warning_pin, HIGH);
      cue_time = millis();
   }
   nonono_playable = false;
}

