/*
  BopIt
 
Play BopIt the way it was meant to be.. with a decapitated arm of course!
 Pins:
 RESET- Start Button
 2 - Stroke Sensor
 3 - Stroke LED
 4 - Finger Squeeze Sensor
 5 - Finger Squeeze LED
 6 - Finger Pull Sensor
 7 - Finger Pull LED
 8 - Bone Twist Sensor
 9 - Bone Twist LED
 A0 - Photosensor
 10 - Photosensor/Sniff LED
 11 - Scab Pick Sensor
 12 - Scab Pick LED
 13 - Speaker
 A3 - Round Counter LED
 
 created 10 Jul 2012
 by Jason Schapiro and Sarah Schoemann

 */
 #include "pitches.h"


const int initial_time = 7000;
int fail_count;
int game_time;
int round_number;
int round_start;
int curr_move;
bool round_in_progress;

// stroke checker
int stroke_high;
int stroke_low;

// pins
const int round_led = 17; // A3
const int speaker = 13;
/* sensors (in order):
* stroke sensor
* finger squeeze
* finger pull
* bone twist
* photosensor sniff
* scab pick
*/

// sensor, LED
const int sensor[6][2] = 
{
{ 2, 3 }, // stroke sensor
{ 4, 5 }, // finger squeeze
{ 6, 7 }, // finger pull
{ 8, 9 }, // bone twist
{ A0, 10 }, // photosensor sniff
{ 11, 12 } // scab pick
};

void setup() {
  // set pins
  pinMode(round_led, OUTPUT);  
  pinMode(speaker, OUTPUT);    
  for (int i=0; i<6; i++){
    pinMode(sensor[i][0], INPUT);  
    pinMode(sensor[i][1], OUTPUT); 
    // turn on LED for beginning sequence
    digitalWrite(sensor[i][1], HIGH);   	    
  } 
  digitalWrite(round_led, HIGH);   	
    
  // set game time & round
  game_time = initial_time;
  round_number = 1;
  round_in_progress = false;
  fail_count = 0;
  
  // initialize stroke checkers
  stroke_high = 0;
  stroke_low = 0;
    
   // PLAY ADDAMS FAMILY 
  const int melody[] = {
  NOTE_G3, NOTE_C4, NOTE_E4, NOTE_C4,0, NOTE_C3, 0, NOTE_C3,0,
  NOTE_A3, NOTE_F4, NOTE_D4, NOTE_F4,0, NOTE_C3, 0, NOTE_C3,0, 
  NOTE_A3, NOTE_F4, NOTE_D4, NOTE_F4, 0,
  NOTE_A3, NOTE_F4, NOTE_D4, NOTE_F4, 0,
  NOTE_G3, NOTE_C4, NOTE_E4, NOTE_C4, 0,
  0, NOTE_C3, 0, NOTE_C3};
    
  const int noteDurations[] = {
  8, 8, 8, 8, 4, 4, 8, 8, 4, 
  8, 8, 8, 8, 4, 4, 8, 8, 4,
  8, 8, 8, 8, 8,
  8, 8, 8, 8, 8,
  8, 8, 8, 8, 8,
  8, 4, 8, 4};
  
  // plays starting theme song
  for (int thisNote = 0; thisNote < 37; thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speaker, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speaker);
  }
  
  // turn off LEDs after start
    for (int i=0; i<6; i++){
    digitalWrite(sensor[i][1], LOW);   	
  }
  digitalWrite(round_led, LOW);  
   	  
  // create random seed
  Serial.begin(9600);
  randomSeed(analogRead(1));
}

void loop() {
  if (!round_in_progress){
    round_start = millis();
    round_in_progress = true; 
    curr_move = pickMove();
  } else {
    if (checkRound()){
      // you fail, start endgame 
      Serial.println("Round fail!");
      failure();
      fail_count++;
      if (fail_count == 3){
      	game_over();
      	while(1) { }
      }
    }
    else if (checkMove()){
      // you move on...this time.
      Serial.println("Successful move");
      // turn off action's LED
      digitalWrite(sensor[curr_move][1], LOW); 
      success();
      round_number++;
      if (game_time > 3000){
         game_time -= 300; 
         Serial.print("Reduced game time by 0.3 second. Time:");
         Serial.println(game_time);
      }
      else if (game_time > 1500){
         game_time -= 100; 
         Serial.print("Reduced game time by .1 seconds. Time:");
         Serial.println(game_time);
      }
      round_in_progress = false;
    }
  }
}

// randomized action chooser
int pickMove(){
  int move = random(6);
  digitalWrite(sensor[move][1], HIGH);   // turn the LED on for the chosen action 
  Serial.print("Move: ");
  Serial.println(move);
  return move;
}

// returns true if round over
bool checkRound(){
   int time = millis();
   if ((time - round_start) > game_time){
     return true; 
   }
   else return false;
}

// sees if action done
/* sensors:
* 0 - stroke sensor - button (on when stroked)
* 1 - finger squeeze - button (off when pulled)
* 2 - finger pull - button (off when pulled)
* 3 - bone twist - button (on when twisted)
* 4 - photosensor sniff (photo low when covered)
* 5 - scab pick - button (on when twisted)
*/
bool checkMove(){
  switch(curr_move) {
  	case 0:
  		if (digitalRead(sensor[0][0]) == HIGH){
  			Serial.println("Stroke high");
  			stroke_high++;
  		}
  		else if (digitalRead(sensor[0][0]) == LOW){
  			Serial.println("Stroke low");  		
  			stroke_low++;
  		}
  		if (stroke_high > 5 && stroke_low > 5){
				// reset stroke checkers
				stroke_high = 0;
				stroke_low = 0;  			
  			return true;
  		}
  		break;
  	case 1: 
  		if (digitalRead(sensor[1][0]) == LOW){
      			stroke_low++;
  			return true;
  		}  
  		else if (digitalRead(sensor[1][0]) == HIGH){
  			Serial.println("Stroke low");  		
  			stroke_high++;
  		}
  		if (stroke_high > 1 && stroke_low > 1){
				// reset stroke checkers
				stroke_high = 0;
				stroke_low = 0;  			
  			return true;
  		}  	
  		break; 	
  	case 2:
  		if (digitalRead(sensor[2][0]) == LOW){
      			stroke_low++;
  			return true;
  		}  
  		else if (digitalRead(sensor[2][0]) == HIGH){
  			Serial.println("Stroke low");  		
  			stroke_high++;
  		}
  		if (stroke_high > 1 && stroke_low > 1){
				// reset stroke checkers
				stroke_high = 0;
				stroke_low = 0;  			
  			return true;
  		}  	
  		break; 	
  	case 3:
  		if (digitalRead(sensor[3][0]) == LOW){
      			stroke_low++;
  			return true;
  		}  
  		else if (digitalRead(sensor[3][0]) == HIGH){
  			Serial.println("Stroke low");  		
  			stroke_high++;
  		}
  		if (stroke_high > 1 && stroke_low > 1){
				// reset stroke checkers
				stroke_high = 0;
				stroke_low = 0;  			
  			return true;
  		}  	
  		break; 	
  	case 4:
  		if (analogRead(sensor[4][0]) < 50){
  			return true;
  		}  	  	
  		break;
  	case 5: 
  		if (digitalRead(sensor[5][0]) == LOW){
      			stroke_low++;
  			return true;
  		}  
  		else if (digitalRead(sensor[5][0]) == HIGH){
  			Serial.println("Stroke low");  		
  			stroke_high++;
  		}
  		if (stroke_high > 1 && stroke_low > 1){
				// reset stroke checkers
				stroke_high = 0;
				stroke_low = 0;  			
  			return true;
  		}  	
  		break; 	
  	default:
  		return false; 	
  }
  return false;
}

void success(){
 int mel[] = {
  NOTE_B3, NOTE_C4};

 int noteDur[] = {
  4,4 };

 for (int thisNote = 0; thisNote < 2; thisNote++) {
    int noteDuration = 1000/noteDur[thisNote];
    tone(speaker, mel[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speaker);
 } 
}

void failure(){
 int mel[] = {
  NOTE_C4, 0, NOTE_B4};

 int noteDur[] = {
  4,8,2 };

 for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000/noteDur[thisNote];
    tone(speaker, mel[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speaker);
 } 
}

void game_over() { 
 for (int i = 0; i< round_number; i++){
   digitalWrite(round_led, HIGH);   // turn the LED on (HIGH is the voltage level)
   delay(500);               // wait for a second
   digitalWrite(round_led, LOW);    // turn the LED off by making the voltage LOW
   delay(500);
 }  
}
