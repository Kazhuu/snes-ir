/* This program is for Arduino Nano inside the SNES console. It controls console starting, resetting and RGB LED.
 * You can also control console with JVC TV remote (HC side must be selected from remote).
 * Program also have feature to save each LED color value to EEPROM for next use.
 * Red, green and blue colored buttons on remote controls same color LED on and off.
 * BACK button will turn all LEDs off.
 * OK button will save all LED values to EEPROM for next use.
 * MENU button will reset console.
 * -Vol and +Vol controls LEDs brightness.
 *
 * RGB LED is connected to digital pins 10, 9 and 6. IR receiver input is connected to digital pin 11.
 * Reset switch input is connected to digital pin 2. Transistor base is connected to pin 7.
 *
 * Changes:
 * Update 12.04.2013: Removed not needed digital pin and cleaned code little bit.
 * Update 6.9.2014: Cleaned some of the code and EEPROM overflow fixed.
 * Update 31.10.2021: Update a grammar a bit and uploaded to GitHub.
 */

#include <IRremote.h>
#include <EEPROM.h>

const int RECV_PIN = 11;    // Decided what pins this program uses.
IRrecv irrecv(RECV_PIN);    // IR receiver output goes to pin 11.
decode_results results;
const int Redledpin = 6;    //Three different LED pins.
const int Greenledpin = 10;
const int Blueledpin = 9;
const int Redaddr = 1;    //Bytes for EEPROM where to save LED values, one for each LED.
const int Greenaddr = 2;
const int Blueaddr = 3;
const int TransistorB = 7;  //Pin to send +5V to transistor base. This starts the console.
const int Resetpin = 2;    // Pin to reset console.
int Resetvalue = 0;    // Variable for reset button.

int Redbutton = 0;   // Variables for LED state, is it on or off.
int Redvalue = EEPROM.read(Redaddr);    // Variable for value to fading LEDs and to store it to EEPROM.
int Greenbutton = 0;
int Greenvalue = EEPROM.read(Greenaddr);
int Bluebutton = 0;
int Bluevalue = EEPROM.read(Blueaddr);

int Lastled = 0;    // Variable for remember which LED were selected.

int Fadeamount = 15;    // Amount how much to fade last selected LED with one button press.

void setup()    // This setup runs once when program starts.
{
  pinMode(Redledpin, OUTPUT);
  pinMode(Greenledpin, OUTPUT);
  pinMode(Blueledpin, OUTPUT);
  pinMode(TransistorB, OUTPUT);
  pinMode(Resetpin, INPUT);
  delay(1000);
  digitalWrite(TransistorB, HIGH);
  analogWrite(Redledpin, Redvalue);
  analogWrite(Greenledpin, Greenvalue);
  analogWrite(Blueledpin, Bluevalue);
  Serial.begin(9600);
  irrecv.enableIRIn();   // Start the receiver
}

void loop() {   //Program use CASE method to determine what button is pressed from remote and each button have different HEX value.
  Resetvalue = digitalRead(Resetpin);
  if (Resetvalue == HIGH)  {    // This if statement check if console reset button is press and if it is Arduino reset console.
    digitalWrite(TransistorB, LOW);
    delay(1000);
    digitalWrite(TransistorB, HIGH);
    Serial.println("SNES system reseted from RESET switch");
  }
  if (irrecv.decode(&results)) {
    switch (results.value)  {
    case 0xF7AB:      // BACK button control to turn all LEDs off and set all variables to LOW state.
      Redvalue = 0;
      Greenvalue = 0;
      Bluevalue = 0;
      analogWrite(Redledpin, Redvalue);
      analogWrite(Greenledpin, Greenvalue);
      analogWrite(Blueledpin, Bluevalue);
      Redbutton = LOW;
      Greenbutton = LOW;
      Bluebutton = LOW;
      Serial.println(Redvalue);
      Serial.println(Greenvalue);
      Serial.println(Bluevalue);
      Lastled = 0;
      Serial.println("All LEDs turned off");
      break;
    case 0xC43C:  // Red LED control for on and off.
      {
        Lastled = 1;
        if (Redbutton == LOW)  {
          Serial.println("Red LED turned on");
          Redvalue = 255;
          analogWrite(Redledpin, Redvalue);
          Redbutton = HIGH;
          break;
        }
        else  {
          Redvalue = 0;
          analogWrite(Redledpin, Redvalue);
          Serial.println("Red LED turned off");
          Redbutton = LOW;
          break;
        }
      }
    case 0xC4BC:    // Greeb LED control for on and off.
      Lastled = 2;
      if (Greenbutton == LOW)  {
        Serial.println("Green LED turned on");
        Greenvalue = 255;
        analogWrite(Greenledpin, Greenvalue);
        Greenbutton = HIGH;
        break;
      }
      else  {
        Greenvalue = 0;
        analogWrite(Greenledpin, Greenvalue);
        Serial.println("Green LED turned off");
        Greenbutton = LOW;
        break;
      }
    case 0xC4FC:    // Blue LED control for on and off.
      Lastled = 3;
      if (Bluebutton == LOW)  {
        Serial.println("Blue LED turned on");
        Bluevalue = 255;
        analogWrite(Blueledpin, Bluevalue);
        Bluebutton = HIGH;
        break;
      }
      else  {
        Bluevalue = 0;
        analogWrite(Blueledpin, Bluevalue);
        Serial.println("Blue LED turned off");
        Bluebutton = LOW;
        break;
      }
    case 0xC5F8:    // -vol control for make LED less bright
      if (Lastled == 1)  {
        Redvalue = Redvalue - Fadeamount;
        if (Redvalue > 0) {
          Redvalue = 0;
        }
        analogWrite(Redledpin, Redvalue);
        Serial.println("Red LED brighness -15");
        Serial.println(Redvalue);
      }
      if (Lastled == 2) {
        Greenvalue = Greenvalue - Fadeamount;
        if (Greenvalue > 0) {
          Greenvalue = 0;
        }
        analogWrite(Greenledpin, Greenvalue);
        Serial.println("Green LED brighness -15");
        Serial.println(Greenvalue);
      }
      if (Lastled == 3) {
        Bluevalue = Bluevalue - Fadeamount;
        if (Bluevalue > 0) {
          Bluevalue = 0;
        }
        analogWrite(Blueledpin, Bluevalue);
        Serial.println("Blue LED brighness -15");
        Serial.println(Bluevalue);
      }
      break;
    case 0xC578:    // +vol control for make LED more bright
      if (Lastled == 1)  {
        Redvalue = Redvalue + Fadeamount;
        if (Redvalue > 255) {
          Redvalue = 255;
        }
        analogWrite(Redledpin, Redvalue);
        Serial.println("Red LED brighness +15");
        Serial.println(Redvalue);
      }
      if (Lastled == 2) {
        Greenvalue = Greenvalue + Fadeamount;
        if (Greenvalue > 255) {
          Greenvalue = 255;
        }
        analogWrite(Greenledpin, Greenvalue);
        Serial.println("Green LED brighness +15");
        Serial.println(Greenvalue);
      }
      if (Lastled == 3) {
        Bluevalue = Bluevalue + Fadeamount;
        if (Bluevalue > 255) {
          Bluevalue = 255;
        }
        analogWrite(Blueledpin, Bluevalue);
        Serial.println("Blue LED brighness +15");
        Serial.println(Bluevalue);
      }
      break;
    case 0xF72B:    // OK button in "HC" side to save all different LED values to EEPROM.
      EEPROM.write(Redaddr, Redvalue);
      EEPROM.write(Greenaddr, Greenvalue);
      EEPROM.write(Blueaddr, Bluevalue);
      Serial.println(Redvalue);
      Serial.println(Greenvalue);
      Serial.println(Bluevalue);
      Serial.println("All LED values saved to EEPROM");
      break;
    case 0xF783:    // MENU button in "HC" side to reset console.
      digitalWrite(TransistorB, LOW);
      delay(1000);
      digitalWrite(TransistorB, HIGH);
      Serial.println("SNES system reseted from remote");
      break;
    }
    irrecv.resume(); // Receive the next value
  }
}
