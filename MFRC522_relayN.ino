/*
*********************************************************************************************************
*
*                                     MFRC522_relayN
*
* Filename      : MFRC522_relayN.ino
* Compilers     : Arduino
* IDE           : Arduino
* Version       : 1.0
* Programmer(s) : militrik@gmail.com
*                 vitalii
*
*********************************************************************************************************
*
* Note(s)       : Програма для роздільного доступу з RF карток з довільним алгоритмом роботи реле 
*
********************************************************************************************************
*
*History        
*1.0            : Базова програма
*
*********************************************************************************************************
*/

#include <stdio.h>
#include <string.h>

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

/* налаштування */

#define pinLED 2
#define pinRELAY1 3
#define pinRELAY2 5
#define pinBUZZER 4
#define pinSS 10
#define pinRST 9

#define RELAY1ACTIVE 0
#define RELAY2ACTIVE 1


#define RELAY_ON_TIMER 5000 // мС

#define CARDSAMOUUNT 50

#define TONEMAINCARD 800 // тон увімкнення реле
#define TONEALIENCARD 200 // тон чужа мітка
#define TONEDURATION 300 // тривалість звукового сигналу

enum CARDGROUPS { ALIEN, SERVICE, VIP, PEOPLE, GARAGE, // перерахування груп карток. Можна додавати
                  LASTCARDGROUP
                };
struct
{
  uint8_t cardcode[4];
  uint8_t cardtype;
} Cards[CARDSAMOUUNT] = // Сюди додавати карти. Перші чотири байти сам код, останній - типу карти з групи
{
  {{58, 214, 116, 130, }, VIP, } ,
  {{122, 218, 115, 130, }, VIP, } ,
  {{218, 147, 119, 130, }, VIP, } ,
  {{231, 114, 13, 175, }, VIP, } ,
  {{51, 75, 78, 22, }, SERVICE, } ,
  //{{51, 127, 218, 22, }, PEOPLE, } , // це велика картка
  {{10, 15, 126, 130, }, PEOPLE, } ,
  {{218, 134, 132, 129, }, SERVICE, } ,
  {{122, 110, 117, 130, }, PEOPLE, } ,
  {{138, 197, 114, 130, }, PEOPLE, } ,
  {{186, 178, 122, 130, }, PEOPLE, } ,
  {{106, 170, 115, 130, }, PEOPLE, } ,
  {{74, 230, 133, 130, }, PEOPLE, } ,
  {{138, 13, 126, 130, }, PEOPLE, } ,
  {{186, 220, 113, 130, }, PEOPLE, } ,
  {{218, 60, 140, 129, }, PEOPLE, } ,
  {{122, 2, 144, 129, }, SERVICE, } ,
  {{106, 14, 129, 129, }, PEOPLE, } ,
  {{138, 42, 142, 129, }, PEOPLE, } ,
  {{234, 226, 113, 130, }, PEOPLE, } ,
  {{74, 97, 133, 129, }, PEOPLE, } ,
  {{234, 106, 132, 130, }, PEOPLE, } ,
  {{250, 5, 121, 130, }, PEOPLE, } ,
  {{154, 193, 118, 130, }, PEOPLE, } ,
  {{135, 232, 25, 175, }, PEOPLE, } ,
  {{90, 134, 127, 129, }, PEOPLE, } ,
  {{218, 99, 129, 130, }, PEOPLE, } ,
  {{58, 184, 123, 130, }, PEOPLE, } ,
  {{106, 213, 114, 130, }, SERVICE, } ,
  {{106, 11, 121, 130, }, PEOPLE, } ,
  {{186, 122, 135, 129, }, PEOPLE, } ,
  {{138, 253, 130, 129, }, PEOPLE, } ,
  {{106, 205, 133, 130, }, PEOPLE, } ,
  {{42, 245, 120, 130, }, PEOPLE, } ,
  {{23, 101, 29, 175, }, PEOPLE, } ,
  {{138, 202, 128, 130, }, PEOPLE, } ,
  {{215, 33, 11, 175, }, PEOPLE, } ,
  {{250, 2, 124, 130, }, PEOPLE, } ,
  {{218, 100, 132, 130, }, PEOPLE, } ,
  {{10, 23, 130, 130, }, PEOPLE, } ,
  {{26, 138, 114, 130, }, PEOPLE, } ,
  {{106, 1, 133, 130, }, PEOPLE, } ,
  {{218, 40, 116, 130, }, PEOPLE, } ,
  {{186, 153, 119, 130, }, PEOPLE, } ,
  {{122, 193, 123, 130, }, PEOPLE, } ,
  {{186, 15, 114, 129, }, PEOPLE, } ,
  {{138, 247, 122, 130, }, PEOPLE, } ,
  {{42, 62, 126, 130, }, PEOPLE, } ,
  {{186, 208, 125, 130, }, PEOPLE, } ,
  {{234, 26, 130, 130, }, PEOPLE, } ,
};

MFRC522 mfrc522(pinSS, pinRST); //(SS_PIN, RST_PIN)

/* функції */

// Вимкнення всіх виходів
void AllPinsOFF (void)
{
  digitalWrite(pinLED, LOW);
  digitalWrite(pinRELAY1, LOW ^ RELAY1ACTIVE);
  digitalWrite(pinRELAY2, LOW ^ RELAY2ACTIVE);
  digitalWrite(pinBUZZER, LOW);
}

// попередня установка
void setup() {
  Serial.begin(9600);
  while (! Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  //Serial.println(mfrc522.PCD_GetAntennaGain());
  //mfrc522.PCD_SetAntennaGain(0x6<<4);
  //Serial.println(mfrc522.PCD_GetAntennaGain());
  pinMode(pinLED, OUTPUT); // led
  pinMode(pinRELAY1, OUTPUT); // rele1
  pinMode(pinRELAY2, OUTPUT); // rele2
  pinMode(pinBUZZER, OUTPUT); // zumer
  AllPinsOFF();

}

// активація
void activation(byte cardgroups)
{
  switch (cardgroups) // Сюди додавати дії з реле для певних груп карт
  {
    case ALIEN:
      {
        tone(pinBUZZER, TONEALIENCARD, TONEDURATION);
      }
      break;

    case VIP :
      {
        digitalWrite(pinLED, HIGH);
        digitalWrite(pinRELAY1, HIGH ^ RELAY1ACTIVE);
        digitalWrite(pinRELAY2, HIGH ^ RELAY2ACTIVE);
        tone(pinBUZZER, TONEMAINCARD, TONEDURATION);
        delay (RELAY_ON_TIMER);
      }
      break;
    case PEOPLE :
      {
        digitalWrite(pinLED, HIGH);
        digitalWrite(pinRELAY1, HIGH ^ RELAY1ACTIVE);
        tone(pinBUZZER, TONEMAINCARD, TONEDURATION);
        delay (RELAY_ON_TIMER);
      }
      break;
    case GARAGE :
      {

      }
      break;
      case SERVICE :
      {
        digitalWrite(pinLED, HIGH);
        digitalWrite(pinRELAY1, HIGH ^ RELAY1ACTIVE);
        digitalWrite(pinRELAY2, HIGH ^ RELAY2ACTIVE);
        tone(pinBUZZER, TONEMAINCARD*2, TONEDURATION);
        delay (RELAY_ON_TIMER);
      }
      break;

    default: break;
  }
  AllPinsOFF();
}

// Перевірка мітки
byte checkCard(void)
{
  byte res = ALIEN;

  for ( byte y = 0; y < CARDSAMOUUNT; y++)
  {
    if (!memcmp(Cards[y].cardcode, mfrc522.uid.uidByte, 4))
    {
      res = Cards[y].cardtype;
      break;
    }
  }
  return res;
}


/* основна програма */

void loop()
{
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() )
  {
    delay(200);
    return;
  }
  
  for (byte i = 0; i < 10; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i]);
    Serial.print(", ");
  }
  Serial.print("\r\n");
  activation(checkCard());
}
