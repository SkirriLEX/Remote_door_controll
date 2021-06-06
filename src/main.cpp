//Led strip ws2812b 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>

#define DEBUG

#define PIN_C1_BUTTONS A2
#define PIN_C2_BUTTONS 8
#define PIN_R1_BUTTONS 3
#define PIN_R2_BUTTONS 4
#define PIN_R3_BUTTONS 5
#define PIN_R4_BUTTONS 6
#define PIN_R5_BUTTONS 7
#define PIN_LED        2
#define NUM_LEDS       10
#define RADIO_ADDRESS  "1N"

int pinC_button [] = {PIN_C1_BUTTONS, PIN_C2_BUTTONS};
int pinR_button [] = {PIN_R1_BUTTONS, PIN_R2_BUTTONS, PIN_R3_BUTTONS, PIN_R4_BUTTONS, PIN_R5_BUTTONS};
bool buttons [2][5] = {0};

RF24 radio(9, 10); // CE and CSN
const byte rxAddr[2] = "1N";;  // адрес

byte counter ;
CRGBArray<NUM_LEDS> leds;

void setup() {
  #ifdef DEBUG
  Serial.begin (115200);
  Serial.println("Start");
  #endif
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  for (int i = 0; i < 2; i++) {
    pinMode (pinC_button[i], OUTPUT);
  }
  for (int i = 0; i < 5; i++) {
    pinMode (pinR_button[i], INPUT_PULLUP);
  }
  digitalWrite(PIN_C2_BUTTONS, LOW);
  digitalWrite(PIN_C1_BUTTONS, HIGH);
  // radio.begin();
  // radio.setRetries(0, 1);    //(время между попыткой достучаться, число попыток)
  // radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  // radio.setPayloadSize(1);     //размер пакета, в байтах

  // radio.openWritingPipe(rxAddr);   //мы - труба 0, открываем канал для передачи данных
  // radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  // radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  // radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  // //должна быть одинакова на приёмнике и передатчике!
  // //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

}

void Send_radio (byte id){
  //radio.write(&id, sizeof(id));
}

void led_blink (byte id){
  int led_id = (8-((id%5)*2)) + (id/5);
  leds[led_id] = CRGB(10,10,0);
  FastLED.show();
  delay(100);
  leds[led_id] = CRGB::Black;
  FastLED.show();
}

void Check_Button() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(pinC_button[i], LOW);
    for (int j = 0; j < 5; j++) {
      if (digitalRead(pinR_button[j]) == LOW) {
        #ifdef DEBUG
        Serial.print("Get ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(j);
        Serial.println();
        #endif
        counter = i * 5 + j;
        Send_radio(counter);
        led_blink(counter);
        delay(100);
      }
    }
    digitalWrite(pinC_button[i], HIGH);
  }
}

void loop() {
  

//  for (int i = 20; i < 30 ; i++) {
//    counter = i;
//    Serial.println (i);
//    radio.write(&i, sizeof(i));
    Check_Button();
//    delay(100);
//  }
}
