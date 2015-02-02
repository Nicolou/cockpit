/* Pushbutton with Pullup, Teensyduino 
    test all the buttons
    Put the state of pressed buttons to the serial print output
    Prog customise for Teensy2++
    
    author: Nicolas Montarnal
    date : 2-2-2015
    
*/
const int numButtons = 36;  //36 for Teensy++

byte allButtons[numButtons]; //use to store current state of buttons
byte prevButtons[numButtons]; //use to store last state of buttons


void setup() {                
  Serial.begin(38400);
  // warm up the pins
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  //unset values
  for (int i=0; i<numButtons; i++) {
    allButtons[i] = 0;
    prevButtons[i] = 0;
  }
  Serial.println("Begin Complete for the Test");
}



void loop()                     
{
  // read digital pins and use them for the buttons
  for (int i=0; i<numButtons; i++) {
    if (digitalRead(i)) {
      // when a pin reads high, the button is not pressed
      // the pullup resistor creates the "on" signal
      allButtons[i] = 0;
    } else {
      // when a pin reads low, the button is pressed and connecting to ground.
      allButtons[i] = 1;
    }
  }
   
   
  // check to see if any button changed since last time
  boolean anyChangeButton = false;
  for (int i=0; i<numButtons; i++) {
    if (allButtons[i] != prevButtons[i]) {
      if (anyChangeButton == false) {
        Serial.println ("-------- Button(s) pressed : ------- ");
      }  
      anyChangeButton = true;
      //so if something changed, tell me
      Serial.print("Button number ");
      Serial.print(i, DEC);
      Serial.print(", value: ");
      Serial.println(allButtons[i], DEC);
    }
    prevButtons[i] = allButtons[i];
  }
  

  //wait 100 ms
  delay(100);
}

