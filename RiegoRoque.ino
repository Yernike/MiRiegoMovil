
/*  AUTHOR: Yeray Betancor Caro

  Microcontrolador wifi ESP8266
  
 *************************************************************/

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

SimpleTimer timer;
char auth[] = "eaf538e0f0844388bc1d1a5460bc4b1f";

char ssid[] = "NETLLAR_63B136";
char pass[] = "60191748";

//Variables del programa
const int vPins[5] = {V0, V1, V2, V3, V4};
const int channelPins[4] = {5, 4, 2, 0}; //Numero asignacion de Pines
const int displayPins[4] = {V5, V6, V7, V8}; //Numero asignacion de Pines

int channelRemaining[4] = {0, 0, 0, 0}; //Tiempo restante
int channelMode[4]  = {0, 0, 0, 0}; // 0 OFF, 1 SECUENCIAL, 2 MANUAL
int timeSlider = 0;
int modeSimultaneo = 0;

const int OFF = 0;
const int SECUENCIAL = 1;
const int SIMULTANEO = 2;
const int MANUAL = 3;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000, heartBeat);
}

void loop()
{
  Blynk.run();
  timer.run();
}
void heartBeat1() {
  Serial.println("Hola");
}
void heartBeat() {
  exit;
  if (Blynk.connected()) {
    bool leastOne = false;
    for (int i = 0; i < 4; i++) {
      if (channelMode[i] != OFF && channelMode[i] != MANUAL && channelRemaining[i] <= 0) {
        channelMode[i] = OFF;
        Serial.println("APAGANDO Canal " + String(i));
        Blynk.setProperty(displayPins[i], "color", "#c83349");
        Blynk.virtualWrite(vPins[i + 1], LOW);
      }
      switch (channelMode[i]) {

        case MANUAL:
          Serial.println("Canal " + String(i) + " MANUAL");

          if ((channelRemaining[i] + 1) % 60 != 0) {
            break;
          }
          Blynk.setProperty(displayPins[i], "color", "#4CAF50");

          //ACTIVAR
          break;
        case SIMULTANEO:
          Serial.println("Canal " + String(i) + " SIMULTANEO");

          //ACTIVAR
          if ((channelRemaining[i]) % 60 == 0) {
            Blynk.setProperty(displayPins[i], "color", "#4CAF50");
            Blynk.virtualWrite(displayPins[i], channelRemaining[i] / 60);
          }
          channelRemaining[i]--;
          break;
        case SECUENCIAL:
          Serial.print("Canal " + String(i) + " SECUENCIAL ");
          if (!leastOne) {
            Serial.println("ACTIVO");

            leastOne = true;
            //ACTIVAR
            if ((channelRemaining[i]) % 60 == 0) {


              Blynk.setProperty(displayPins[i], "color", "#4CAF50");
              Blynk.virtualWrite(displayPins[i], channelRemaining[i] / 60);

            }
            channelRemaining[i]--;
          } else {

            Serial.println("ESPERANDO");
            //DESACTIVAR
            if ((channelRemaining[i] + 1) % 60 != 0) {
              break;
            }
            Blynk.setProperty(displayPins[i], "color", "#c83349");
          }
          break;
        case OFF:
          Serial.println("Canal " + String(i) + " OFF");
          if ((channelRemaining[i] + 1) % 60 != 0) {
            break;
          }
          Blynk.setProperty(displayPins[i], "color", "#c83349");
          break;

      }

    }
  }
}
BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V13) //Selector de tiempo
{
  timeSlider = param.asInt();
}

BLYNK_WRITE(V14) //Selector Simultaneo
{
  modeSimultaneo = param.asInt();
}

BLYNK_WRITE(V1) {
  setTimer(0, param.asInt());
}
BLYNK_WRITE(V2) {
  setTimer(1, param.asInt());
}
BLYNK_WRITE(V3) {
  setTimer(2, param.asInt());
}
BLYNK_WRITE(V4) {
  setTimer(3, param.asInt());
}



void setTimer(int channel, int buttonValue) {

  if (buttonValue == HIGH) {
    if (timeSlider == 0) {
      Blynk.virtualWrite(displayPins[channel], "M");
      Blynk.setProperty(displayPins[channel], "color", "#4CAF50");
      channelMode[channel] = MANUAL;
    } else {
      if (modeSimultaneo == HIGH) {
        channelMode[channel] = SIMULTANEO;
      } else {
        channelMode[channel] = SECUENCIAL;
      }
      Blynk.virtualWrite(displayPins[channel], timeSlider);
    }
    //Blynk.setProperty(displayPins[channel], "color", "#c83349");
    channelRemaining[channel] = timeSlider * 60;

  } else {
    Blynk.setProperty(displayPins[channel], "color", "#c83349");
    channelMode[channel] = OFF;
    Blynk.virtualWrite(displayPins[channel], 0);
    channelRemaining[channel] = 0;
  }


}


