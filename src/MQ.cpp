#include "MQ.h"

#define TOTAL_SAMPLE_R0      20
#define TOTAL_SAMPLE_PER_CAL 50

MQ::MQ(byte channel)
{
  _analogPin = channel;
}

void MQ::setRl(float RL)
{
  _RL = RL;
  _R0 = 0;
}

void MQ::setGraphPoints(float x1, float x2, float y1, float y2)
{
  _m = log10(y2 / y1) / log10(x2 / x1);
  _c = log10(y1) - _m * log10(x1);
  //  Serial.println(_m);
  //  Serial.println(_c);
}

float MQ::getPPM()
{
  Rs = _calculateRs();

  float ratio_RS_R0 = (Rs / _R0);
  //Serial.print(F("Rs/R0: "));Serial.println(ratio_RS_R0, 4);

  float ppm_log = (log10(ratio_RS_R0) - _c) / _m; //x = (y-c)/m
  float ppm = pow(10, ppm_log);
  return ppm;
}

float MQ::_calculateRs()
{
  int Vrl;
  for(uint8_t i = 0; i< TOTAL_SAMPLE_PER_CAL; i++)
  {
    Vrl += analogRead(_analogPin);
  }

  int Vrl = Vrl/TOTAL_SAMPLE_PER_CAL;
  float RS = ((1023 - Vrl) * _RL) / Vrl; //deduced from voltage divider equation
  return RS;
}

float MQ::calculateR0(float air_Rs_by_R0)
{
  float Rs_Air;
  for (byte i = 0; i < TOTAL_SAMPLE_R0; i++)
  {
    Rs_Air += _calculateRs();
  }
  Rs_Air = Rs_Air / TOTAL_SAMPLE_R0;
  
  Rs_Air = Rs_Air / air_Rs_by_R0;
  _R0 = Rs_Air;
  Serial.print("R0: ");Serial.println(_R0);
  if (_R0 < 0)
  {
    _R0 = 0; //no negative value accepted
  } 
  return _R0;
}

