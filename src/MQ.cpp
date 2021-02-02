#include "MQ.h"

#define TOTAL_SAMPLE_R0      20
#define TOTAL_SAMPLE_PER_CAL 50

MQ::MQ(byte channel)
{
  _analogPin = channel;
}

void MQ::setXY(float x1, float x2, float y1, float y2)
{
  _calib.m = log10(y2 / y1) / log10(x2 / x1);
  _calib.c = log10(y1) - _calib.m * log10(x1);
   Serial.print(F("m : "));Serial.println(_calib.m,6);
   Serial.print(F("c : "));Serial.println(_calib.c);
}

void MQ::setR(float RL, float air_Rs_by_R0)
{
  _calib.RL = RL;
  _calib.airRsR0 = air_Rs_by_R0;
}

void MQ::runCalib(funCalib_t save)
{
  _calib.R0 = calculateR0(_calib.airRsR0);
  _calib.done  = true;
  if(save != NULL)
  {
    save(&_calib);
  }
}

void MQ::beginFromMem(funCalib_t read)
{
  if(read != NULL)
  {
    read(&_calib);
  }

  if(_calib.done == true)
  {
     Serial.println(F("Sensor is Calibrated"));
  }
  else
  {
    Serial.println(F("Sensor not Calibrated"));
  }
}


float MQ::getPPM()
{
  // Rs = _calculateRs();

  float ratio_RS_R0 = (_calculateRs() / _calib.R0);
  //Serial.print(F("Rs/R0: "));Serial.println(ratio_RS_R0, 4);

  float ppm_log = (log10(ratio_RS_R0) - _calib.c) /_calib.m; //x = (y-c)/m
  float ppm = pow(10, ppm_log);
  return ppm;
}

float MQ::_calculateRs()
{
  uint32_t Vrs = 0;
  for(uint8_t i = 0; i< TOTAL_SAMPLE_PER_CAL; i++)
  {
    uint32_t adc = analogRead(_analogPin);
    // Serial.print(F("Adc : ")); Serial.println(adc);
    Vrs += adc;
  }

  Vrs = Vrs/TOTAL_SAMPLE_PER_CAL;
  // Serial.print(F("VRs : "));Serial.println(Vrs);
  float RS = ((1023 - Vrs) * _calib.RL) / Vrs; //deduced from voltage divider equation
  // Serial.print(F("Rs : "));Serial.println(RS);
  return RS;
}

float MQ::calculateR0(float air_Rs_by_R0)
{
  float Rs_Air;
  // float  ratio = pow(10.0,air_Rs_by_R0);
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

