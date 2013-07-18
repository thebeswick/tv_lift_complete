#include <IRremote.h>
const int RECV_PIN = 11;

const int activate = 2; //set up our button constants
const int gateOpen = 3;
const int gateClosed = 4;
const int bridgeOneA = 5;
const int bridgeTwoA = 6;
const int statLED = 13;

IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long currentTime = 0;
boolean gateState = false; //false = closed true = open
boolean IRstate = false;
void setup() {
  Serial.begin(9600);
  pinMode(activate, INPUT); //set up I/O
  pinMode(gateOpen, INPUT);
  pinMode(gateClosed, INPUT);
  pinMode(bridgeOneA, OUTPUT);
  pinMode(bridgeTwoA, OUTPUT);
//  pinMode(enable, OUTPUT);
//  digitalWrite(enable, LOW); //make sure H-Bridge is off
  pinMode(statLED, OUTPUT); //setup our status LED
// IR setup stuff
 irrecv.enableIRIn();
 
}
// Compare two tick values, returning 0 if newval is shorter,
// 1 if newval is equal, and 2 if newval is longer
// Use a tolerance of 20%
int compare(unsigned int oldval, unsigned int newval) {
  if (newval < oldval * .8) {
    return 0;
  } 
  else if (oldval < newval * .8) {
    return 2;
  } 
  else {
    return 1;
  }
}

// Use FNV hash algorithm: http://isthe.com/chongo/tech/comp/fnv/#FNV-param
#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 2166136261

/* Converts the raw code values into a 32-bit hash code.
 * Hopefully this code is unique for each button.
 */
unsigned long decodeHash(decode_results *results) {
  unsigned long hash = FNV_BASIS_32;
  for (int i = 1; i+2 < results->rawlen; i++) {
    int value =  compare(results->rawbuf[i], results->rawbuf[i+2]);
    // Add value into the hash
    hash = (hash * FNV_PRIME_32) ^ value;
  }
  return hash;
}




void loop() {
      if (irrecv.decode(&results)) {
    unsigned long hash = decodeHash(&results);
    Serial.print("Hash =");
    Serial.println(hash);
  
      switch (hash) {

    case 0xba58513:
      Serial.println("yay");
      delay(200);
      IRstate = true;
      break;
    default:
      Serial.print("Unknown ");
      Serial.println(hash, HEX);    
    }
    
    
//  if (digitalRead(activate) == HIGH && gateState == false) { //check to see if the button is pressed and the gate is closed
if (IRstate == true && gateState == false) {
    Serial.println("Should activate"
    );
//    digitalWrite(enable, HIGH); //enable h-bridge
    digitalWrite(bridgeOneA, HIGH); //configure for CW rotation
    digitalWrite(bridgeTwoA, LOW);
    while(1){ //run motor until switch is tripped
      if (digitalRead(gateClosed) == HIGH) { //check switch state
        gateState = true;
        IRstate = false;
        Serial.println("Received switch x");
        Serial.print("Gatestate = ");
        Serial.println(gateState);
        digitalWrite(statLED, LOW); //turn off LED
//        digitalWrite(enable, LOW); //disable h-bridge
        digitalWrite(bridgeOneA, LOW);
        digitalWrite(bridgeTwoA, LOW); 
        //reset h-bridge config
        break;
      }
      if (millis() > currentTime + 500) { //flash status LED once
        digitalWrite(statLED, HIGH);
        delay(500);
        currentTime = millis();
      }
      else {
        digitalWrite(statLED, LOW);
      }
    }
  }
//  if (digitalRead(activate) == HIGH && gateState == true) { //check to see if the button is pressed and the gate is open

if (IRstate == true && gateState == true) {
    Serial.println("Close section");
    digitalWrite(bridgeOneA, LOW); //configure for CCW rotation
    digitalWrite(bridgeTwoA, HIGH);
    while(1){
      if (digitalRead(gateOpen) == HIGH) {
        gateState = false;
        IRstate = false;
        digitalWrite(statLED, LOW);
//        digitalWrite(enable, LOW);
        
        digitalWrite(bridgeTwoA, LOW);
        
        break;
      }
      if (millis() > currentTime + 500) { //flash status LED once
        digitalWrite(statLED, HIGH);
        delay(500);
        currentTime = millis();
      }
      else {
        digitalWrite(statLED, LOW);
      }
    }
  }


    irrecv.resume(); // Resume decoding (necessary!)
  }
}


