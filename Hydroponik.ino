// Hydroponic Control, Stand 200401
// alles ohne Gewähr!
// Fehler und Vorschläge an salatmaschine@mnet-online.de
//#include <Key.h>

#include <Keypad.h>
#include <stdio.h> // for function sprintf
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// LCD Adresse auf 0x20 für ein 20 chars/2 line display
LiquidCrystal_I2C lcd(0x27,20,4);  


//Hier wird die größe des Keypads definiert
const byte COLS = 4; //4 Spalten
const byte ROWS = 4; //4 Zeilen

//Die Ziffern und Zeichen des Keypads werden eingegeben:
char hexaKeys[ROWS][COLS]={
{'D','#','0','*'},
{'C','9','8','7'},
{'B','6','5','4'},
{'A','3','2','1'}
};

byte colPins[COLS] = {2,3,4,5}; //Definition der Pins für die 4 Spalten
byte rowPins[ROWS] = {6,7,8,9};//Definition der Pins für die 4 Zeilen
char Taste; //Taste ist die Variable für die jeweils gedrückte Taste.

//Das Keypad kann absofort mit "Tastenfeld" angesprochen werden
Keypad Tastenfeld = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// digitale Ausgaenge für Relaissteuerung
int Kanal_1 = 10;
int Kanal_2 = 11;
int Kanal_3 = 12;
int Kanal_4 = 13;

// alle variablen hier als unsigned long
// mit int gab es bei langen Perioden Überläufen
unsigned long last_water = 0; // Zeitpunkt letzte Waesserung in ms
unsigned long max_water;                // Dauer Wasser
unsigned long duration_interval;        // Interval
bool pumpe_an;                // Pumpe an
unsigned long dry_time_left;            // vebleibende Zeit bis Start Waesserung
unsigned long wet_time_left;            // Vebleibende Zeit bis Ende Waesserung
char out[3];


void setup() {
  pinMode(Kanal_1, OUTPUT);   // Setzt den Digitalpin 13 als Outputpin
  // hier weitere Kanaele ergaenzen

  // Initialwerte setzen
  max_water = 5.0; // sec 
  duration_interval = 1.0; //min
  last_water=millis(); // millis
  //duration_water = 0; // sec
  digitalWrite(Kanal_1, LOW);
  pumpe_an = false;


  // LCD initialisieren
  lcd.init(); 
  
  // begruessung
  lcd.clear();
  lcd.setBacklight(10); 
  //lcd.blink();
  lcd.print("Hydroponic Control");
  delay(2000);

  // Standardanzeige
  lcd.setCursor(0,1);  
  lcd.print("*Dry/m:");
  lcd.setCursor(1,2);  
  lcd.print("Wet/s:");

}

void loop() {

  Taste = Tastenfeld.getKey();

  //if (Taste) 

  switch (Taste) {
    case 'A':
      // Bewässerungsdauer reduzieren  
      lcd.setBacklight(10); 
      if (max_water - 1.0 > 0) {
        max_water = max_water - 1.0;
      }
      break;
    case 'B':
      // Bewässerungsdauer verlängern
      lcd.setBacklight(10); 
      max_water = max_water + 1.0;
      break;
    case 'C':
      // Bewässerungsintervall verkürzen
      lcd.setBacklight(10); 
      if (duration_interval - 1.0 > 0) {
        duration_interval = duration_interval - 1.0;
      }
      break;
    case 'D':
      // Bewässerungsintervall erhöhen
      lcd.setBacklight(10); 
      duration_interval = duration_interval + 1.0;
      break;
    case '0':
      // sofort wässern
      lcd.setBacklight(10); 
      last_water = millis() - duration_interval*60000;
      break;     
    case '*':
      // Werte zuruecksetzen
      max_water = 5.0; // sec 
      duration_interval = 1; //min
      break;
    default:
      break;
  }

  //Anzeige aktualisieren
  lcd.setCursor(1,1);  
  lcd.print("Dry/m:");
  lcd.setCursor(1,2);  
  lcd.print("Wet/s:");
  lcd.setCursor(8,1);
  sprintf(out,"%3d",duration_interval);
  lcd.print(out);
  //lcd.print("   ");
  lcd.setCursor(8,2);
  sprintf(out,"%3d",max_water);
  lcd.print(out);
  //lcd.print("   ");
    
  //pruefen ob wasser angeschaltet werden soll
  if (millis() > last_water + duration_interval*60000) {
    // pumpe anschalten
    digitalWrite(Kanal_1, HIGH);
    pumpe_an = true;
    // Bildschirm ausschalten, muss nicht immer leuchten!
    lcd.setBacklight(0);
    last_water = millis();
    // Anzeige setzen
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print("*");  
    lcd.setCursor(12,1);
    lcd.print("        ");
  } else if (!pumpe_an) {
    // Zeit runterzaehlen
    lcd.setCursor(12,1);
    lcd.print("now: ");
    dry_time_left = duration_interval -(millis()-last_water)/60000;
    sprintf(out,"%3d",dry_time_left);
    lcd.print(out);
  }
  
        
  if (pumpe_an) {
    if (millis() > (last_water + max_water*1000)) {
      // Pumpe aus
      digitalWrite(Kanal_1, LOW);
      pumpe_an = false;
      // Anzeige setzen
      lcd.setCursor(0,1);
      lcd.print("*");
      lcd.setCursor(0,2);
      lcd.print(" ");
      lcd.setCursor(12,2);
      lcd.print("        ");
    } // if
    
    else {
      // Zeit runterzaehlen
      lcd.setCursor(12,2);
      wet_time_left = max_water -(millis()-last_water)/1000;
      lcd.print("now: ");
      sprintf(out,"%3d",wet_time_left);
      lcd.print(out);
      lcd.setCursor(12,1);
      lcd.print("        ");
    } // else
  } // if Pumpe_an
} // loop
