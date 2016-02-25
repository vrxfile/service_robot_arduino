/*
  Сервисный робот на Arduino
  Created by Rostislav Varzar
*/

#include <Servo.h>

#define LINESENSOR1 2
#define LINESENSOR2 3

#define IRSENSOR1 A0

#define US1_trigPin A1
#define US1_echoPin A2
#define US2_trigPin A3
#define US2_echoPin A4
#define minimumRange 0
#define maximumRange 200

//#define COLOR_S0PIN 2
//#define COLOR_S1PIN 3
#define COLOR_S2PIN 8
#define COLOR_S3PIN 11
#define COLOR_OUTPIN 12
float r0, g0, b0, c0;

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
  // Нахождениен коэффициентов для исключения сперктральной характеристики окружающего света
  r0 = 1000 / (float)readRED();
  g0 = 1000 / (float)readGREEN();
  b0 = 1000 / (float)readBLUE();
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
  servo_1.write(60);
  servo_2.write(30);
}

void loop()
{
  // Ожидание цветной карточки
  String card_color = "UNDEFINED";
  while (card_color == "UNDEFINED")
  {
    card_color = getColor();
    Serial.println("Color: " + card_color);
    delay(1000);
  }
  Serial.println("Color detected! Color: " + card_color);
  // Выбор зоны для выполнения задания
  if (card_color == "RED")
  {
    rotateLeft();
  }
  else if (card_color == "GREEN")
  {

  }
  else if (card_color == "BLUE")
  {
    rotateRight();
  }



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
  float clRed = 1000 / (float)readRED() - r0;
  float clGreen = 1000 / (float)readGREEN() - g0;
  float clBlue = 1000 / (float)readBLUE() - b0;
  if ((clRed > -0.400) && (clRed < 0.400) && (clGreen > 0.200) && (clGreen < 1.000) && (clBlue > 1.300) && (clBlue < 2.100))
  {
    MY_COLOR = "BLUE";
  }
  else if ((clRed > -0.300) && (clRed < 0.500) && (clGreen > 0.300) && (clGreen < 1.100) && (clBlue > 0.200) && (clBlue < 1.000))
  {
    MY_COLOR = "GREEN";
  }
  else if ((clRed > 1.900) && (clRed < 2.700) && (clGreen > -0.100) && (clGreen < 0.700) && (clBlue > 0.200) && (clBlue < 1.000))
  {
    MY_COLOR = "RED";
  }
  else if ((clRed > 2.900) && (clRed < 3.700) && (clGreen > 1.800) && (clGreen < 2.600) && (clBlue > 0.600) && (clBlue < 1.400))
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

// Поворот влево на 90 градусов
void rotateLeft()
{
  motorA_setpower(-100);
  motorB_setpower(-100);
  delay(500);
  motorA_setpower(0);
  motorB_setpower(0);
}

// Поворот вправо на 90 градусов
void rotateRight()
{
  motorA_setpower(100);
  motorB_setpower(100);
  delay(500);
  motorA_setpower(0);
  motorB_setpower(0);
}

