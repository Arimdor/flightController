#include <Arduino.h>
#include <ESP32Servo.h>
#include <esp32-hal-ledc.h>

String data = "";

float x = 0.0;
float y = 0.0;
float power = 0.0;

const int CH_MOTOR = 4;
const int LED = 19;
const int MOTOR = 18;

//const int CH_SERVOY = 2;
const int SERVOY = 21;

const int CH_SERVOX = 3;
const int SERVOX = 23;

Servo servoY;

void setup()
{
  ledcSetup(CH_MOTOR, 60, 10);
  ledcAttachPin(MOTOR, CH_MOTOR);
  ledcAttachPin(LED, CH_MOTOR);

  //ledcSetup(CH_SERVOY, 50, 12);
  //ledcAttachPin(SERVOY, CH_SERVOY);

  //ledcSetup(CH_SERVOX, 50, 12);
  //ledcAttachPin(SERVOX, CH_SERVOX);

  servoY.setPeriodHertz(60);
  servoY.attach(SERVOY, 500, 2400);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char rawData = Serial.read();
    if (rawData == 'H')
    {
      data = "";
    }
    else if (rawData == 'E')
    {
      convertData();
      Serial.println(data);

      ledcWrite(CH_MOTOR, power);
      //ledcWrite(CH_SERVOY, y);
      servoY.write(y);
      //ledcWrite(CH_SERVOX, x);
    }
    else
    {
      data += rawData;
    }
  }
}

void convertData()
{
  String temp = "";
  int counter = 0;
  for (int i = 0; i <= data.length(); i++)
  {
    if (data.substring(i, i + 1) != "H" || data.substring(i, i + 1) != "E")
    {
      if (data.substring(i, i + 1) == "|")
      {
        switch (counter)
        {
        case 0:
          power = temp.toFloat();
          break;
        case 1:
          x = temp.toFloat();
          break;
        case 2:
          y = temp.toFloat();
          break;
        }
        temp = "";
        counter++;
      }
      else
      {
        temp += data.substring(i, i + 1);
      }
    }
  }
}
