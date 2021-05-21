//Led strip ws2812b 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>

// #define PIN_C1_BUTTONS A0
// #define PIN_C2_BUTTONS A1
// #define PIN_R1_BUTTONS 2
// #define PIN_R2_BUTTONS 3
// #define PIN_R3_BUTTONS 4
// #define PIN_R4_BUTTONS 5
// #define PIN_R5_BUTTONS 7
#define PIN_LED        2
#define NUM_LEDS       8

// int pinC_button [] = {A0, A1};
// int pinR_button [] = {2, 3, 4, 5, 7};
// bool buttons [2][5] = {0};

// RF24 radio(9, 10); // CE and CSN
// const byte rxAddr[2] = "1N";  // адрес

// byte counter ;
CRGBArray<NUM_LEDS> leds;

void setup() {
  Serial.begin (115200);
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  // for (int i = 0; i < 2; i++) {
  //   pinMode (pinC_button[i], OUTPUT);
  // }
  // for (int i = 0; i < 5; i++) {
  //   pinMode (pinR_button[i], INPUT_PULLUP);
  // }
  // digitalWrite(PIN_C2_BUTTONS, LOW);
  // digitalWrite(PIN_C1_BUTTONS, HIGH);
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

  // //не слушаем радиоэфир, мы передатчик

  Serial.println("Start");
}

// void Check_Button() {
//   for (int i = 0; i < 2; i++) {
//     digitalWrite(pinC_button[i], LOW);
//     for (int j = 0; j < 5; j++) {
//       if (digitalRead(pinR_button[j]) == LOW) {
//         Serial.print("Get ");
//         Serial.print(i);
//         Serial.print(" ");
//         Serial.print(j);
//         Serial.println();
//         counter = i * 5 + j;
//         radio.write(&counter, sizeof(counter));
//         delay(100);
//       }
//     }
//     digitalWrite(pinC_button[i], HIGH);
//   }
// }

void loop() {
  for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB(255,255,0);

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
   }
  //  Serial.print (digitalRead(PIN_R1_BUTTONS));
  //  Serial.print (" ");
  //  Serial.print (digitalRead(PIN_R2_BUTTONS));
  //  Serial.print (" ");
  //  Serial.print (digitalRead(PIN_R3_BUTTONS));
  //  Serial.print (" ");
  //  Serial.print (digitalRead(PIN_R4_BUTTONS));
  //  Serial.print (" ");
  //  Serial.print (digitalRead(PIN_R5_BUTTONS));
  //  Serial.println (" ");
//  for (int i = 20; i < 30 ; i++) {
//    counter = i;
//    Serial.println (i);
//    radio.write(&i, sizeof(i));
    // Check_Button();
//    delay(100);
//  }
}
