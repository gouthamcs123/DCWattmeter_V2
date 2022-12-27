#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int analogInput1 = 7;//For measuring voltage
int analogInput2 = 3; //For measuring current

float vout = 0.0;//output from voltage divider circuit.
float voltage = 0.0;//Actual value of voltage
float R1 = 220000.0; // resistance of R1 (220K)
float R2 = 32000.0; // resistance of R2 (32K)

double current = 0; //Actual value of current
double rawvoltage = 0; //output from current sensor
float power = 0; //For calculating power
//---------------------------------------------------
#define windowSize 10
int window1[windowSize] = {0};
int window2[windowSize] = {0};


//---------------------------------------------------

void setup()
{
  //Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(1,1);
  display.setTextSize(3);
  display.print("Hi");
  display.setCursor(1,30);
  display.setTextSize(1);
  display.print("1.5V-30V/0-10A");
  display.display();
  delay(1000);
}

void push1(int val)
{
  int i = 0;
  for (i = 0; i < windowSize; i++)
  {
    window1[i - 1] = window1[i];
  }
  window1[windowSize - 1] = val;

}
void push2(int val)
{
  int i = 0;
  for (i = 0; i < windowSize; i++)
  {
    window2[i - 1] = window2[i];
  }
  window2[windowSize - 1] = val;

}
int avg1()
{
  long sum = 0;
  int i = 0;
  for (i = 0; i < windowSize; i++)
  {
    sum += window1[i];
  }
  return sum / windowSize;
}
int avg2()
{
  long sum = 0;
  int i = 0;
  for (i = 0; i < windowSize; i++)
  {
    sum += window2[i];
  }
  return sum / windowSize;
}


void readVoltage()
{
  //    int voltSample = 0;
  //    voltSample = analogRead(analogInput1);
  //    delay(100);
  //    push1(voltSample);
  //    int avgvoltSample = avg1();
  //    vout = (avgvoltSample * 5.000) / 1024;
  //    voltage = vout / (R2 / (R1 + R2));
  //    Serial.println(vout);
  //    Serial.println(avgvoltSample);
  for (int j = 0; j < 1000; j++)
  {
    vout = (vout + (analogRead(analogInput1) * 4.40) / 1024);
    delay(0.8);
  }
  vout = vout / 1000;
  voltage = vout / (R2 / (R1 + R2));
  //voltage = voltage - 0.8; // For calibration,you can change this value according to your voltage deviation.
  if (voltage < 0)voltage = 0;
  Serial.println(vout);

}

void readCurrent()
{
  int currentSample = 0;
  currentSample = analogRead(analogInput2);
  delay(10);
  push2(currentSample);
  int avgcurrentSample = avg2();
  rawvoltage = (avgcurrentSample * 5.000) / 1024; // (5 V / 1024 (Analog) = 0.0049) which converter Measured analog input voltage to 5 V Range
  current = (rawvoltage - 2.500) / 0.100; // Sensed voltage is converted to current
  if (current < 0)current = 0;
  Serial.println(avgcurrentSample);
  Serial.println(rawvoltage, 3);


}

void powerCalc()
{
  power = voltage * current; //For calculating power.
}

void oledDisplay()
{
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setCursor(10, 1);
  display.setTextSize(2);
  display.print(voltage, 2);
  display.setCursor(100, 0);
  display.print("V");

  display.setCursor(10, 25);
  display.setTextSize(2);
  display.print(current, 2);
  display.setCursor(100, 25);
  display.print("A");

  display.setCursor(10, 50 );
  display.setTextSize(2);
  display.print(power, 2);
  display.setCursor(100, 50);
  display.print("W");

  display.display();
}


void loop()
{
  readCurrent();
  delay(10);
  readVoltage();
  delay(10);
  powerCalc();
  oledDisplay();

}
