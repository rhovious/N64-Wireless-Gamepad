//-------------------HANDLE BUTTONS

//--RIGHT SIDE BUTTONS; ZBXY
if (currentStartState != previousStartState)
{
    if (currentStartState == HIGH)
    {
        Serial.println(F("START"));

        strcpy(pressedButton, "S");
    }
    previousStartState = currentStartState;
}

else if (currentZState != previousZState)
{
    if (currentZState == LOW)
    {
        Serial.println(F("Z"));
        strcpy(pressedButton, "Z");
    }
    previousZState = currentZState;
}

else if (previousLState != currentLState)
{
    if (currentLState == LOW)
    {
        Serial.println(F("L"));
        strcpy(pressedButton, "L");
    }
    previousLState = currentLState;
}

else if (currentBState != previousBState)
{
    if (currentBState == LOW)
    {
        Serial.println(F("B"));
        strcpy(pressedButton, "B");
    }
    previousBState = currentBState;
}

else if (currentAState != previousAState)
{
    if (currentAState == LOW)
    {
        Serial.println(F("A"));
        strcpy(pressedButton, "A");
    }
    previousAState = currentAState;
}

// handles no buttons pressed

else if (currentRState != previousRState)
{
    if (currentRState == LOW)
    {
        Serial.println(F("R"));
        strcpy(pressedButton, "R");
    }
    previousRState = currentRState;
}

//----------C-PAD

else if (currentCDState != previousCDState)
{
    if (currentCDState == LOW)
    {
        Serial.println(F("C DOWN"));
        strcpy(pressedButton, "CD");
    }
    previousCDState = currentCDState;
}

else if (currentCLState != previousCLState)
{
    if (currentCLState == LOW)
    {
        Serial.println(F("C LEFT"));
        strcpy(pressedButton, "CL");
    }
    previousCLState = currentCLState;
}

else if (currentCUState != previousCUState)
{
    if (currentCUState == LOW)
    {
        Serial.println(F("C UP"));
        strcpy(pressedButton, "CU");
    }
    previousCUState = currentCUState;
}

else if (currentCRState != previousCRState)
{
    if (currentCRState == LOW)
    {
        Serial.println(F("C RIGHT"));
        strcpy(pressedButton, "CR");
    }
    previousCRState = currentCRState;
}

//----------D-PAD
else if (currentDPadUpState != previousDPadUpState)
{
    if (currentDPadUpState == LOW)
    {
        Serial.println(F("DPAD UP"));
        strcpy(pressedButton, "DU");
    }
    previousDPadUpState = currentDPadUpState;
}

else if (currentDPadLeftState != previousDPadLeftState)
{
    if (currentDPadLeftState == LOW)
    {
        Serial.println(F("DPAD_LEFT"));
        strcpy(pressedButton, "DL");
    }
    previousDPadLeftState = currentDPadLeftState;
}

else if (currentDPadDownState != previousDPadDownState)
{
    if (currentDPadDownState == LOW)
    {
        Serial.println(F("DPAD_DOWN"));
        strcpy(pressedButton, "DD");
    }
    previousDPadDownState = currentDPadDownState;
}

else if (currentDPadRightState != previousDPadRightState)
{
    if (currentDPadRightState == LOW)
    {
        Serial.println(F("DPAD_RIGHT"));
        strcpy(pressedButton, "DR");
    }
    previousDPadRightState = currentDPadRightState;
}
else
{
    // turn LED off:
    strcpy(pressedButton, "0");
    digitalWrite(ledPin, LOW);
}
