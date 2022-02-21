void handleButtons()
{
  int currentButton1State = digitalRead(R1);
  int currentButton2State = digitalRead(R2);
  int currentButton3State = digitalRead(R3);
  int currentButton4State = digitalRead(R4);
  int currentDPadUpState = digitalRead(DUP);
  int currentDPadLeftState = digitalRead(DLEFT);
  int currentDPadDownState = digitalRead(DDOWN);
  int currentDPadRightState = digitalRead(DRIGHT);

  int currentLSState = digitalRead(LS);
  int currentRSState = digitalRead(RS);
  //-------------------HANDLE BUTTONS

  //--RIGHT SIDE BUTTONS; ZBXY
  if (currentButton1State != previousButton1State)
  {
    if (currentButton1State == LOW)
    {
      Serial.println(F("Button 1"));
            strcpy(myData.buttonCMD, "B1");
      sendTheReport();

    }

  }
  previousButton1State = currentButton1State;

  if (currentButton2State != previousButton2State)
  {
    if (currentButton1State == LOW)
    {
      Serial.println(F("Button 2"));
            strcpy(myData.buttonCMD, "B2");
      sendTheReport();

    }

  }
  previousButton2State = currentButton2State;

  if (currentButton3State != previousButton3State)
  {
    if (currentButton1State == LOW)
    {
      Serial.println(F("Button 3"));
            strcpy(myData.buttonCMD, "B3");
      sendTheReport();

    }

  }
  previousButton3State = currentButton3State;

  if (currentButton4State != previousButton4State)
  {
    if (currentButton1State == LOW)
    {
      Serial.println(F("Button 4"));
            strcpy(myData.buttonCMD, "B4");
      sendTheReport();
    }

  }
  previousButton4State = currentButton4State;

  //----------D-PAD
  if (currentDPadUpState != previousDPadUpState)
  {
    if (currentDPadUpState == LOW)
    {
      Serial.println(F("DPAD UP"));
            strcpy(myData.buttonCMD, "DUP");
      sendTheReport();
    }

  }
  previousDPadUpState = currentDPadUpState;

  if (currentDPadLeftState != previousDPadLeftState)
  {
    if (currentDPadLeftState == LOW)
    {
      Serial.println(F("DPAD_LEFT"));
            strcpy(myData.buttonCMD, "DLEFT");
      sendTheReport();
    }

  }
  previousDPadLeftState = currentDPadLeftState;

  if (currentDPadDownState != previousDPadDownState)
  {
    if (currentDPadDownState == LOW)
    {
      Serial.println(F("DPAD_DOWN"));
            strcpy(myData.buttonCMD, "DDOWN");
      sendTheReport();

    }

  }
  previousDPadDownState = currentDPadDownState;

  if (currentDPadRightState != previousDPadRightState)
  {
    if (currentDPadRightState == LOW)
    {
      Serial.println(F("DPAD_RIGHT"));
            strcpy(myData.buttonCMD, "DUP");
      sendTheReport();

    }

  }
  previousDPadRightState = currentDPadRightState;

  //----------STICKS
  if (currentLSState != previousLSState)
  {
    if (currentLSState == LOW)
    {
      Serial.println(F("LEFT STICK"));
      strcpy(myData.buttonCMD, "LS");
      sendTheReport();


    }

  }
  previousLSState = currentLSState;
  //------------------
  if (currentRSState != previousRSState)
  {
    if (currentRSState == LOW)
    {
      Serial.println(F("RIGHT STICK"));
            strcpy(myData.buttonCMD, "RS");
      sendTheReport();
    }
  }
  previousRSState = currentRSState;
}

void handleJoysticks()
{
  //---JOYSTICKS

  // Map analog reading from 0 ~ 4095 to 32737 ~ -32737 for use as an axis reading
  leftJoyHoriz = map(analogRead(LH), 0, 4095, 32737, -32737);
  leftJoyVert = map(analogRead(LV), 0, 4095, 32737, -32737);

  rightJoyHoriz = map(analogRead(RH), 0, 4095, 32737, -32737);
  rightJoyVert = map(analogRead(RV), 0, 4095, 32737, -32737);

   myData.leftStickX = leftJoyHoriz;
  myData.leftStickY = leftJoyVert;
  myData.RightStickX = rightJoyHoriz;
  myData.RightStickY = rightJoyVert;

}
