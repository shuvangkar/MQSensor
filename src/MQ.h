#ifndef _MQ_H_
#define _MQ_H_
#include "Arduino.h"


typedef struct calib_t
{
  float c;
  float m;
  float RL,
  float R0;
  float airRsR0;
  bool done;
};

void saveCalib(calib_t *cPtr);
void readCalib(calib_t *cPtr);

typedef void (*funCalib_t)(calib_t*);

typedef void (*saveCalib)()
class MQ
{
  public:
    MQ(byte channel);
    // MQ(byte ch, float RL, float airRsR0);
    void setGraphPoints(float x1, float x2, float y1, float y2);
    void setRl(float RL);
    float getPPM();
    float getRs();
    float calculateR0(float air_Rs_by_R0);
    // float Rs;
  private:
    float _calculateRs();
    byte _analogPin;
    float _RL;
    float 
    float _m;
    float _c;
    
    float _R0;
};
#endif
