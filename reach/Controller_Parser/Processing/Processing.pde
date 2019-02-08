import com.studiohartman.jamepad.ControllerManager;
import processing.serial.*; //import the Serial library
Serial myPort;  //the Serial port object

char last_input = ' ';

ControllerManager controllers;
void setup() {
  controllers = new ControllerManager();
  controllers.initSDLGamepad();
  size(200, 200); //make our canvas 200 x 200 pixels big
  //  initialize your serial port and set the baud rate to 9600
  for(String s : Serial.list()) {
    System.out.println("SERIAL: " + s);
  }
  // can change 1 to another number as 1 is the serial port number
  myPort = new Serial(this, Serial.list()[0], 9600);
}

void draw() {
  if ( myPort.available() > 0) {  // If data is available,
    String val = myPort.readStringUntil('\n'); 
    println(val); //print it out in the console
  } 
  ControllerState currState = controllers.getState(0);
  if (currState.a) {
    send('B');
  }
  else if (currState.b) {
    send('A');
  }
  else if (currState.x) {
    send('Y');
  }
  else if (currState.y) {
    send('X');
  }
  else if (currState.dpadUp) {
    send('U');
  }
  else if (currState.dpadDown) {
    send('D');
  }
  else if (currState.dpadLeft) {
    send('L');
  }
  else if (currState.dpadRight) {
    send('R');
  }
  else if (currState.lb) {
    send('l');
  }
  else if (currState.rb) {
    send('r');
  }
  else if (currState.start) {
    send('s');
  }
  else if (currState.back) {
    send('b');
  }
  else {
    send(' ');
  }
}

void send(char c) {
  if (c != last_input) {
    myPort.write(c); 
    last_input = c;
  }
}
