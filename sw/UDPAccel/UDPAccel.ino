// BIPBox - Djordje Herceg

// UDPAccel test
// version 0.0.1
// date 26.3.2026.
// The purpose of this program is to demonstrate and test sending accelerometer data over UDP

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "your-ssid";
const char* password = "your-password";

// UDP server details
const char* udpAddress = "192.168.0.255";  // Broadcast address or specific server IP
const int udpPort = 3333;

// UDP instance
WiFiUDP udp;

#define KEY1 39
#define KEY2 36
#define PIR 23
#define DHT 19
#define PHOTORESISTOR 34
#define POTENTIOMETER 35
#define LED1 16
#define LED2 17
#define LED3 18
#define BUZZER 33
#define SERVO 32

// ArduinoJSON
#include <ArduinoJson.h>

//BMI160
/*!
 * @file accelGyro.ino
 * @brief I2C addr:
 * @n  0x68: connect SDIO pin of the BMI160 to GND which means the default I2C address
 * @n  0x69: set I2C address by parameter
 * @n Through the example, you can get the sensor data by using getSensorData:
 * @n get acell by paremeter onlyAccel;
 * @n get gyro by paremeter onlyGyro;
 * @n get both acell and gyro by paremeter bothAccelGyro.
 * @n With the rotation of the sensor, data changes are visible.
 * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author  DFRobot_haoJ(hao.jiang@dfrobot.com)
 * @version V1.0
 * @date 2017-12-01
 * @url https://github.com/DFRobot/DFRobot_BMI160
 */

// DFRobot_BMI160 library by DFRobot
//  https://github.com/DFRobot/DFRobot_BMI160
#include <DFRobot_BMI160.h>
DFRobot_BMI160 bmi160;
const int8_t bmi_i2c_addr = 0x69;

// LCD I2C 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F or 0x27 for a 16 chars and 2 line display

// Variables
int a1, a2;
int b1, b2, oldb1, oldb2;
int p;
bool k1, k2, led1, led2, led3;
float ax, ay, az, gx, gy, gz;
bool bmiOK = false;
unsigned long mils = millis(), oldMils, delta, interval = 500;


void setup() {
  Serial.begin(115200);
  Serial.println("UDPAccel v0.0.1");
  lcd.init();  // initialize the lcd
    // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("UDPAccel v0.0.1");

  // BMI160 INIT
  //init the hardware bmin160
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("BMI160 reset false");
    lcd.print("BMI reset false");
    while (1)
      ;
  }

  //set and init the bmi160 i2c address
  if (bmi160.I2cInit(bmi_i2c_addr) != BMI160_OK) {
    Serial.println("BMI160 init false");
    lcd.print("BMI init false");
    while (1)
      ;
  }

  lcd.print("BMI OK");
  // END BMI160 INIT

  // Set up UDP
  // Connect to WiFi
  connectToWiFi();

  // Initialize UDP
  udp.begin(udpPort);


  // Set up pins and BEEP
  pinMode(PHOTORESISTOR, INPUT);
  pinMode(POTENTIOMETER, INPUT);
  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  pinMode(PIR, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  digitalWrite(2, LOW);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");

  // Disconnect any previous connections
  WiFi.disconnect(true);

  // Start WiFi connection
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}



void loop() {
  bmiLoop();

  if (millis() - mils > interval) {
    delta = mils;
    mils = millis();

    char* output;
    size_t outputCapacity = 150;

    JsonDocument doc;

    doc["sensor"] = "accelerometer";

    JsonArray gyro = doc["gyro"].to<JsonArray>();
    gyro.add(gx);
    gyro.add(gy);
    gyro.add(gz);

    JsonArray accel = doc["accel"].to<JsonArray>();
    accel.add(ax);
    accel.add(ay);
    accel.add(az);

    doc.shrinkToFit();  // optional

    serializeJson(doc, output, outputCapacity);

    // Send data over UDP
    udp.beginPacket(udpAddress, udpPort);
    udp.print(output);
    udp.endPacket();
  }
}



void bmiLoop() {
  int rslt;
  int16_t accelGyro[6] = { 0 };

  //get both accel and gyro data from bmi160
  bmiOK = false;
  rslt = bmi160.getAccelGyroData(accelGyro);
  if (rslt == 0) {
    // GYRO data
    gx = accelGyro[0] * 3.14 / 180.0;
    gy = accelGyro[1] * 3.14 / 180.0;
    gz = accelGyro[2] * 3.14 / 180.0;
    // ACCEL data
    ax = accelGyro[3] / 16384.0;
    ay = accelGyro[4] / 16384.0;
    az = accelGyro[5] / 16384.0;
    // Data read OK
    bmiOK = true;
  } else {
    // not OK
  }
  /*
   * //only read accel data from bmi160
   * int16_t onlyAccel[3]={0};
   * bmi160.getAccelData(onlyAccel);
   */

  /*
   * ////only read gyro data from bmi160
   * int16_t onlyGyro[3]={0};
   * bmi160.getGyroData(onlyGyro);
   */
}