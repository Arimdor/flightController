#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <AsyncUDP.h>

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

const char *ssid = "Casa";
const char *password = "987654321R";
unsigned int check_wifi = 30000;

IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

AsyncUDP udp;

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

  setupWifi();
  handleUDP();
}

void loop()
{
  checkWifi();
  readSerial();
}

void doCommand()
{
  ledcWrite(CH_MOTOR, power);
  //ledcWrite(CH_SERVOY, y);
  servoY.write(y);
  //ledcWrite(CH_SERVOX, x);
}

void convertData()
{
  String temp = "";
  int counter = 0;
  for (int i = 0; i <= data.length(); i++)
  {
    if (data.substring(i, i + 1) != "H" || data.substring(i, i + 1) != "E" || data.substring(i, i + 1) != "\n")
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
        //Serial.println(temp);
      }
    }
  }
  Serial.println(y);
  doCommand();
}

void readSerial()
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
    }
    else
    {
      data += rawData;
    }
  }
}

void checkWifi()
{
  if ((WiFi.status() != WL_CONNECTED) && (millis() > check_wifi))
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    check_wifi = millis() + 30000;
  }
}

void setupWifi()
{
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    while (true)
    {
      Serial.println("WiFi Failed");
      delay(1000);
    }
  }
  Serial.println("WiFi Connected");
}

void handleUDP()
{
  if (udp.listen(6969))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      //Serial.write(packet.data(), packet.length());
      data = String((char *)packet.data());
      convertData();
      //Serial.println(data);
    });
  }
}
