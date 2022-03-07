// constants won't change. They're used here to set pin numbers:
const int BTN_START_PIN =  36;     // the number of the pushbutton pin
const int BTN_B_PIN =  14;     // the number of the pushbutton pin
const int BTN_A_PIN =  12;     // the number of the pushbutton pin

const int BTN_CUP_PIN =  19;     // the number of the pushbutton pin
const int BTN_CL_PIN =  17;     // the number of the pushbutton pin
const int BTN_CR_PIN =  18;     // the number of the pushbutton pin
const int BTN_CD_PIN =  22;     // the number of the pushbutton pin

const int BTN_DUP_PIN =  25;     // the number of the pushbutton pin
const int BTN_DL_PIN =  26;     // the number of the pushbutton pin
const int BTN_DR_PIN =  33;     // the number of the pushbutton pin
const int BTN_DD_PIN =  27;     // the number of the pushbutton pin

const int BTN_SR_PIN =  23;     // the number of the pushbutton pin
const int BTN_SL_PIN =  32;     // the number of the pushbutton pin

const int BTN_Z_PIN =  16;     // the number of the pushbutton pin


const int internalLEDPin =  13;      // the number of the LED pin
const int ledPin =  4;      // the number of the LED pin


// variables will change:
int buttonStartState = 0;         // variable for reading the pushbutton status
int buttonBState = 0;         // variable for reading the pushbutton status
int buttonAState = 0;         // variable for reading the pushbutton status

int buttonCUPState = 0;         // variable for reading the pushbutton status
int buttonCDState = 0;         // variable for reading the pushbutton status
int buttonCLState = 0;         // variable for reading the pushbutton status
int buttonCRState = 0;         // variable for reading the pushbutton status

int buttonDUPState = 0;         // variable for reading the pushbutton status
int buttonDDState = 0;         // variable for reading the pushbutton status
int buttonDLState = 0;         // variable for reading the pushbutton status
int buttonDRState = 0;         // variable for reading the pushbutton status

int buttonSRState = 0;         // variable for reading the pushbutton status
int buttonSLState = 0;         // variable for reading the pushbutton status

int buttonZState = 0;         // variable for reading the pushbutton status

int VRx = 39;
int VRy = 34;
int xPosition = 0;
int yPosition = 0;
int mapX = 0;
int mapY = 0;

int printSticks = 1;
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)


void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BTN_START_PIN, INPUT);
  pinMode(BTN_B_PIN, INPUT_PULLDOWN);
  pinMode(BTN_A_PIN, INPUT_PULLDOWN);


  pinMode(BTN_CUP_PIN, INPUT_PULLDOWN);
  pinMode(BTN_CL_PIN, INPUT_PULLDOWN);
  pinMode(BTN_CR_PIN, INPUT_PULLDOWN);
  pinMode(BTN_CD_PIN, INPUT_PULLDOWN);

  pinMode(BTN_DUP_PIN, INPUT_PULLDOWN);
  pinMode(BTN_DL_PIN, INPUT_PULLDOWN);
  pinMode(BTN_DR_PIN, INPUT_PULLDOWN);
  pinMode(BTN_DD_PIN, INPUT_PULLDOWN);

  pinMode(BTN_SR_PIN, INPUT_PULLDOWN);
  pinMode(BTN_SL_PIN, INPUT_PULLDOWN);
  pinMode(BTN_Z_PIN, INPUT_PULLDOWN);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);


  Serial.begin(115200);

}

void loop() {
  // read the state of the pushbutton value:
  buttonStartState = digitalRead(BTN_START_PIN);
  buttonBState = digitalRead(BTN_B_PIN);
  buttonAState = digitalRead(BTN_A_PIN);

  buttonCUPState = digitalRead(BTN_CUP_PIN);
  buttonCLState = digitalRead(BTN_CL_PIN);
  buttonCRState = digitalRead(BTN_CR_PIN);
  buttonCDState = digitalRead(BTN_CD_PIN);

  buttonDUPState = digitalRead(BTN_DUP_PIN);
  buttonDLState = digitalRead(BTN_DL_PIN);
  buttonDRState = digitalRead(BTN_DR_PIN);
  buttonDDState = digitalRead(BTN_DD_PIN);

  buttonSLState = digitalRead(BTN_SL_PIN);
  buttonSRState = digitalRead(BTN_SR_PIN);
  buttonZState = digitalRead(BTN_Z_PIN);

  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonBState == HIGH) {
    Serial.println("B");
    digitalWrite(ledPin, HIGH);

  }
  
    else if (buttonStartState == HIGH) {
    Serial.println("START");
  digitalWrite(ledPin, HIGH);
    }
  
  else if (buttonAState == HIGH) {
    Serial.println("A");
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonSRState == HIGH) {
    Serial.println("SR");
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonSLState == HIGH) {
    Serial.println("SL");
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonZState == HIGH) {
    Serial.println("Z");
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  }

  //~~~~~~~C PAD
  else if (buttonCUPState == HIGH) {
    Serial.println("CUP");
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonCDState == HIGH) {
    Serial.println("CD");
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonCLState == HIGH) {
    Serial.println("CL");
    digitalWrite(ledPin, HIGH);
  }
  else if (buttonCRState == HIGH) {
    Serial.println("CR");
    digitalWrite(ledPin, HIGH);
  }

  //~~~~~~~D PAD
  else if (buttonDUPState == HIGH) {
    Serial.println("DUP");
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonDDState == HIGH) {
    Serial.println("DD");
    digitalWrite(ledPin, HIGH);
  }

  else if (buttonDLState == HIGH) {
    Serial.println("DL");
    digitalWrite(ledPin, HIGH);
  }
  else if (buttonDRState == HIGH) {
    Serial.println("DR");
    digitalWrite(ledPin, HIGH);
  }

  else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }

unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    Serial.print("X: ");
    Serial.print(mapX);
    Serial.print(" | Y: ");
    Serial.println(mapY);
  }
}
