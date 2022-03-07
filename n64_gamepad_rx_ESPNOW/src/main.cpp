#include <esp_now.h>
#include <WiFi.h>
//=============================================================================
//

// 16 servo objects can be created on the ESP32

int x_pos = 0;
int y_pos = 0; // variable to store the servo position

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33

//=============================================================================
// receive data structure
// Must match the sender structure
typedef struct data_structure
{
  int x;
  int y;
  // bool isButtonPressed;
  char buttonCMD[3];
} data_structure;

// Create a struct_message called receivedData
data_structure receivedData;

char pressedButton[3] = "0";


//=============================================================================
// callback function that will be executed when ESP-NOW data is received
void printRecieved(int xRecieved, int yRecieved, int len)
{
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(receivedData.x);
  Serial.print("y: ");
  Serial.println(receivedData.y);
  // Serial.print("isButtonPressed: ");
  // Serial.println(receivedData.isButtonPressed);
  Serial.print("pressedButton: ");
      strcpy(pressedButton,receivedData.buttonCMD);

Serial.print(pressedButton);
  //Serial.println(receivedData.buttonCMD);
  Serial.println();
}

void handleInput(int xRecieved, int yRecieved, char buttonPressed)
{
  if (buttonPressed == 'A')
  {
    // DO SOMETHING
    Serial.print("HANDLE A");
  }
  else if(buttonPressed == 'S')
  {
    // DO SOMETHING ELSE
    Serial.print("HANDLE START");
  }
  // y_servo.write(90 + int(receivedData.x));
  // x_servo.write(90 + int(receivedData.y));
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  printRecieved(int(receivedData.x), int(receivedData.y), len);
  handleInput(int(receivedData.x), int(receivedData.y), char(receivedData.buttonCMD[3]));
}

void setup()
{
  /*
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  x_servo.setPeriodHertz(50);    // standard 50 hz servo
  x_servo.attach(x_servoPin, 100, 2400);
  y_servo.setPeriodHertz(50);    // standard 50 hz servo
  y_servo.attach(y_servoPin, 100, 2400);
  */
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}
//=============================================================================
// END