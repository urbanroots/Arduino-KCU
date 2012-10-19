#include <Encoder.h>

#define TICKS_PER_REV 48

//*** Define Variables ***//

// Define Encoder Variables
Encoder knobLeft(20,21);
Encoder knobRight(18,19);

long currentLeft, currentRight;
long positionLeft  = 1;
long positionRight = 1;
long powerState = 96;
long newPowerState = 96;
//int numRevs = 0;
//int numHalfRevs = 0;
//int numHalfRevsRight = 0;
long error1 = 0;
long error2 = 0;

// Serial Contol Variables

const char leftCalLeft = 'a';
const char leftCalRight = 'b';
const char rightCalLeft = 'c';
const char rightCalRight = 'd';
const char stopMotors = 'S';

const char turn = 't';
const char power = 'p';
const char depower = 'd';
const char Speed = 's';
char terminateChar = '/';
const char handshake = 'x';
String command = "";
String positionStr = "";
String motorSpeedStr = "";
String powerStr = "";


int lastState = 0;
int currentState =0;
char incomingByte = 'a';
int count = 0;

long leftMax = 20*TICKS_PER_REV;
long rightMax = 20*TICKS_PER_REV;

float powerMax = 500;
float depowerMax = 500;
float horizontalCount = 50;
float deltaX = 0.001;
long newPositionR = 96;
long newPositionL = 96;
long lastPosition = 0;
long turnLeftPosition = 0;
long turnRightPosition = 0;
long currentPosition = 0;
float howFar = 0.0;


// Motor pin connections for PWNM control 
//  ** THESE PINS MUST BE INITIALIZED ** // 

// TODO - ASSIGN PIN MAPPINGS TO MOTOR DRIVE PINS

int pwmPinL = 10;
int pwmPinR = 11;

int leftMotorLeft = 40;
int leftMotorRight = 41;

int rightMotorLeft = 34;
int rightMotorRight = 36;

int maxSpeed = 255;
int motorSpeed = 255;

boolean calibrating = false;
boolean powerChange = false;
boolean leftMovingLeft = false;
boolean leftMovingRight = false;
boolean rightMovingLeft = false;
boolean rightMovingRight = false;
boolean movingLeft = false;
boolean movingRight = false;
boolean changeDirection = false;
boolean atSetPoint = false;
boolean print2screen = true;


// Define Functions 

void turnLeft(int motorSpeed = maxSpeed)
{
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
 
  digitalWrite(leftMotorLeft, HIGH);
  digitalWrite(leftMotorRight,LOW);
  analogWrite(pwmPinL, motorSpeed);
  
  digitalWrite(rightMotorLeft, LOW);
  digitalWrite(rightMotorRight, HIGH);
  analogWrite(pwmPinR, motorSpeed);
  
}

void leftMotorL(int motorSpeed = maxSpeed)
{
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
  
  digitalWrite(leftMotorLeft,HIGH);
  digitalWrite(leftMotorRight, LOW);
  analogWrite(pwmPinL, motorSpeed);
}

void rightMotorL(int motorSpeed = maxSpeed)
{
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
  
  digitalWrite(rightMotorLeft, HIGH);
  digitalWrite(rightMotorRight, LOW);
  analogWrite(pwmPinR, motorSpeed);
}


void leftMotorR(int motorSpeed = maxSpeed)
{
  
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
  
  digitalWrite(leftMotorLeft, LOW);
  digitalWrite(leftMotorRight,HIGH);
  analogWrite(pwmPinL, motorSpeed);
}

void rightMotorR(int motorSpeed = maxSpeed)
{
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
  
  digitalWrite(rightMotorLeft, LOW);
  digitalWrite(rightMotorRight, HIGH);
  analogWrite(pwmPinR, motorSpeed);
}

void turnRight(int motorSpeed = maxSpeed)
{
  
  if(motorSpeed >255) motorSpeed = 255;
  if(motorSpeed < 0) motorSpeed = 0;
  
  digitalWrite(leftMotorLeft, LOW);
  digitalWrite(leftMotorRight,HIGH);
  analogWrite(pwmPinL, motorSpeed);
  
  digitalWrite(rightMotorLeft, HIGH);
  digitalWrite(rightMotorRight, LOW);
  analogWrite(pwmPinR, motorSpeed);
}

void holdPosition()
{
  // TODO 
  digitalWrite(leftMotorLeft, LOW);
  digitalWrite(leftMotorRight,LOW);
  analogWrite(pwmPinL, 0);
  
  digitalWrite(rightMotorLeft, LOW);
  digitalWrite(rightMotorRight, LOW);
  analogWrite(pwmPinR, 0);
  
}

void stopLeft()
{
  digitalWrite(leftMotorLeft, LOW);
  digitalWrite(leftMotorRight,LOW);
  analogWrite(pwmPinL, 0);
}

void stopRight()
{
  digitalWrite(rightMotorLeft, LOW);
  digitalWrite(rightMotorRight, LOW);
  analogWrite(pwmPinR, 0);
}



void setup() {
  
  Serial.begin(115200);   // open serial port 
  Serial.flush();
  Serial.println("Kite Control 1.0");

  knobLeft.write(TICKS_PER_REV*12);
  knobRight.write(TICKS_PER_REV*12);
  
}

void loop() {
  
  // Do stuff if data is available to read
  if(Serial.available() > 0)
  {
    command = "";
    incomingByte = Serial.read();
    
    while(incomingByte != terminateChar)  // Read from serial until terminating char is recieved
    {
      if(Serial.available() > 0){
        //Serial.println(incomingByte);
        //delay(1000);
        command += incomingByte;    // concatonate command with incoming byte
        incomingByte = Serial.read();  // Read byte from serial port
      }
    }
    Serial.write('x');        // done reading data, so give PC ok to write more data
    
    //** Parse serial command **//
    
    // Check first index in incoming command 
    if(command[0] == turn)
    {  
      positionStr = command.substring(1);  // assigns rest of position command string 
      turnLeftPosition = - long(positionStr.toFloat());  // convert to float   
      turnRightPosition = -turnLeftPosition;
      if(turnLeftPosition > 20) turnLeftPosition = 20;
      if(turnLeftPosition < -20) turnLeftPosition = -20;
      if(turnRightPosition > 20) turnRightPosition = 20;
      if(turnRightPosition < -20) turnRightPosition = -20;
      
      newPositionL = turnLeftPosition + powerState;
      newPositionR = turnRightPosition + powerState;
      //Serial.print("\nNew Position: ");    // Comment out when serial is via XBEE
      //Serial.print("L: ");
      //Serial.print(long(newPositionL));       // Comment out when serial is via XBEE 
      //Serial.print(" R: ");
      //Serial.println(long(newPositionR));       // Comment out when serial is via XBEE 
      print2screen = true;

    }
    if(command[0] == power)
    {
      powerStr = command.substring(1);
      newPowerState = int(powerStr.toFloat());
      powerState = newPowerState;
      powerChange = true;
      newPositionL = turnLeftPosition + newPowerState;
      newPositionR = turnRightPosition + newPowerState;
    }
    
    
    if(command[0] == Speed) 
    {
      motorSpeedStr = command.substring(1);  // assings rest of speed command to string
      motorSpeed = int(motorSpeedStr.toFloat()); // convert to int
      if(motorSpeed > 255) motorSpeed = 255;
      if(motorSpeed < 0 ) motorSpeed = 0;
      if(leftMovingLeft) leftMotorL(motorSpeed);
      if(leftMovingRight) leftMotorR(motorSpeed);
      if(rightMovingLeft) rightMotorL(motorSpeed);
      if(rightMovingRight) rightMotorR(motorSpeed);
      //Serial.print("New Motor Speed: ");
      //Serial.println(motorSpeed);
    }
    
    if(command[0] == leftCalLeft) {
      leftMotorL(75);
      calibrating = true;
    }
    if(command[0] == leftCalRight){
      leftMotorR(75);
      calibrating = true;
    }
    if(command[0] == rightCalLeft){
      rightMotorL(75);
      calibrating = true;
    }
    if(command[0] == rightCalRight){
      rightMotorR(75);
      calibrating = true;
    } 
    if(command[0] == stopMotors){    // reset all values to default 
      stopLeft();
      stopRight(); 
      knobLeft.write(TICKS_PER_REV*12);
      knobRight.write(TICKS_PER_REV*12);
      powerState = 96;
      newPositionL = 96;
      newPositionR = 96;
      calibrating = false;
    }
  }
  
   // Encoder tracking
  // Determine Current position 
  
  
  
  // Compare current Position to setPostion
  // If current setPosition is less than current position Move left
  // Else Move right
  
  currentLeft = knobLeft.read();    
  currentRight = knobRight.read();
  
  long numRevs = currentLeft/TICKS_PER_REV;
  long numHalfRevs = currentLeft/(TICKS_PER_REV/8);
  long numHalfRevsRight = currentRight/(TICKS_PER_REV/8);
  
 
  
  if(!calibrating)
  {
    if(newPositionL < numHalfRevs)    // reel IN line 
      { 
        leftMotorL(motorSpeed);
        leftMovingLeft = true;
        leftMovingRight = false;
        rightMovingLeft = false;
        rightMovingRight = false;
        
      }
      if(newPositionR < numHalfRevsRight)  // reel OUT line
      {
  
        rightMotorR(motorSpeed);
        leftMovingLeft = false;
        leftMovingRight = false;
        rightMovingLeft = false;
        rightMovingRight = true;
      }
      
      if(newPositionL > numHalfRevs)  // reel OUT line
      { 
  
        leftMotorR(motorSpeed);
        leftMovingLeft = false;
        leftMovingRight = true;
        rightMovingLeft = false;
        rightMovingRight = false;
      }
      if(newPositionR > numHalfRevsRight)  // reel IN line
      {
        rightMotorL(motorSpeed);
        leftMovingLeft = false;
        leftMovingRight = false;
        rightMovingLeft = true;
        rightMovingRight = false;
      }
      
      if(newPositionL == numHalfRevs){
        stopLeft();
        //powerChange = false;
      }
      if(newPositionR == numHalfRevsRight) {
        stopRight();
       // powerChange = false;
      }
  

  
  
  
    if(newPositionL == numHalfRevs && numHalfRevsRight == newPositionL && print2screen)
    {   
       //Serial.println("Reached Set Point");
    }
       
       
     if(numHalfRevs!= positionLeft || numHalfRevsRight != positionRight)
     {
//       Serial.print("Revs - L:");
//       Serial.print(numHalfRevs);
//       Serial.print(" R: ");
//       Serial.println(numHalfRevsRight);
       positionLeft = numHalfRevs; 
       positionRight = numHalfRevsRight;
     }
     
  }
     
  // TODO 
  // Check battery levels of each cell in LiPo Pack
  // if they are low cut power to motors and send Message to PC 
  
  // TODO
  // Read accelerometer/gyro data  
  
  
  
}

