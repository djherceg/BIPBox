// BIP2024 UNS, UPT, USZ

// AllDeviceTest
// version 0.1.0
// date 7.1.2024.
// The purpose of this program is to demonstrate and test all devices connected to the BIPBOX ESP32


// ESP32Servo library Copyright (c) 2017 John K. Bennett
// https://github.com/jkb-git/ESP32Servo
//#include <ESP32Servo.h>
//Servo servo1;

// SimpleDHT library by Winlin
// https://github.com/winlinvip/SimpleDHT
#include <SimpleDHT.h>

//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

// DHT22
SimpleDHT22 dht22(DHT);

// LCD I2C 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F or 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(115200);
  Serial.println("BIPBOX v0.1");
  lcd.init();  // initialize the lcd
    // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0, 2);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(2, 3);
  lcd.print("Power By Ec-yuan!");

  // BMI160 INIT
  //init the hardware bmin160
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("BMI160 reset false");
    while (1)
      ;
  }

  //set and init the bmi160 i2c address
  if (bmi160.I2cInit(bmi_i2c_addr) != BMI160_OK) {
    Serial.println("BMI160 init false");
    while (1)
      ;
  }
  // END BMI160 INIT

  // Servo INIT
  //servo1.attach(SERVO);

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

int a1, a2;
int b1, b2, oldb1, oldb2;
int p;
bool k1, k2, led1, led2, led3;

void loop() {
  // ---- KEYS ----
  b1 = !digitalRead(KEY1);
  b2 = !digitalRead(KEY2);

  if (b1 && (b1 != oldb1))
    k1 = true;
  else
    k1 = false;
  if (b2 && (b2 != oldb2))
    k2 = true;
  else
    k2 = false;
  oldb1 = b1;
  oldb2 = b2;

  //Serial.print(k1);
  //Serial.println(k2);

  if (k1) {
    led1 = !led1;
  }

  if (k2) {
    led2 = !led2;
    //digitalWrite(LED2, led2);
  }

  // ---- PIR ----
  p = digitalRead(PIR);
  lcd.setCursor(15, 0);
  if (p != LOW)
    lcd.print("*");
  else
    lcd.print("_");

  a1 = analogRead(34);
  a2 = analogRead(POTENTIOMETER);
  lcd.setCursor(0, 0);
  lcd.print(a1);
  lcd.print("   ");
  lcd.setCursor(0, 1);
  lcd.print(a2);
  lcd.print("   ");


  led3 = !led3;
  //digitalWrite(LED3, led3);


  if (led1)
    analogWrite(LED1, a2 / 16);
  else
    analogWrite(LED1, 0);

  if (led2)
    analogWrite(LED2, a2 / 16);
  else
    analogWrite(LED2, 0);

  if (led3)
    analogWrite(LED3, a2 / 16);
  else
    analogWrite(LED3, 0);

  dhtloop();
  bmiLoop();
  servoLoop();
  delay(300);
}

void servoLoop() {
    int servoPosition = map(analogRead(POTENTIOMETER), 0, 4096, 0, 180);
    //servo1.write(servoPosition);
    //Serial.println(servoPosition);
}

void dhtloop() {
  static int mils = millis();

  if (millis() - mils < 5000)
    return;
  mils = millis();


  // start working...
  //Serial.println("=================================");
  //Serial.println("Sample DHT22...");

  // read without samples.
  // @remark We use read2 to get a float data, such as 10.1*C
  //    if user doesn't care about the accurate data, use read to get a byte data, such as 10*C.
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    //delay(2000);
    return;
  }

  Serial.print("Sample OK: ");
  Serial.print((float)temperature);
  Serial.print(" *C, ");
  Serial.print((float)humidity);
  Serial.println(" RH%");

  lcd.setCursor(8, 0);
  lcd.print((float)temperature);
  lcd.setCursor(8, 1);
  lcd.print((float)humidity);

  // DHT22 sampling rate is 0.5HZ.
  //delay(2500);
}


void bmiLoop() {
  static int mils = millis();

  int i = 0;
  float ax, ay, az, d;
  int rslt;
  int16_t accelGyro[6] = { 0 };

  //get both accel and gyro data from bmi160
  //parameter accelGyro is the pointer to store the data
  rslt = bmi160.getAccelGyroData(accelGyro);
  if (rslt == 0) {
    // calculate the modulus of the acceleration vector
    // sound the buzzer if it is greater than 2.0
    // try shaking the device!
    ax = accelGyro[3] / 16384.0;
    ay = accelGyro[4] / 16384.0;
    az = accelGyro[5] / 16384.0;
    d = sqrt(ax * ax + ay * ay + az * az);
    if (d > 2.0) {
      digitalWrite(BUZZER, HIGH);
      delay(200);
      digitalWrite(BUZZER, LOW);
    }

    if (millis() - mils > 250) {
      mils = millis();
      Serial.print("BMI160 Gyro and Accel: ");
      for (i = 0; i < 6; i++) {
        if (i < 3) {
          //the first three are gyro data
          Serial.print(accelGyro[i] * 3.14 / 180.0);
          Serial.print("\t");
        } else {
          //the following three data are accel data
          Serial.print(accelGyro[i] / 16384.0);
          Serial.print("\t");
        }
      }
      Serial.print(" |d|: ");
      Serial.print(d);
      Serial.println();
    }
  } else {
    Serial.println("BMI160 err");
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