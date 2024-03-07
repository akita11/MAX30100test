#include <Arduino.h>
#include <M5unified.h>
#include <Wire.h>

#include "MAX30100.h"

// Sampling is tightly related to the dynamic range of the ADC.
// refer to the datasheet for further info
//#define SAMPLING_RATE                       MAX30100_SAMPRATE_100HZ
#define SAMPLING_RATE                       MAX30100_SAMPRATE_50HZ

// The LEDs currents must be set to a level that avoids clipping and maximises the
// dynamic range
#define IR_LED_CURRENT                      MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT                     MAX30100_LED_CURR_27_1MA

// The pulse width of the LEDs driving determines the resolution of
// the ADC (which is a Sigma-Delta).
// set HIGHRES_MODE to true only when setting PULSE_WIDTH to MAX30100_SPC_PW_1600US_16BITS
#define PULSE_WIDTH                         MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE                        true


// Instantiate a MAX30100 sensor class
MAX30100 sensor;

// display: 240 x 135
#define X 240
#define Y 135

uint16_t val_red[X], val_ir[X];
uint8_t px = 0;

void setup()
{
  M5.begin();
//  Serial.begin(115200);
//  Serial.print("Initializing MAX30100..");

  if (!sensor.begin()) {
    printf("fail\n");
//      Serial.println("FAILED");
      for(;;);
  } else {
    printf("ok\n");
//      Serial.println("SUCCESS");
  }

  // Set up the wanted parameters
  sensor.setMode(MAX30100_MODE_SPO2_HR);
  sensor.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
  sensor.setLedsPulseWidth(PULSE_WIDTH);
  sensor.setSamplingRate(SAMPLING_RATE);
  sensor.setHighresModeEnabled(HIGHRES_MODE);
  M5.Lcd.clear();
}

uint8_t mode = 0;

void loop()
{
  M5.update();
  if (M5.BtnA.wasClicked()){
    mode = 1 - mode;
  }
  uint16_t ir, red;
  sensor.update();
  while (sensor.getRawValues(&ir, &red)) {
    uint16_t min_red = 65535, min_ir = 65535, max_red = 0, max_ir = 0;
    val_red[px] = red; val_ir[px] = ir;
    for (uint8_t x = 0; x < X; x++){
      if (val_red[x] > max_red) max_red = val_red[x];
      if (val_red[x] < min_red) min_red = val_red[x];
      if (val_ir[x] > max_ir) max_ir = val_ir[x];
      if (val_ir[x] < min_ir) min_ir = val_ir[x];
    }
//    printf("%d %d / %d\n", max_red, min_red, max_red - min_red);
/*
    float mag_red, mag_ir;
    if (max_red == min_red) mag_red = 1;
    else mag_red = (float)(max_red - min_red) / (float)Y;
    if (max_ir == min_ir) mag_ir = 1;
    else mag_ir = (float)(max_ir - min_ir) / (float)Y;
*/
//    printf("%d : %d - %d / %d / %f / %d\n", px, max_red, min_red, red, mag_red, (uint8_t)((red - min_red) / mag_red));
//    printf("max_red: %d, min_red: %d, mag_red: %f\n", max_red, min_red, mag_red);
    uint8_t dx = px;
    uint8_t y_red, y_ir;
//    M5.Lcd.clear();
    for (uint8_t x = 0; x < X; x++){
//      y_red = (uint8_t)((val_red[dx] - min_red) / mag_red);
//      y_ir = (uint8_t)((val_ir[dx] - min_ir) / mag_ir);
      y_red = (val_red[dx] - min_red) / 4;
//      y_ir = (uint8_t)((val_ir[dx] - min_ir) / mag_ir);
      if (y_red > Y) y_red = Y;
//      if (y_ir > Y) y_ir = Y;
//      printf("%d %d %d\n", x, y_red, y_ir);
      M5.Lcd.drawFastHLine(0, x, Y, BLACK);
      M5.Lcd.drawPixel(Y - y_red, x, RED);
//      M5.Lcd.drawPixel(Y - y_ir, x, GREEN);
      dx = (dx + 1) % X;
    }
    px = (px + 1) % X;
  }
}