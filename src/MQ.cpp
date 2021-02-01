#include "MQ.h"

#define TOTAL_SAMPLE_R0      20
#define TOTAL_SAMPLE_PER_CAL 50

MQ::MQ(byte channel)
{
  _analogPin = channel;
}

void MQ::setMemFun(funCalib_t save, funCalib_t read)
{
  _saveCalib = save;
  _readCalib = read;

  if(_readCalib != NULL)
  {
    _readCalib(&_calib);
  }
}

void MQ::setXY(float x1, float x2, float y1, float y2)
{
  _calib.m = log10(y2 / y1) / log10(x2 / x1);
  _calib.c = log10(y1) - _m * log10(x1);
  //  Serial.println(_m);
  //  Serial.println(_c);
}

void MQ::setR(float RL, float air_Rs_by_R0)
{
  _calib.RL = RL;
  _calib.airRsR0 = air_Rs_by_R0;
}

void MQ::runCalib()
{
  _calib.R0 = calculateR0(_calib.airRsR0);
  _calib.done  = true;
  if(_saveCalib != NULL)
  {
    _saveCalib(&_calib);
  }
}

void begin()
{
  if(_calib.done == false)
  {
    runCalib();
  }
  else
  {
    Serial.println(F("Sensor is Calibrated"));
  }
}

// void MQ::setRl(float RL)
// {
//   _RL = RL;
//   _R0 = 0;
// }

// void MQ::setGraphPoints(float x1, float x2, float y1, float y2)
// {
//   _m = log10(y2 / y1) / log10(x2 / x1);
//   _c = log10(y1) - _m * log10(x1);
//   //  Serial.println(_m);
//   //  Serial.println(_c);
// }

float MQ::getPPM()
{
  // Rs = _calculateRs();

  float ratio_RS_R0 = (_calculateRs() / _R0);
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
  // _R0 = Rs_Air;
  Serial.print("R0: ");Serial.println(Rs_Air);
  if (Rs_Air < 0)
  {
    Rs_Air = 0; //no negative value accepted
  } 
  return Rs_Air;
}

