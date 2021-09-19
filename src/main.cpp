#include <Arduino.h>
#include <SPI.h>
#include <math.h>
#include <String.h>
#include "ADXL345.h"

#define CS_PIN 37
#define SCLK_PIN 13
#define MOSI_PIN 11
#define MISO_PIN 12

void averageXYZ(double *xyz);
void dumpADXL(ADXL345 adxl);
/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using   */
ADXL345 adxl1 = ADXL345(37, "first"); // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl2 = ADXL345(41, "second");
//ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/****************** VARIABLES ******************/
/*                                             */
int AccelMinX = 0;
int AccelMaxX = 0;
int AccelMinY = 0;
int AccelMaxY = 0;
int AccelMinZ = 0;
int AccelMaxZ = 0;

int accX = 0;
int accY = 0;
int accZ = 0;
int accX2 = 0;
int accY2 = 0;
int accZ2 = 0;

double avgx[10], avgy[10], avgz[10];
double avgx2[10], avgy2[10], avgz2[10];
double ax, ay, az;

/************** DEFINED VARIABLES **************/
/*                                             */
#define offsetX 0 // OFFSET values
#define offsetY 0
#define offsetZ 0

#define gainX 1 // GAIN factors
#define gainY 1
#define gainZ 1
/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup()
{
  Serial.begin(115200); // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Breakout Calibration");
  Serial.println();
  SPI.setSCK(SCLK_PIN);
  SPI.setMOSI(MOSI_PIN);
  SPI.setMISO(MISO_PIN);
  delay(10);

  adxl1.powerOn(); // Power on the ADXL345
  adxl2.powerOn();

  adxl1.setSelfTestBit(0);
  adxl2.setSelfTestBit(0);

  adxl1.setRangeSetting(8); // Give the range settings
  adxl2.setRangeSetting(8); // Accepted values are 2g, 4g, 8g or 16g
  // Higher Values = Wider Measurement Range
  // Lower Values = Greater Sensitivity

  adxl1.setSpiBit(0); // Configure the device: 4 wire SPI mode = '0' or 3 wire SPI mode = 1
  adxl2.setSpiBit(0); // Default: Set to 1
  // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library
}

/****************** MAIN CODE ******************/
/*  Accelerometer Readings and Min/Max Values  */
void loop()
{

  // Get the Accelerometer Readings

  /* Note: Must perform offset and gain calculations prior to seeing updated results
  /  Refer to SparkFun ADXL345 Hook Up Guide: https://learn.sparkfun.com/tutorials/adxl345-hookup-guide
  /  offsetAxis = 0.5 * (Acel+1g + Accel-1g)
  /  gainAxis = 0.5 * ((Acel+1g - Accel-1g)/1g) */

  // UNCOMMENT SECTION TO VIEW NEW VALUES
  /*   accX2 = (x2 - offsetX) / gainX; // Calculating New Values for X, Y and Z
  accY2 = (y2 - offsetY) / gainY;
  accZ2 = (z2 - offsetZ) / gainZ; */
  //adxl2.readAccel(&x2, &y2, &z2); // Read the accelerometer values and store in variables x,y,z

  /*   adxl1.get_Gxyz(Gxyz);
  averageXYZ(Gxyz, 1);
  double xAngle = atan(ax / (sqrt(pow(ay, 2) + pow(az, 2))));
  double yAngle = atan(ay / (sqrt(pow(ax, 2) + pow(az, 2))));
  double zAngle = atan(sqrt(pow(ax, 2) + pow(ay, 2)) / az);

  xAngle *= 180.00;
  yAngle *= 180.00;
  zAngle *= 180.00;
  xAngle /= 3.141592;
  yAngle /= 3.141592;
  zAngle /= 3.141592; */

  /*   Serial.print("ADXL1 Gvalues: ");
  Serial.print(xAngle);
  Serial.print("  ");
  Serial.print(yAngle);
  Serial.print("  ");
  Serial.print(zAngle);
  Serial.println();
  Serial.println(); */

  /*   adxl2.get_Gxyz(Gxyz2);
  averageXYZ(Gxyz2, 2);
  xAngle = atan(ax / (sqrt(pow(ay, 2) + pow(az, 2))));
  yAngle = atan(ay / (sqrt(pow(ax, 2) + pow(az, 2))));
  zAngle = atan(sqrt(pow(ax, 2) + pow(ay, 2)) / az);

  xAngle *= 180.00;
  yAngle *= 180.00;
  zAngle *= 180.00;
  xAngle /= 3.141592;
  yAngle /= 3.141592;
  zAngle /= 3.141592; */
  dumpADXL(adxl1);
  dumpADXL(adxl2);
  delay(5);
}

void dumpADXL(ADXL345 adxl)
{
  double Gxyz[3];
  adxl.get_Gxyz(Gxyz);
  averageXYZ(Gxyz);
  double xAngle = atan(ax / (sqrt(pow(ay, 2) + pow(az, 2))));
  double yAngle = atan(ay / (sqrt(pow(ax, 2) + pow(az, 2))));
  double zAngle = atan(sqrt(pow(ax, 2) + pow(ay, 2)) / az);

  xAngle *= 180.00;
  yAngle *= 180.00;
  zAngle *= 180.00;
  xAngle /= 3.141592;
  yAngle /= 3.141592;
  zAngle /= 3.141592;
  Serial.print("ADXL " + adxl.getName() + " Gvalues: ");
  Serial.print(xAngle);
  Serial.print("  ");
  Serial.print(yAngle);
  Serial.print("  ");
  Serial.print(zAngle);
  Serial.println();
  Serial.println();
}

void averageXYZ(double *xyz)
{
  static int counter = 0;
  double xtotal = 0.0f, ytotal = 0.0f, ztotal = 0.0f;
  avgx[counter] = xyz[0];
  avgy[counter] = xyz[1];
  avgz[counter] = xyz[2];

  for (int i = 0; i < 5; i++)
  {
    xtotal = xtotal + avgx[i];
    ytotal = ytotal + avgy[i];
    ztotal = ztotal + avgz[i];
  }
  counter = counter + 1;
  if (counter > 9)
  {
    counter = 0;
  }

  ax = xtotal / 5;
  ay = ytotal / 5;
  az = ztotal / 5;
}