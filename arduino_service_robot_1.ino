/*
  Сервисный робот на Arduino
  Created by Rostislav Varzar
*/

#include <Servo.h>
#include <SoftwareSerial.h>

// Определения для портов
#define LINESENSOR1 2
#define LINESENSOR2 3
#define CRASHSENSOR1 A0

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

// Параметры зон
#define DIST1 50
#define DIST2 100
#define DIST3 150

// Параметры ПИД регулятора
#define KPID 10.0

// Программный UART для Bluetooth
SoftwareSerial BT(A5, 13);

// Переменные для управления роботом
char command = 'S';
char prevCommand = 'A';
int velocity = 0;
unsigned long timer0 = 2000;
unsigned long timer1 = 0;

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
  // Начальное положение сервомоторов
  servo_1.write(90); delay(2000);
  // Тренировка хвата
  servo_2.write(180); delay(2000);
  servo_2.write(0); delay(2000);
}


void loop()
{

  /*
    // Тестирование датчиков
    while (true)
    {
    Serial.print(readUS1_distance());
    Serial.print("\t\t");
    delay(50);
    Serial.print(readUS2_distance());
    Serial.print("\t");
    Serial.print(getColor());
    Serial.print("\t");
    Serial.print(digitalRead(CRASHSENSOR1));
    Serial.print("\t");
    Serial.println("");
    delay(50);
    }
  */

  // Ожидание цветной карточки или команды Bluetooth
  String card_color = "UNDEFINED";
  while (card_color == "UNDEFINED")
  {
    // Цвет карточки
    card_color = getColor();
    if (digitalRead(CRASHSENSOR1) == 0)
    {
      card_color = "BUTTON";
    }
    Serial.println("Color: " + card_color);
    delay(100);
  }
  if (card_color == "BUTTON")
  {
    for (int ddd = 0; ddd < 100; ddd++)
    {
      if (digitalRead(CRASHSENSOR1) == 1)
      {
        break;
      }
      delay(10);
    }
  }
  Serial.println("Color detected! Color: " + card_color);

  // Работа по Bluetooth
  if (card_color == "BUTTON")
  {
    int srv1 = 90;
    int srv2 = 0;
    // Инициализация программного последовательного порта для Bluetooth
    BT.begin(9600);
    while (digitalRead(CRASHSENSOR1))
    {
      // Bluetooth
      if (BT.available() > 0) {
        timer1 = millis();
        prevCommand = command;
        command = BT.read();
        if (command != prevCommand) {
          Serial.println(command);
          switch (command) {
            case 'F':
              motorA_setpower(velocity, false);
              motorB_setpower(velocity, true);
              break;
            case 'B':
              motorA_setpower(-velocity, false);
              motorB_setpower(-velocity, true);
              break;
            case 'L':
              motorA_setpower(-velocity, false);
              motorB_setpower(velocity, true);
              break;
            case 'R':
              motorA_setpower(velocity, false);
              motorB_setpower(-velocity, true);
              break;
            case 'S':
              motorA_setpower(0, false);
              motorB_setpower(0, false);
              break;
            case 'I':  //FR
              //yellowCar.ForwardRight_4W(velocity);
              motorA_setpower(velocity, false);
              motorB_setpower(velocity / 2, true);
              break;
            case 'J':  //BR
              motorA_setpower(-velocity, false);
              motorB_setpower(-velocity / 2, true);
              break;
            case 'G':  //FL
              motorA_setpower(velocity / 2, false);
              motorB_setpower(velocity, true);
              break;
            case 'H':  //BL
              motorA_setpower(-velocity / 2, false);
              motorB_setpower(-velocity, true);
              break;
            case 'W':
              srv1 = srv1 - 5;
              if (srv1 <= 0)
              {
                srv1 = 0;
              }
              servo_1.write(srv1);
              break;
            case 'w':
              srv1 = srv1 - 5;
              if (srv1 <= 0)
              {
                srv1 = 0;
              }
              servo_1.write(srv1);
              break;
            case 'U':
              srv1 = srv1 + 5;
              if (srv1 >= 180)
              {
                srv1 = 180;
              }
              servo_1.write(srv1);
              break;
            case 'u':
              srv1 = srv1 + 5;
              if (srv1 >= 180)
              {
                srv1 = 180;
              }
              servo_1.write(srv1);
              break;
            case 'V':
              srv2 = srv2 - 5;
              if (srv2 <= 0)
              {
                srv2 = 0;
              }
              servo_2.write(srv2);
              break;
            case 'v':
              srv2 = srv2 - 5;
              if (srv2 <= 0)
              {
                srv2 = 0;
              }
              servo_2.write(srv2);
              break;
            case 'X':
              srv2 = srv2 + 5;
              if (srv2 >= 180)
              {
                srv2 = 180;
              }
              servo_2.write(srv2);
              break;
            case 'x':
              srv2 = srv2 + 5;
              if (srv2 >= 180)
              {
                srv2 = 180;
              }
              servo_2.write(srv2);
              break;
            case 'D':  //Everything OFF
              motorA_setpower(0, false);
              motorB_setpower(0, false);
              break;
            default:  //Get velocity
              if (command == 'q') {
                velocity = 100;  //Full velocity
              }
              else {
                //Chars '0' - '9' have an integer equivalence of 48 - 57, accordingly.
                if ((command >= 48) && (command <= 57)) {
                  //Subtracting 48 changes the range from 48-57 to 0-9.
                  //Multiplying by 25 changes the range from 0-9 to 0-225.
                  velocity = (command - 48) * 10;
                  Serial.println(velocity);
                }
              }
          }
        }
      }
      else {
        timer0 = millis();  //Get the current time (millis since execution started).
        //Check if it has been 500ms since we received last command.
        if ((timer0 - timer1) > 500) {
          //More tan 500ms have passed since last command received, car is out of range.
          //Therefore stop the car and turn lights off.
          motorA_setpower(0, false);
          motorB_setpower(0, false);
        }
      }
    }
  }
  BT.end();
  delay(500);

  // Выбор зоны для выполнения задания
  servo_1.write(45);
  if (card_color == "RED")
  {
    // Поворачиваем влево
    rotateLeft(); delay(500);
    // Чуть проезжаем
    goRobot(DIST1, 15, 200, 75, 2.0);  delay(500);
    // Поворачиваем вправо
    rotateRight(); delay(500);
    // Доезжаем до кубика
    goRobot(7, DIST1, 50, 75, 2.0);
    goRobot(7, DIST1, 50, 75, 5.0);
    goRobot(7, DIST1, 300, 75, 10.0);
    delay(500);
    // Хватаем кубик (если не схватили, циклически повтоярем хватание)
    for (int iter = 0; iter < 15; iter++)
    {
      long sss1 = readUS1_distance();
      servo_2.write(180); delay(250);
      slowServo(servo_1, 45, 150, 2500); delay(250);
      servo_2.write(0); delay(250);
      slowServo(servo_1, 150, 45, 2500); delay(250);
      long sss2 = readUS1_distance();
      // Если схватили кубик (расстояние до следующего изменилось), то прерываем цикл
      if (abs(sss2 - sss1) > 5)
      {
        break;
      }
      // Если не схватили кубик, то крутимся на несколько градусов
      if (iter < 7)
      {
        motorA_setpower(100, true);
        motorB_setpower(-100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
      else
      {
        motorA_setpower(-100, true);
        motorB_setpower(100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
    }
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов (с приблизительным доездом до центра)
    rotateRight();  delay(250);
    motorA_setpower(-100, true);
    motorB_setpower(-100, false);
    delay(1000);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(250);
    rotateRight();  delay(250);
    // Возвращаемся
    goRobot(15, DIST2, 300, 75, 2.0);  delay(500);
    // Отпускаем кубик
    slowServo(servo_1, 45, 150, 2500); delay(250);
    servo_2.write(180); delay(250);
    slowServo(servo_1, 150, 45, 2500); delay(250);
    servo_2.write(0); delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов
    rotateLeft();  delay(250);
    rotateLeft();  delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(1000);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Тренировка хвата
    servo_2.write(180); delay(2000);
    servo_2.write(0); delay(2000);
  }
  else if (card_color == "GREEN")
  {
    // Доезжаем до кубика
    goRobot(7, DIST2, 50, 75, 2.0);
    goRobot(7, DIST2, 50, 75, 5.0);
    goRobot(7, DIST2, 300, 75, 10.0);
    delay(500);
    // Хватаем кубик (если не схватили, циклически повтоярем хватание)
    for (int iter = 0; iter < 15; iter++)
    {
      long sss1 = readUS1_distance();
      servo_2.write(180); delay(250);
      slowServo(servo_1, 45, 150, 2500); delay(250);
      servo_2.write(0); delay(250);
      slowServo(servo_1, 150, 45, 2500); delay(250);
      long sss2 = readUS1_distance();
      // Если схватили кубик (расстояние до следующего изменилось), то прерываем цикл
      if (abs(sss2 - sss1) > 5)
      {
        break;
      }
      // Если не схватили кубик, то крутимся на несколько градусов
      if (iter < 7)
      {
        motorA_setpower(100, true);
        motorB_setpower(-100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
      else
      {
        motorA_setpower(-100, true);
        motorB_setpower(100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
    }
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов
    rotateLeft();  delay(250);
    rotateLeft();  delay(250);
    // Возвращаемся
    goRobot(15, DIST2, 300, 75, 2.0);  delay(500);
    // Отпускаем кубик
    slowServo(servo_1, 45, 150, 2500); delay(250);
    servo_2.write(180); delay(250);
    slowServo(servo_1, 150, 45, 2500); delay(250);
    servo_2.write(0); delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов
    rotateRight();  delay(250);
    rotateRight();  delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(1000);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Тренировка хвата
    servo_2.write(180); delay(2000);
    servo_2.write(0); delay(2000);
  }
  else if (card_color == "BLUE")
  {
    // Поворачиваем вправо
    rotateRight(); delay(500);
    // Чуть проезжаем
    motorA_setpower(-100, true);
    motorB_setpower(-100, false);
    delay(1500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(250);
    // Поворачиваем влево
    rotateLeft(); delay(500);
    // Доезжаем до кубика
    goRobot(7, DIST3, 50, 75, 2.0);
    goRobot(7, DIST3, 50, 75, 5.0);
    goRobot(7, DIST3, 300, 75, 10.0);
    delay(500);
    // Хватаем кубик (если не схватили, циклически повтоярем хватание)
    for (int iter = 0; iter < 15; iter++)
    {
      long sss1 = readUS1_distance();
      servo_2.write(180); delay(250);
      slowServo(servo_1, 45, 150, 2500); delay(250);
      servo_2.write(0); delay(250);
      slowServo(servo_1, 150, 45, 2500); delay(250);
      long sss2 = readUS1_distance();
      // Если схватили кубик (расстояние до следующего изменилось), то прерываем цикл
      if (abs(sss2 - sss1) > 5)
      {
        break;
      }
      // Если не схватили кубик, то крутимся на несколько градусов
      if (iter < 7)
      {
        motorA_setpower(100, true);
        motorB_setpower(-100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
      else
      {
        motorA_setpower(-100, true);
        motorB_setpower(100, false);
        delay(10);
        motorA_setpower(0, false);
        motorB_setpower(0, false);
        delay(500);
      }
    }
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов (с приблизительным доездом до центра)
    rotateLeft();  delay(250);
    motorA_setpower(-100, true);
    motorB_setpower(-100, false);
    delay(1000);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(250);
    rotateLeft();  delay(250);
    // Возвращаемся
    goRobot(15, DIST2, 300, 75, 2.0);  delay(500);
    // Отпускаем кубик
    slowServo(servo_1, 45, 150, 2500); delay(250);
    servo_2.write(180); delay(250);
    slowServo(servo_1, 150, 45, 2500); delay(250);
    servo_2.write(0); delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Вращаем робота на 180 градусов
    rotateRight();  delay(250);
    rotateRight();  delay(250);
    // Отъезжаем чуть назад
    motorA_setpower(100, true);
    motorB_setpower(100, false);
    delay(1000);
    motorA_setpower(0, false);
    motorB_setpower(0, false);
    delay(500);
    // Тренировка хвата
    servo_2.write(180); delay(2000);
    servo_2.write(0); delay(2000);
  }
  delay(500);
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
void motorA_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
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
void motorB_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
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
  motorA_setpower(100, true);
  motorB_setpower(-100, false);
  delay(450);
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

// Поворот вправо на 90 градусов
void rotateRight()
{
  motorA_setpower(-100, true);
  motorB_setpower(100, false);
  delay(450);
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

// Движения робота вдоль стены на расстоянии dst2, до препятствия с расстоянием не меньше dst1, таймаут выполнения работы tmout (количество интераций цикла)
// Коэффициент регуклирования k, скорость робота spd
void goRobot(long dst1, long dst2, long tmout, int spd, float k)
{
  long timecount = 0;
  long d1 = 0;
  long d2 = 0;
  long oldd2 = 0;
  // Доезжаем до кубика
  while (timecount < tmout)
  {
    d1 = readUS1_distance();
    delay(15);
    float u = 0;
    if (d2 != (-1) && (abs(oldd2 - d2) < 10))
    {
      u = float(d2 - dst2) * k;
      motorA_setpower(spd - u, false);
      motorB_setpower(spd + u, true);
    }
    d2 = readUS2_distance();
    delay(15);
    if ((d1 != (-1)) && (d1 < dst1))
    {
      break;
    }
    oldd2 = d2;
    Serial.print(timecount);
    Serial.print("\t");
    Serial.print(d1);
    Serial.print("\t");
    Serial.print(d2);
    Serial.print("\t");
    Serial.print(u);
    Serial.print("\t");
    Serial.println();
    timecount ++;
  }
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

// Функция медленного вращения сервомотора s от угла a до угла b в течении времени t (мс)
void slowServo(Servo s, int a, int b, int t)
{
  if ((t != 0) && (a != b))
  {
    float delay_step = (float)t / fabs(b - a);
    if (b >= a)
    {
      for (int angle = a; angle <= b; angle++)
      {
        s.write(angle);
        delay(delay_step);
      }
    }
    else
    {
      for (int angle = a; angle >= b; angle--)
      {
        s.write(angle);
        delay(delay_step);
      }
    }
  }
}

