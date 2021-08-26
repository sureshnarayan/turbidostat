/*
  Mesurer la qualité d'éclairage de votre habitation à l'aide d'un capteur GY-302 (BH1750)
  Measure the lighting quality of your home with a GY-30 (BH1750) sensor

  Code basé sur la librairie Arduino de Christopher Laws disponible sur GitHub
  Based on the Arduino library of Christopher Laws abailable on GitHub https://github.com/claws/BH1750
  
  Connection:
    VCC -> 5V (3V3 on Arduino Due, Zero, MKR1000, etc)
    GND -> GND
    SCL -> SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due)
    SDA -> SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due)
    ADD -> GND or VCC (see below)

  ADD pin uses to set sensor I2C address. If it has voltage greater or equal to
  0.7VCC voltage (as example, you've connected it to VCC) - sensor address will be
  0x5C. In other case (if ADD voltage less than 0.7 * VCC) - sensor address will
  be 0x23 (by default).

  https://projetsdiy.fr - https://diyprojects.io
*/

#include <Wire.h>
#include <BH1750.h>

/*
 * Niveau d'éclairage définit à partir de la norme EN 12464-1 
 * Lighting level defined from the standard EN 12464-1 
 * http://www.afe-eclairage.fr/docs/362-ext.pdf
 */
#define _TOOLOW                       25
#define _LOW                          50
#define _HIGH                         500
#define _TOOHIGH                      750
#define LEVEL_TOOLOW                  "Trop bas"      // Too low
#define LEVEL_LOW                     "Bas"           // Low
#define LEVEL_OPTIMAL                 "Idéal"         // Ideal
#define LEVEL_HIGH                    "Elevé"         // High
#define LEVEL_TOOHIGH                 "Trop élevé"    // Too High

uint16_t lux        = 250;
int      luxLevel   = 3;
String   luxMessage = LEVEL_OPTIMAL;
/*
  BH1750 can be physically configured to use two I2C addresses:
    - 0x23 (most common) (if ADD pin had < 0.7VCC voltage)
    - 0x5C (if ADD pin had > 0.7VCC voltage)

  Library use 0x23 address as default, but you can define any other address.
  If you had troubles with default value - try to change it to 0x5C.
*/
BH1750 lightMeter(0x23);

void setup(){

  Serial.begin(115200);
  /*
    Each mode, has three different precisions:

      - Low Resolution Mode - (4 lx precision, 16ms measurment time)
      - High Resolution Mode - (1 lx precision, 120ms measurment time)
      - High Resolution Mode 2 - (0.5 lx precision, 120ms measurment time)

    Full mode list:

      BH1750_CONTINUOUS_LOW_RES_MODE
      BH1750_CONTINUOUS_HIGH_RES_MODE (default)
      BH1750_CONTINUOUS_HIGH_RES_MODE_2

      BH1750_ONE_TIME_LOW_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE_2
  */

  lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
  Serial.println(F("BH1750 Test"));
}

void loop() {
  
  lux = lightMeter.readLightLevel();
  
  if ( lux <= _TOOLOW ) {
    luxLevel = 1;
    luxMessage = LEVEL_TOOLOW;
  } else if ( lux > _TOOLOW && lux <= _LOW ) {
    luxLevel = 2;
    luxMessage = LEVEL_LOW;
  } else if ( lux > _LOW && lux <= _HIGH ) {
    luxLevel = 3;
    luxMessage = LEVEL_OPTIMAL;
  } else if ( lux > _HIGH && lux < _TOOHIGH ) {
    luxLevel = 4;
    luxMessage = LEVEL_HIGH;
  } else {
    luxLevel = 5;
    luxMessage = LEVEL_TOOHIGH;
  }
  
  Serial.print("Light: "); 
  Serial.print(lux);
  Serial.print(" lx, level: ");
  Serial.print(luxLevel);
  Serial.print(" , quality: ");
  Serial.println(luxMessage);
  delay(1000);
}
