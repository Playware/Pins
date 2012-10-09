/*
 * Movement
 *
 * Arduino library for 6DOF (accelerometer and gyroscope)
 * IMU Analog Combo Board Razor - 6DOF Ultra-Thin IMU 
 * http://www.sparkfun.com/products/10010 
 *
 * Author: Arnþór Magnússon <arnthorm@gmail.com>
 * Created: 8. Jun 2011
 * Updated: 8. Jun 2011
 */

/*
 * TODO:
 *   - Do we need calibration?, Are there standard values which are possible to
 *     use to zero/calibrate with ...
 *   - Skoða hvaða gildi koma úr calibrate, x,y,z (accelometer) til að null stilla
 *     - Skoða zero gildi fyrir acc og gyro
 *   - Athuga hvort x,y,z á accu passi við gyri (sjá accu gyro tutorial)
 *   - ATH!, update gert mörgum sinnum, en kalman gert bara á sec fresti. Má það?
 */

#ifndef Movement_h
#define Movement_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"

#define X_IDX 0
#define Y_IDX 1
#define Z_IDX 2
#define XZ_IDX 3

#define OFFSET_ACC 0
#define OFFSET_GYRO 4
#define OFFSET_ACC_ANGLE 8
#define OFFSET_MAGNITUDE 11

class Movement
{
  struct KalmanStruct {
    float xBias;
    float P[4];
  } kalmanData[3];

  // Offset values/calibrated values for (acc) Rx, Ry, Rz and (gyro) Gyz, Gxz, Gxy
  int16_t measuredValues[6]; // Raw measurements

  // Calculated values for Rx, Ry, Rz, Gyz, Gxz, Gxy, Ax, Ay, Az, R (respectively)
  float values[12]; // Calculated values 
  bool isValueSet[12]; // Are values in values[] set

  // TODO: Make pin selection part of constructor
  static const uint8_t FIRST_PIN = 9;
  static const uint8_t LAST_PIN  = 14;

  static const float ACC_CONV = 102.3; // Sensitivity/(Vref/1023) = (0.33V/g)/(3.3V/1023)
  static const float GYRO_CONV = 1.0323; // Sensitivity/(Vref/1023) = (0.00333V/°/s)/(3.3V/1023)
  static const float GYRO_ZERO = 381.3; // 1.23V*1023/3.3V

  static const float Q_ANGLE  =  0.001; 
  static const float Q_GYRO   =  0.003;  
  static const float R_ANGLE  =  0.03;  

  uint16_t deltaTime;
  unsigned long timer;

  public:
  int16_t zeroValues[6]; // "Zero" data, when rolling pin is not moving.
    /*
     * Pin setup, calibration and timer
     */
    void setup();
    /* 
     * Calibrate the accelerometer
     *
     * Takes 100 samples for each sensor input and generates zero values.
     * When calibrating, the y axis must be vertical.
     */
    void calibrate();
    void calibrate(int16_t* calibrateValues);
    void setCalibrationValues(int16_t*);
    /*
     * Update
     *
     * Measures values from accu and  gyro and resets timer.
     */
    void update();

    // Magnitudes
    float* R();
    float* Rx();
    float* Ry();
    float* Rz();
    float* Rxz();

    // Angles (in degrees)
    float* Ax();
    float* Ay();
    float* Az();
    float* Axz();

    // Rotation (acceleration)
    float* Gyz(); // Rotation around x axis
    float* Gxz(); // Rotation around y axis
    float* Gxy(); // Rotation around z axis

    void mdebug(void (*printFunc)(const uint8_t, float&), uint16_t &index);

  private:
    float* getAccValue_(uint8_t idx);
    float* getAccAngle_(uint8_t idx);
    float* getGyroValue_(uint8_t idx);

    /*float* getEstRotAngle_(uint8_t idx);
    float* getEstR_(uint8_t idx);
    float* getEstAngle_(uint8_t idx);*/
    /*
     * Kalman filtered angle
     *
     * Fuses values from accu and gyro into a estimated angle.
     */
    void kalmanFilteredAngle_(float* xAngle, float* newAngle, float* newRate, int deltaTime, KalmanStruct* data);
};

extern Movement movement;

#endif
