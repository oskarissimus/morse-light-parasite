//morse code implementation taken from http://www.mechatrobot.pl/morse-arduino/
/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "XXX";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "XXX";
char pass[] = "XXX";

WidgetTerminal terminal(V1);


#define RELAY_PIN 14


//http://www.mechatrobot.pl/morse-arduino/
const int ledPin = 14; //numer pinu diody led
int dotLength = 500; //długość trwania kropki w milisekundach
bool morseLoop = false;

char readChar = ' '; //wczytany znak

const char charTable[37] = { //tablica z literami i cyframi
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'w', 'v', 'x', 'y', 'z',
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

const char capitalLetter[27] = { //tablica dużych liter, pozycje w tablicy analogiczne do małych liter
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'W', 'V', 'X', 'Y', 'Z'
};

const short morseTable[37][7] = { //dwuwymiarowa tablica, pierwszy wymiar odpowiada za dany znak z tablicy liter i cyfr,
  //a drugi zawiera przetworzony ciąg kropek i kresek - zrozumiały w kodzie morse'a.
  //0 - dot, 1 - dash, 2 - end //jako kropkę oznaczamy 0, kreska to 1, a koniec ciągu to 2
  {0, 1, 2},//a
  {1, 0, 0, 0, 2},//b
  {1, 0, 1, 0, 2},//c
  {1, 0, 0, 2},//f
  {0, 2},//e
  {0, 0, 1, 0, 2},//f
  {1, 0, 0, 2},//g
  {0, 0, 0, 0, 2},//h
  {0, 0, 2},//i
  {0, 1, 1, 1, 2},//j
  {1, 0, 1, 2},//k
  {0, 1, 0, 0, 2},//l
  {1, 1, 2},//m
  {1, 0, 2},//n
  {1, 1, 1, 2},//o
  {0, 1, 1, 0, 2},//p
  {1, 1, 0, 1, 2},//q
  {0, 1, 0, 2},//r
  {0, 0, 0, 2},//s
  {1, 2},//t
  {0, 0, 1, 2},//u
  {0, 1, 1, 2},//w
  {0, 0, 0, 1, 2},//v
  {1, 0, 0, 1, 2},//x
  {1, 0, 1, 2},//y
  {1, 1, 0, 0, 2},//z
  {0, 1, 1, 1, 1, 2},//1
  {0, 0, 1, 1, 1, 2},//2
  {0, 0, 0, 1, 1, 2},//3
  {0, 0, 0, 0, 1, 2},//4
  {0, 0, 0, 0, 0, 2},//5
  {1, 0, 0, 0, 0, 2},//6
  {1, 1, 0, 0, 0, 2},//7
  {1, 1, 1, 0, 0, 2},//8
  {1, 1, 1, 1, 0, 2},//9
  {1, 1, 1, 1, 1, 2}//0
  //0 - dot, 1 - dash, 2 - end
};




void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
}

BLYNK_WRITE(V1)
{
  String odbior = param.asStr();

  terminal.print("\nusing morse code to send message \"");
  terminal.print(odbior);
  terminal.print("\"...\n");
  terminal.flush();

    for (int i = 0 ; i < odbior.length() ; i++) {
      Serial.print(odbior[i]);
      morseCharToDigitalPin(odbior[i]);

    }
  
}



BLYNK_WRITE(V3)
{
  dotLength = param.asInt();
  terminal.print("parameter dotLength set to ");
  terminal.println(dotLength);
  terminal.flush();
}


void morseCharToDigitalPin (char readChar) {
  Serial.print("Load character '"); //wyświetla komunikat
  Serial.print(readChar); //z wczytanym znakiem
  Serial.print("'");
  for (int i = 0; i < 27; i++) { //iteruje po 26 elementach, tyle ile ma tablica z wielkimi literami

    if (readChar == capitalLetter[i]) { //sprawdza, czy wczytana litera jest dużą
      readChar = charTable[i]; //zmienia na małą literę
    }
  }

  for (int i = 0; i < 37; i++) { //iteruje po tablicy z literami i cyframi

    if (readChar == charTable[i]) { //jeśli wczytany znak odpowiada temu spośród liter i cyfr
      Serial.print(", sending { "); //wysyla komunikat

      for (int j = 0; j < 6; j++) { //6, ponieważ tyle maksymalnie znaków(kropek, kresek i zakończeń) moze miec przypisane litera lub liczba

        if (morseTable[i][j] == 0) { //jeśli napotka na 0, to zgodnie z ustaleniem jest to kropka
          playDot(); //nadaję kropkę
          Serial.print(". ");
        }
        else if (morseTable[i][j] == 1) { //jeśli napotka na 1, to zgodnie z ustaleniem jest to kreska
          playDash(); //nadaję kreskę
          Serial.print("- ");
        }
        else { //w innym przypadku musi to być 2, czyli koniec ciągu
          pauseChar(); //nadaje przerwę między znakami zgodną z zasadami nadawania kodem morse'a
          Serial.println(" } done.");
          break; //przerywa tą pętlę i wychodzi poziom wyżej
        }
      }
      break;
    }
  }
}



void playDot() { //funkcja do nadawania kropki przy pomocy buzzera i diody led

  digitalWrite(ledPin, LOW);  //zapala diodę led
  delay(dotLength); //oczekuje zgodnie z zasadami nadawania
  digitalWrite(ledPin, HIGH); //gasi diodę led
  delay(dotLength); //oczekuje zgodnie z zasadami nadawania
}

void playDash() { //funkcja do nadawania kreski przy pomocy buzzera i diody led

  digitalWrite(ledPin, LOW);  //zapalam diodę led
  delay(dotLength * 3); //oczekuje zgodnie z zasadami nadawania
  digitalWrite(ledPin, HIGH); //gasi diodę led
  delay(dotLength); //oczekuje zgodnie z zasadami nadawania
}

void pauseChar() { //funkcja wymuszajaca przerwę w nadawaniu pomiędzy znakami

  delay(dotLength * 2); //oczekuje zgodnie z zasadami nadawania
}

void pauseWord() { //funkcja wymuszajaca przerwę w nadawaniu pomiędzy wyrazami

  delay(dotLength * 6); //oczekuje zgodnie z zasadami nadawania
}
