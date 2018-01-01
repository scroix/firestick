
/*
/ energyUpdate
/ The energy increases here based on the distance since last measured.
*/
void energyUpdate()
{
  // distance and energy updated based on position difference
  new_distance = abs(position - new_position);
  new_energy = energy + new_distance;
 
  // update our measurements for a comparison next round
  position = new_position;
  distance = distance + new_distance;
  energy = new_energy;
}

/*
/ energyDecay
/ A static energy decay that is called from the main loop.
*/
void energyDecay()
{   
    // is it less than it was since we last decayed?
    if(debugging){
      if ((energy + new_distance) < ((energy + new_distance) - cooling_amount * 2)) {
        Serial.print("Too Slow!");
      }
    }
    
    if (!outrunDecay() && energy > energy_levels[0]) {
    if(nonono_playable) { 
       if(debugging){
          Serial.println("Playing Encouragement.");
       }   
       encouragement();
    }
  }  
  last_second_energy = energy;
  
  // energy level decay
  new_energy = energy + new_distance - cooling_amount;
 
  // update our measurements for a comparison next round
  energy = new_energy;
  
  if (!stage[4]) {
    if(debugging) { Serial.println("Ember decaying"); }
    rampEmber();
  }
  
  if(debugging && stage[7]) {
      Serial.print("Energy = ");
      Serial.print(energy);
      Serial.println();      
   }  
}

/*
/ outrunDecay
/ Check if we're generating energy faster than the cooling amount.
*/
boolean outrunDecay() {
 if ((energy - last_second_energy) > cooling_amount) {
   return true;
 }  
 return false; 
}


