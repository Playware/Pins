
//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "Movement.h"

// Pin numbers for acc and gyro are not in same order,
// this is a map from pin number to array number.
static const int PIN_MAP[6] = {0, 1, 2, 4, 3, 5};

enum PointName {R_, RX, RY, RZ, RXZ, AX, AY, AZ, AXZ, GYZ, GXZ, GXY};

void Movement::setup()
{
  analogReference(EXTERNAL); // Using external ref of 3.3V
  DDRK = B00000000;          // Set all PORTF as inputs
  for (int i=0; i<12; i++)
  {
    isValueSet[i] = false;
  }

  /*for (int i=0; i<3; i++)
  {
    kalmanData[i].xBias = 0;
    for (int j=0; j<4; j++)
    {
      kalmanData[i].P[j] = 0;
    }
  }*/
};

void Movement::calibrate()
{
  int16_t calibrateValues[6];
  calibrate(calibrateValues);
}

void Movement::calibrate(int16_t* calibrateValues)
{
  long tmpCalc = 0;
  for (int j=FIRST_PIN; j<=LAST_PIN; j++)
  {
    tmpCalc = 0;
    for (int i = 0; i < 100; i++)
    {
      tmpCalc += analogRead(j);
    }
    zeroValues[PIN_MAP[j-FIRST_PIN]] = tmpCalc/100;
  }
  // Due to gravity on y axis, zero y value is populated from zero x value.
  // x and y should have the same zero values.
  zeroValues[1] = zeroValues[0];
  for (int i = 0; i < 6; i++)
  {
    calibrateValues[i] = zeroValues[i];
  }
};

void Movement::setCalibrationValues(int16_t* calibration)
{
  for (int i = 0; i < 6; i++)
  {
    zeroValues[i] = calibration[i];
  }
}

void Movement::update()
{
  long tmp = 0;
  for (int i=FIRST_PIN; i<=LAST_PIN; i++)
  {
    tmp = 0;
    for (int j=0; j<5; j++)
    {
      tmp += analogRead(i);
    }
    measuredValues[PIN_MAP[i-FIRST_PIN]] = tmp/5;
  }

  for (int i=0; i<12; i++)
  {
    isValueSet[i] = false;
  }
};

float* Movement::getAccValue_(uint8_t idx)
{
  uint8_t idxOffset = idx + OFFSET_ACC;
  if (!isValueSet[idxOffset])
  {
    if (idxOffset == XZ_IDX)
    {
      values[idxOffset] = sqrt(pow(*getAccValue_(X_IDX), 2) + pow(*getAccValue_(Z_IDX), 2));
    }
    else
    {
      values[idxOffset] = (measuredValues[idxOffset] - zeroValues[idxOffset])/ACC_CONV;
    }
    isValueSet[idx] = true;
  }
  return &values[idxOffset];
};

float* Movement::getAccAngle_(uint8_t idx)
{
  uint8_t idxOffset = idx + OFFSET_ACC_ANGLE;
  if (!isValueSet[idxOffset])
  {
    values[idxOffset] = acos(*getAccValue_(idx)/(*R()))*RAD_TO_DEG;
    isValueSet[idxOffset] = true;
  }
  return &values[idxOffset];
}

float* Movement::getGyroValue_(uint8_t idx)
{
  uint8_t idxOffset = idx + OFFSET_GYRO;
  uint8_t idxOffset2 = idx + OFFSET_GYRO - 1;
  if (!isValueSet[idxOffset])
  {
    values[idxOffset] = (measuredValues[idxOffset2] - zeroValues[idxOffset2])/GYRO_CONV;
    isValueSet[idxOffset] = true;
  }
  return &values[idxOffset];
}

float* Movement::R()
{
  if (!isValueSet[OFFSET_MAGNITUDE])
  {
    values[OFFSET_MAGNITUDE] = sqrt(
      pow(*getAccValue_(X_IDX), 2) + 
      pow(*getAccValue_(Y_IDX), 2) + 
      pow(*getAccValue_(Z_IDX), 2)
    ); // Magnitude of force
    isValueSet[OFFSET_MAGNITUDE] = true;
  }
  return &values[OFFSET_MAGNITUDE];
};

float* Movement::Rx()
{
  return getAccValue_(X_IDX);
};

float* Movement::Ry()
{
  return getAccValue_(Y_IDX);
};

float* Movement::Rz()
{
  return getAccValue_(Z_IDX);
};

float* Movement::Rxz()
{
  return getAccValue_(XZ_IDX);
}

float* Movement::Ax()
{
  return getAccAngle_(X_IDX);
}

float* Movement::Ay()
{
  return getAccAngle_(Y_IDX);
}

float* Movement::Az()
{
  return getAccAngle_(Z_IDX);
}

float* Movement::Axz()
{
  return getAccAngle_(XZ_IDX);
}

float* Movement::Gyz()
{
  return getGyroValue_(X_IDX);
};

float* Movement::Gxz()
{
  return getGyroValue_(Y_IDX);
};

float* Movement::Gxy()
{
  return getGyroValue_(Z_IDX);
};

/*float* Movement::getEstRotAngle_(uint8_t idx)
{
  uint8_t idxOffset = idx + 3;
  if (!isValueSet[idxOffset])
  {
    float gyroRate;
    float accRotAngle;
    getGyroValue_(idx, &gyroRate);
    getAccRotAngle_(idx, &accRotAngle);

    kalmanFilteredAngle_(&values[idxOffset], &accRotAngle, &gyroRate, deltaTime, &kalmanData[idx]);
  }
  return &values[idxOffset];
};*/


void Movement::kalmanFilteredAngle_(float* xAngle, float* newAngle, float* newRate, int deltaTime, KalmanStruct* data)
{
  float dt = deltaTime/1000.0;

  *xAngle += dt*(*newRate - data->xBias);

  data->P[0] += dt*(Q_ANGLE - (data->P[2] + data->P[1]));
  data->P[1] += -dt*data->P[3];
  data->P[2] += -dt*data->P[3];
  data->P[3] += dt*Q_GYRO;

  float y = *newAngle - *xAngle;
  float S = data->P[0] + R_ANGLE;
  float K0 = data->P[0]/S;
  float K1 = data->P[2]/S;

  *xAngle += K0*y;
  data->xBias += K1*y;
  data->P[0] -= K0*data->P[0];
  data->P[1] -= K0*data->P[1];
  data->P[2] -= K1*data->P[0];
  data->P[3] -= K1*data->P[1];
}


void Movement::mdebug(void (*printFunc)(const uint8_t, float&), uint16_t &index)
{
  // Send all values

  float tmp;

  //printFunc("deltaTime", (float)deltaTime);

  printFunc(R_, *R());

  printFunc(RX, *getAccValue_(X_IDX));
  printFunc(RY, *getAccValue_(Y_IDX));
  printFunc(RZ, *getAccValue_(Z_IDX));
  printFunc(RXZ, *getAccValue_(XZ_IDX));

  //printFunc(AX, *getAccAngle_(X_IDX));
  //printFunc(AY, *getAccAngle_(Y_IDX));
  //printFunc(AZ, *getAccAngle_(Z_IDX));

  printFunc(GYZ, *getGyroValue_(X_IDX));
  printFunc(GXZ, *getGyroValue_(Y_IDX));
  printFunc(GXY, *getGyroValue_(Z_IDX));


  /*printFunc("EstAyz", *getEstRotAngle_(X_IDX));
  printFunc("EstAxz", *getEstRotAngle_(Y_IDX));
  printFunc("EstAxy", *getEstRotAngle_(Z_IDX));*/

  index++;
}

Movement movement;
