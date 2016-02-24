/*
  Сервисный робот на Arduino
  Created by Rostislav Varzar
*/

#include <Servo.h>

#define LINESENSOR1 2
#define LINESENSOR2 3

#define IRSENSOR1 A4

#define US1_trigPin A0
#define US1_echoPin A1
#define US2_trigPin A2
#define US2_echoPin A3
#define minimumRange 0
#define maximumRange 200

//#define COLOR_S0PIN 2
//#define COLOR_S1PIN 3
#define COLOR_S2PIN 8
#define COLOR_S3PIN 11
#define COLOR_OUTPIN 12

#define PWMA 5
#define DIRA 4
#define PWMB 6
#define DIRB 7

#define SERVO1_PWM 9
#define SERVO2_PWM 10

Servo servo_1;
Servo servo_2;

void setup()
{
  // Инициализация последовательного порта
  Serial.begin(9600);
  // Инициализация выводов для управления датчиком цвета
  //pinMode(COLOR_S0PIN, OUTPUT);
  //pinMode(COLOR_S1PIN, OUTPUT);
  pinMode(COLOR_S2PIN, OUTPUT);
  pinMode(COLOR_S3PIN, OUTPUT);
  pinMode(COLOR_OUTPIN, INPUT);
  //digitalWrite(COLOR_S0PIN, LOW);
  //digitalWrite(COLOR_S1PIN, HIGH);
  // Инициализация выводов для работы с УЗ датчиком
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);
  // Инициализация выходов для управления моторами
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  // Инициализация портов для управления сервомоторами
  servo_1.attach(SERVO1_PWM);
  servo_2.attach(SERVO2_PWM);
}

void loop()
{


}

// УЗ датчик 1
long readUS1_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US1_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_trigPin, LOW);
  duration = pulseIn(US1_echoPin, HIGH);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 2
long readUS2_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US2_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US2_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_trigPin, LOW);
  duration = pulseIn(US2_echoPin, HIGH);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// Чтение интенсивности красного цвета
int readRED()
{
  digitalWrite(COLOR_S2PIN, LOW);
  digitalWrite(COLOR_S3PIN, LOW);
  return pulseIn(COLOR_OUTPIN, digitalRead(COLOR_OUTPIN) == HIGH ? LOW : HIGH);
}

// Чтение интенсивности зеленого цвета
int readGREEN()
{
  digitalWrite(COLOR_S2PIN, HIGH);
  digitalWrite(COLOR_S3PIN, HIGH);
  return pulseIn(COLOR_OUTPIN, digitalRead(COLOR_OUTPIN) == HIGH ? LOW : HIGH);
}

// Чтение интенсивности синего цвета
int readBLUE()
{
  digitalWrite(COLOR_S2PIN, LOW);
  digitalWrite(COLOR_S3PIN, HIGH);
  return pulseIn(COLOR_OUTPIN, digitalRead(COLOR_OUTPIN) == HIGH ? LOW : HIGH);
}

// Вычисление цвета
String getColor()
{
  String MY_COLOR = "";
  float clRed = 1000 / (float)readRED();
  float clGreen = 1000 / (float)readGREEN();
  float clBlue = 1000 / (float)readBLUE();
  if ((clRed > 0.600) && (clRed < 0.900) && (clGreen > 0.900) && (clGreen < 1.200) && (clBlue > 3.900) && (clBlue < 4.200))
  {
    MY_COLOR = "BLUE";
  }
  else if ((clRed > 0.700) && (clRed < 1.000) && (clGreen > 1.000) && (clGreen < 1.300) && (clBlue > 3.000) && (clBlue < 3.300))
  {
    MY_COLOR = "GREEN";
  }
  else if ((clRed > 2.900) && (clRed < 3.200) && (clGreen > 0.600) && (clGreen < 0.900) && (clBlue > 4.450) && (clBlue < 4.750))
  {
    MY_COLOR = "RED";
  }
  else if ((clRed > 4.050) && (clRed < 4.350) && (clGreen > 2.500) && (clGreen < 2.800) && (clBlue > 8.550) && (clBlue < 8.850))
  {
    MY_COLOR = "YELLOW";
  }
  else
  {
    MY_COLOR = "UNDEFINED";
  }
  return MY_COLOR;
}

// Мощность мотора "A" от -100% до +100% (от знака зависит направление вращения)
void motorA_setpower(int pwr)
{
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRA, LOW);
  }
  else
  {
    digitalWrite(DIRA, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMA, pwmvalue);
}

// Мощность мотора "B" от -100% до +100% (от знака зависит направление вращения)
void motorB_setpower(int pwr)
{
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRB, LOW);
  }
  else
  {
    digitalWrite(DIRB, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMB, pwmvalue);
}

