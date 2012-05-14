#include "AudioBeat.h"

void AudioBeat::init() {
  /* set the counter limits */
  _correction_limit = 1000;    // auto correct the MIN & MAX every 1000 samples
  _high_noise_limit = 10000;   // resample the waveform when the high noise count reaches 10000
  _low_noise_limit = 10000;    // resample the waveform when the low noise count reaches 10000
  _high_noise_threshold = 384; // add to the high noise count if the LEVEL is greater then 384
  _low_noise_threshold = 128;  // add to the low noise count if the LEVEL is less the 128
  _high_noise_reset = 0;       // reset the high noise count if the LEVEL reaches 0
  _low_noise_reset = 511;      // reset the low noise count if the LEVEL reaches 511
  
  /* init the counters */
  HIGH_NOISE_COUNT = 0; LOW_NOISE_COUNT = 0; CORRECTION_COUNT = 0;
  
  /* init the waveform values */
  RAW = 0; SAMPLE = 0; MIN = 0; MAX = 0; RANGE = 0; LEVEL = 0; VOLUME = 0;
}

void AudioBeat::setPIN(int pin) {
  _pin = pin;            // set the pin
  pinMode(_pin, INPUT);  // ready the analog pin for input
}

AudioBeat::AudioBeat(void) {
  init();       // init stuff
  setPIN(A0);   // use analog 0 for input
}

AudioBeat::AudioBeat(int pin) {
  init();       // init stuff
  setPIN(pin);  // set the analog pin
}

/* sets the correction limit */
void AudioBeat::setCorrectionLimit(int limit) {
  _correction_limit = limit;
}

/* sets the high noise limit */
void AudioBeat::setHighNoiseLimit(unsigned long limit) {
  _high_noise_limit = limit;
}

/* sets the low noise limit */
void AudioBeat::setLowNoiseLimit(unsigned long limit) {
  _low_noise_limit = limit;
}

/* sets the point at which we start running corrections due to the amount of high noise */
void AudioBeat::setHighNoiseThreshold(int thres) {
  _high_noise_threshold = thres;
}

/* sets the point at which we start running corrections due to the amount of low noise */
void AudioBeat::setLowNoiseThreshold(int thres) {
  _low_noise_threshold = thres;
}

/* sets the point at which we reset the high noise counter due to the amount of low noise */
void AudioBeat::setHighNoiseReset(int reset) {
  _high_noise_reset = reset;
}
    
/* sets the point at which we reset the low noise counter due to the amount of high noise */
void AudioBeat::setLowNoiseReset(int reset) {
  _low_noise_reset = reset;
}

/* reads the analog input and calculates values */
void AudioBeat::readInput() {
  /* incress the correction counter */
  CORRECTION_COUNT++;
  
  /* should we correct the sample range? */
  if ( CORRECTION_COUNT > _correction_limit ) {
    MIN++;
    MAX--;
    CORRECTION_COUNT = 0;
  }
  
  /* start taking readings */
  MIN = min(MIN, RAW);       // get current the MIN
  RAW = analogRead(_pin);    // read value from the analog pin
  MAX = max(MAX, RAW);       // get the current MAX
  
  
  /* waveform too loud or too low?  should we lower the sample range? */
  if ( HIGH_NOISE_COUNT > _high_noise_limit || LOW_NOISE_COUNT > _low_noise_limit ) {
    int temp = (analogRead(_pin) / 2);
    MAX = temp + 10; MIN = temp - 5;
    HIGH_NOISE_COUNT = 0; LOW_NOISE_COUNT = 0;
  }
  
  /* set the range, middle, and sample */
  RANGE = MAX - MIN;
  MIDDLE = RANGE / 2;  // set the middle of the waveform
  SAMPLE = RAW - MIN;
  
  /* set the level */
  LEVEL = SAMPLE - MIDDLE;
  if ( LEVEL < 0 ) {        // are we on the lower part of the waveform?
    LEVEL = LEVEL * -1;     // if so make the negitive value a positive one
  }
  /* create postive level given the current sample rate between 0 and 511 */
  LEVEL = constrain(map(LEVEL,0,MIDDLE,0,511),0,511); 
  
  /* add to the high noise count */
  if ( LEVEL > _high_noise_threshold ) {
   HIGH_NOISE_COUNT++;
  }
  
  /* add to the low noise count */
  if ( LEVEL < _low_noise_threshold ) {
   LOW_NOISE_COUNT++;
  }
  
  /* reset the high noise count */
  if ( LEVEL <= _high_noise_reset ) { 
    HIGH_NOISE_COUNT = 0;
  }
  
  /* reset the high noise count */
  if ( LEVEL >= _low_noise_reset ) { 
    LOW_NOISE_COUNT = 0;
  }
  
  /* create postive volume value given the level between 0 and 100 */
  VOLUME = constrain(map(LEVEL,0,MIDDLE,0,100),0,100); 
}