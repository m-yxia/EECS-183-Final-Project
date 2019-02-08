char val; // Data received from the serial port

void setup() {
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600); 
}

void loop() { 
  if (Serial.available()) { // If data is available to read,
    val = Serial.read(); // read it and store it in val
    //Serial.println(val);

    // if we get a B
    if (val == 'B') {
       Serial.println("B");
    }
    else if (val == 'A') {
      Serial.println("A");
    }
    else if (val == 'X') {
      Serial.println("X");
    }
    else if (val == 'Y') {
      Serial.println("Y");
    }
    else if (val == 'U') {
      Serial.println("U");
    }
    else if (val == 'D') {
      Serial.println("D");
    }
    else if (val == 'L') {
      Serial.println("L");
    }
    else if (val == 'R') {
      Serial.println("R");
    }
    else if (val == 'l') {
      Serial.println("lb");
    }
    else if (val == 'r') {
      Serial.println("rb");
    }
    
    delay(100);
  } 
    else {
      Serial.println("Chicken nuggets"); //send back 
      delay(100);
    }
}
