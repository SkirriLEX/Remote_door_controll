#define DEBUG

#define PIN_INTERRUPT 2
#define PIN_TRIGGER_A 3
#define PIN_TRIGGER_B 7
#define PIN_MOTOR_DIR 5
#define PIN_MOTOR_PUL 4
#define PIN_MOTOR_EN  6
#define SIGNCHANEl 9

#define MOTOR_SPEED_MIN 200
#define MOTOR_SPEED_MAX 10
#define C_START_LIM_MIN 0
#define C_START_LIM_MAX 1000
#define C_END_LIM_MIN   10300
#define C_END_LIM_MAX   10800
#define T_START_LIM_MIN 90
#define T_START_LIM_MAX 20
#define T_END_LIM_MIN 20
#define T_END_LIM_MAX 100

#include <Arduino.h>
#include "printf.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);  // указать номера пинов, куда подключаются CE и CSN
const byte pipe[2] = "1N"; // адрес
int count;
byte pipeNo, gotByte;

volatile boolean flag_tx = 0;
volatile boolean flag_fail = 0;
volatile boolean flag_rx = 0;

bool flag_dir = false;
int motor_speed = 100;
int motor_time = 10;
bool last_t_A = false;
bool last_t_B = false;
uint16_t test_counter = 0;
uint16_t subcounter = 0;

long maper(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void nrf_init() {
  radio.begin();
  radio.setAutoAck(0);          //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 1);      //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();     //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(1);     //размер пакета, в байтах

  radio.openReadingPipe(1, pipe);     //хотим слушать трубу 0
  //  radio.openWritingPipe(pipe);
  radio.setChannel(0x60);               //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS);   //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();         //начать работу
//  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

#ifdef DEBUG
  radio.printDetails();
#endif
}

void M_step(int timing, int count) {
  test_counter++;
  digitalWrite(PIN_MOTOR_DIR, flag_dir);
  for (int i = 0; i < count ; i++ ) {
    digitalWrite(PIN_MOTOR_PUL, HIGH);
    delayMicroseconds(timing);
    digitalWrite(PIN_MOTOR_PUL, LOW);
    delayMicroseconds(timing);
  }
}

void change_dir() {
  digitalWrite(PIN_MOTOR_EN, HIGH);
  Serial.print("Counts :");
  Serial.println(test_counter);
  test_counter = 0;
  delay(1000);

  radio.startListening();
  interrupts();
  while (true) {
    if (flag_rx) {
      byte got = 0;
      radio.read(&got, sizeof(got));

#ifdef DEBUG
      Serial.print("RECEIVED ");
      Serial.println(got);
#endif
      if (got == SIGNCHANEl) {
        Serial.println ("Yes");
        break;
      }
      flag_rx = 0;
    }
  }

  radio.stopListening();
  noInterrupts();
  Serial.print("speed :");
  Serial.println(motor_speed);
  flag_dir = !flag_dir;
  digitalWrite(PIN_MOTOR_EN, LOW);
}

void detectchange() {
  bool current_t_A = digitalRead(PIN_TRIGGER_A);
  bool current_t_B = digitalRead(PIN_TRIGGER_B);
  if (last_t_A != current_t_A) {
    if (current_t_A) {
      if (current_t_A && flag_dir) {
        Serial.println("Move < -  ");
        change_dir();
      }
    }
    last_t_A = current_t_A;
  }
  if (last_t_B != current_t_B) {
    if (current_t_B) {
      if (current_t_B && !flag_dir) {
        Serial.println("Move - > ");
        change_dir();
      }
    }
    last_t_B = current_t_B;
  }
}

void check_radio() {
  bool tx, fail, rx;
  radio.whatHappened(tx, fail, rx); // читаем регистр
  flag_tx = tx; // 1 если успешно отправили данные
  flag_fail = fail; // 1 если данные не отправленны
  flag_rx = rx;  // 1 если пришли данные для чтения
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("start");
  printf_begin();
#endif
  pinMode(PIN_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), check_radio, FALLING);
  pinMode(PIN_TRIGGER_A, INPUT_PULLUP);
  pinMode(PIN_TRIGGER_B, INPUT_PULLUP);
  pinMode(PIN_MOTOR_DIR, OUTPUT);
  pinMode(PIN_MOTOR_EN, OUTPUT);
  pinMode(PIN_MOTOR_PUL, OUTPUT);
  digitalWrite(PIN_MOTOR_EN, LOW);
  digitalWrite(PIN_MOTOR_DIR, flag_dir);
  last_t_A = digitalRead(PIN_TRIGGER_A);
  last_t_A = digitalRead(PIN_TRIGGER_B);
  motor_speed = MOTOR_SPEED_MAX;
  nrf_init();
  noInterrupts();
}

void calcSpeedUp() {
  motor_speed = maper (test_counter, C_START_LIM_MIN, C_START_LIM_MAX, T_START_LIM_MIN, T_START_LIM_MAX);
}

void calcSpeedDown() {
  motor_speed = maper (test_counter, C_END_LIM_MIN, C_END_LIM_MAX, T_END_LIM_MIN, T_END_LIM_MAX);
}

void loop()
{
//    Serial.print(" A:");
//    Serial.print(digitalRead(PIN_TRIGGER_A));
//    Serial.print(" B:");
//    Serial.print(digitalRead(PIN_TRIGGER_B));
//    Serial.println(" ");
//    delay(100);

  detectchange();

  if (test_counter > C_START_LIM_MIN && test_counter < C_START_LIM_MAX) {
    if (test_counter % 10 == 0) {
      calcSpeedUp();
    }
  }
  if (test_counter > C_END_LIM_MIN && test_counter < C_END_LIM_MAX) {
    if (test_counter % 10 == 0) {
      calcSpeedDown();
    }
  }

  //    Serial.print(test_counter);
  //    Serial.print(" ");
  //    Serial.print(motor_speed);
  //    Serial.println(" ");

  M_step(motor_speed, motor_time);
  //    M_step(100);
  if (flag_rx) flag_rx = 0;
  

}
