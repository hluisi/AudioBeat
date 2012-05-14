#ifndef AudioBeat_H
#define AudioBeat_H
 
#include <Arduino.h> 

class AudioBeat {
  private:
    /* used via constructors to init default values */
    void init();
    
    /* analog pin */
    int _pin;
    
    /* the rate at which we run corrections on the sample waveform */
    int _correction_limit;
    
    /* the amount of high noise at which we say enough is enough and resample the waveform */
    unsigned long _high_noise_limit;
    
    /* the amount of low noise at which we say enough is enough and resample the waveform */
    unsigned long _low_noise_limit;
    
    /* the point at which we start running MIN and MAX corrections due to the amount of high noise coming in */
    int _high_noise_threshold;
    
    /* the point at which we start running MIN and MAX corrections due to the amount of low noise coming in */
    int _low_noise_threshold;
    
    /* the point at which reset the high noise counter due to low noise */
    int _high_noise_reset;
    
    /* the point at which reset the low noise counter due to high noise */
    int _low_noise_reset;
    
  public:
    /* counters used to change the sample peaks based on the waveform length */
    unsigned long HIGH_NOISE_COUNT, LOW_NOISE_COUNT;
    int CORRECTION_COUNT;
    
    /* <<constructor>> uses A0 for default pin */
    AudioBeat(void);
    
    /* <<constructor>> for different analog pin */
    AudioBeat(int pin);
    
    /* set the analog pin */
    void setPIN(int pin);
    
    /* sets the limit at which we run corrections on the sample waveform */
    void setCorrectionLimit(int limit);
    
    /* sets the limit at which we resample the waveform due to the amount of high noise */
    void setHighNoiseLimit(unsigned long limit);
    
    /* sets the limit at which we resample the waveform due to the amount of low noise */
    void setLowNoiseLimit(unsigned long limit);
    
    /* sets the point at which we start running corrections due to the amount of high noise */
    void setHighNoiseThreshold(int thres);
    
    /* sets the point at which we start running corrections due to the amount of low noise */
    void setLowNoiseThreshold(int thres);
    
    /* sets the point at which we reset the high noise counter due to the amount of low noise */
    void setHighNoiseReset(int reset);
    
    /* sets the point at which we reset the low noise counter due to the amount of high noise */
    void setLowNoiseReset(int reset);
    
    /* reads the analog input and calculates values */
    void readInput();
    
    /* the raw analog waveform */
    int RAW;
    
    /* a sample of the waveform based on the current minimum and maximum values */
    int SAMPLE;
    
    /* the minimum peak of the sampled waveform */
    int MIN;
    
    /* the maximum peak of the sampled waveform */
    int MAX;
    
    /* the current range of the waveform from top to bottom (min & max) */
    int RANGE;
    
    /* the middle of the waveform based on the current RANGE */
    int MIDDLE;
    
    /* a postive level mapping of the sampled waveform from 0 to 511 
       (no matter what the current sampling rate may be) */
    int LEVEL;
    
    /* a mapping of the level from 0 to 100 */
    byte VOLUME;
    
};

#endif