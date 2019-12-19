#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

const int TRIG_PIN = 2;
const int ECHO_PIN = 3;
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;
const int IN5 = 11;
const int IN6 = 10;
const int IN7 = 9;
const int IN8 = 8;

const int MIN_SPEED = 0;
const int MAX_SPEED = 255;
const int N_DIST = 8;
const int TURN_DIST = 50;

double dist[N_DIST], avg_dist;
int idx_dist = 0;

SoftwareSerial mySoftwareSerial(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// --------------------------------------------------------- END OF CONSTANTS -------------------------------------

void go_forward(int speed) {
  constrain(speed, MIN_SPEED, MAX_SPEED);
  
  digitalWrite(IN1, HIGH);// chân này không có PWM
  analogWrite(IN2, 255 - speed);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);// chân này không có PWM
  
  digitalWrite(IN5, HIGH);// chân này không có PWM
  analogWrite(IN6, 255 - speed);
  analogWrite(IN7, speed);
  digitalWrite(IN8, LOW);// chân này không có PWM
}

void go_backward(int speed) {
  constrain(speed, MIN_SPEED, MAX_SPEED);
  
  digitalWrite(IN2, HIGH);// chân này không có PWM
  analogWrite(IN1, 255 - speed);
  analogWrite(IN4, speed);
  digitalWrite(IN3, LOW);// chân này không có PWM
  
  digitalWrite(IN6, HIGH);// chân này không có PWM
  analogWrite(IN5, 255 - speed);
  analogWrite(IN8, speed);
  digitalWrite(IN7, LOW);// chân này không có PWM
}

void stop() {
  for (int i = 4; i <= 11; ++ i) {
    digitalWrite(i, LOW);
  }
}

double get_distance() {
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(ECHO_PIN, HIGH);
  double new_dist = duration*0.034/2;

  avg_dist += (new_dist - dist[idx_dist]) / N_DIST;
  dist[idx_dist] = new_dist;
  idx_dist ++;
  if (idx_dist == N_DIST) {
    idx_dist = 0;
  }
  Serial.println(avg_dist);
  return avg_dist;
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

// ------------------------------------ MAIN FUNCTIONS ----------------------------------------------------

void setup()
{
  // init distance sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);

  // init speaker
  mySoftwareSerial.begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  // init motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);
  
  // initialize
  myDFPlayer.volume(30);
  myDFPlayer.enableLoopAll();
  go_forward(100);
}

bool current = true; // NOT MOVE

void loop() {
  if (get_distance() <= TURN_DIST) {
    if (current) {
      stop();
      current = false; 
    }
  } else if (!current) {
    go_forward(100);
    current = true;
  }  
}
