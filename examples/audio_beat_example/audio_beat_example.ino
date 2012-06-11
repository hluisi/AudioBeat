#include <PWM.h>
#include <AudioBeat.h>
#include <SoftwareSerial.h>



/* CONTROL object defined in my PWM libary */
CONTROL chan;

/* AudioBeat object */
AudioBeat treble, bass;

/* LCD Object */
SoftwareSerial lcd = SoftwareSerial(0,2); 

//unsigned long HIGH_MAX = 0;
byte thisPWM = 0, debug_check = 0;
boolean spike = false;

const int debug_rate = 50;
const byte BASS_LED = 13;

void init_lcd() {
  lcd.begin(9600);  
  // set the size of the display to 16x2
  lcd.write(0xFE);
  lcd.write(0xD1);
  lcd.write(16);  // 16 columns
  lcd.write(2);   // 2 rows
  delay(10);
  
  // set the contrast to 200
  lcd.write(0xFE);
  lcd.write(0x50);
  lcd.write((uint8_t)200);
  delay(10); 
  
  // set the brightness to 255
  lcd.write(0xFE);
  lcd.write(0x99);
  lcd.write((uint8_t)100);
  delay(10);
  
  // turn off cursors
  lcd.write(0xFE);
  lcd.write(0x4B);
  lcd.write(0xFE);
  lcd.write(0x54);
  delay(10);
  
  // clear screen
  lcd.write(0xFE);
  lcd.write(0x58);
  delay(10);
  
  // go 'home'
  lcd.write(0xFE);
  lcd.write(0x48);
  delay(10);
}

void reset_lcd() {
  // clear screen
  lcd.write(0xFE);
  lcd.write(0x58);
  // go 'home'
  lcd.write(0xFE);
  lcd.write(0x48);
}

/* SETUP */
void setup() {
  //treble.setCorrectionLimit(250);
  //treble.setHighNoiseLimit(300);
  //treble.setCorrectionLimit(500);
  
  treble.setPIN(A0);
  bass.setPIN(A1);
  
  /* Set the max value of 255 for all PWM channels */
  chan.setMax_all(255);
  
  pinMode(BASS_LED, OUTPUT);
  
  /* set the random seed */
  randomSeed(analogRead(0));
  
  /* open a serial connection (for debugging) */
  //Serial.begin(57600);
  
  init_lcd();
}

void loop() {
  /* start counters */
  debug_check++;
  
  /* is it time to print debug info? */
  if ( debug_check >= debug_rate ) {
    byte blvl, tlvl;
    
    blvl = constrain(map(bass.LEVEL, 0, 511, 1, 16),1,16);
    tlvl = constrain(map(treble.LEVEL, 0, 511, 1, 15),1,15);
    
    
    // reset the lcd 
    reset_lcd();
    lcd.print("B ");
    lcd.print("L:");
    if ( bass.LEVEL < 10 ) {
      lcd.print("00");
    } else if ( bass.LEVEL < 100 ) {
      lcd.print("0");
    }
    lcd.print(bass.LEVEL);
    
    lcd.print(" S:");
    if (bass.SAMPLE < 10) {
      lcd.print("000");
    } else if (bass.SAMPLE < 100) {
      lcd.print("00");
    } else if (bass.SAMPLE < 1000) {
      lcd.print("0");
    }
    lcd.println(bass.SAMPLE);
    
    lcd.print("T ");
    lcd.print("L:");
    
    if ( treble.LEVEL < 10 ) {
      lcd.print("00");
    } else if ( treble.LEVEL < 100 ) {
      lcd.print("0");
    }
    lcd.print(treble.LEVEL);
    
    lcd.print(" S:");
    if (treble.SAMPLE < 10) {
      lcd.print("000");
    } else if (treble.SAMPLE < 100) {
      lcd.print("00");
    } else if (treble.SAMPLE < 1000) {
      lcd.print("0");
    }
    lcd.print(treble.SAMPLE);
    
    
    /*
    // Bass
    Serial.print("R: ");
    Serial.print(treble.RANGE);
    Serial.print("  MIN: ");
    Serial.print(treble.MIN);
    Serial.print("  MAX: ");
    Serial.print(treble.MAX);
    Serial.print("  L: ");
    Serial.print(treble.LEVEL );
    Serial.print("  P: ");
    Serial.print(thisPWM);
    Serial.print("  S: ");
    Serial.print(bass.SAMPLE);
    //Serial.print("   N: ");
    //Serial.print(treble.LOW_NOISE_COUNT);
    //Serial.print(",");
    //Serial.print(treble.HIGH_NOISE_COUNT);
    //
    
    
    // Treble
    Serial.print("R: ");
    Serial.print(treble.RANGE);
    Serial.print("  MIN: ");
    Serial.print(treble.MIN);
    Serial.print("  MAX: ");
    Serial.print(treble.MAX);
    Serial.print("  L: ");
    Serial.print(treble.LEVEL );
    Serial.print("  P: ");
    Serial.print(thisPWM);
    Serial.print("  S: ");
    Serial.print(treble.SAMPLE);
    Serial.print("   N: ");
    Serial.print(treble.LOW_NOISE_COUNT);
    Serial.print(",");
    Serial.print(treble.HIGH_NOISE_COUNT);
    Serial.println();
    
    */
    
    
    
    debug_check = 0;
  }
  
  // for tracking noise debugging 
  //HIGH_MAX = max(HIGH_MAX, treble.HIGH_NOISE_COUNT);
  
  /* read from the analog input */
  treble.readInput();
  bass.readInput();
  
  if ( treble.LEVEL > 127 && treble.LEVEL < 221 ) {
    thisPWM = constrain(map(treble.LEVEL,128,220,1,20),1,20);
    chan.setPWM_all(thisPWM);
    //chan.all_on();
  } else if ( treble.LEVEL > 220 ) {
    thisPWM = constrain(map(treble.LEVEL,220,510,21,200),21,200);
    chan.setPWM_all(thisPWM);
  }
  
  // beat decetion 
  if ( bass.SAMPLE == bass.MIN ) {
    thisPWM = 255;
    chan.setPWM_all(thisPWM);
    digitalWrite(BASS_LED, HIGH);
  } else if ( bass.SAMPLE == bass.MAX ) {
    chan.setPWM_all(0);
  } else {
    digitalWrite(BASS_LED, LOW);
  }
  
  if ( treble.LEVEL < 128 ) {
    //chan.random_memory_reset();
    chan.all_off(); 
  } else {
    chan.all_on();
  }
  
  
  /*
  } else if ( (treble.LEVEL > 127) && (treble.HIGH_NOISE_COUNT < 200) ) {
    chan.all_on();
  } else if ( (treble.LEVEL > 127) && (treble.HIGH_NOISE_COUNT >= 200) && ((treble.CORRECTION_COUNT % 20) == 0) ) {
    chan.random_memory_set();
    chan.random_memory_on(); 
  } else {
    chan.random_memory_on();
  }
  
  */
  if ( (treble.CORRECTION_COUNT % 20) == 0 ) {
    chan.all_off();
    //treble.MIN++;
  }
  
  
}

