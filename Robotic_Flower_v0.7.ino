#include <NewPing.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 20000 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SERVOMIN  130 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  350 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int numReadings = 50;
int val;                      // variable to read the value from the sonar analog pin
int servoval;
int readings[numReadings];
int readIndex = 0;              // the index of the current reading
int total = 0;                  // running total to calculat sonar average distance
int average = 0;                // the average sonar distance reading

// our servo # counter
uint8_t servonum = 0;           //this just used internally, doesn't reflet number of servos
int Pulse = 0;                  //PWM frequency value for controlling servos

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  // initialize all the sonar readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  
  pwm.setPWM(0, 0, Pulse);
  pwm.setPWM(1, Pulse, 0);
  pwm.setPWM(2, Pulse, 0);
  pwm.setPWM(3, 0, Pulse);
  pwm.setPWM(4, 0, Pulse);
  
  Pulse = SERVOMIN;
  //yield();
}

void shy_flower() {
    ////////////////////////////////////////////////////////////////////////////
    //Servo Closing and slowly opening mvmt
    ////////////////////////////////////////////////////////////////////////////
    //Turn off blue led
    pwm.setPWM(13,0, 4096);
    
    //Quickly close the flower
    for (Pulse = SERVOMAX-1; Pulse >= SERVOMIN; Pulse--)
    {
    Serial.print("Pulse: ");
    Serial.println(Pulse);
    pwm.setPWM(0, 0, Pulse);
    pwm.setPWM(1, 0, Pulse);
    pwm.setPWM(2, 0, Pulse);
    pwm.setPWM(3, 0, Pulse);
    pwm.setPWM(4, 0, Pulse);
    delay(50);
    }

    //Turn the yellow leds on slowly
    for (int i=0; i<4095; i++)
    {
    pwm.setPWM(12,1, i);
    }
    
    //Slowly open the petals
    for (Pulse = SERVOMIN; Pulse < SERVOMAX; Pulse++)
    {
    Serial.print("Pulse: ");
    Serial.println(Pulse);
    pwm.setPWM(0, 0, Pulse);
    delay(40);
    pwm.setPWM(1, 0, Pulse);
    delay(40);
    pwm.setPWM(2, 0, Pulse);
    delay(40);
    pwm.setPWM(3, 0, Pulse);
    delay(40);
    pwm.setPWM(4, 0, Pulse);
    delay(100);
    }
    
    //Turn the yellow leds off slowly
    for (int i=4095; i>0; i--)
    {
    pwm.setPWM(12,1, i);
    }
    
    get_sonar();
    loop();
}

void happy_flower() {
    ////////////////////////////////////////////////////////////////////////////
    //Servo Pulsing Movement
    /////////////////////////////////////////////////////////////////////////////
    
  for (Pulse = SERVOMAX-1; Pulse >= SERVOMIN+180; Pulse--)
  {
  Serial.print("Pulse: ");
  Serial.println(Pulse);
  pwm.setPWM(0, 0, Pulse);
  pwm.setPWM(1, 0, Pulse);
  pwm.setPWM(2, 0, Pulse);
  pwm.setPWM(3, 0, Pulse);
  pwm.setPWM(4, 0, Pulse);
  delay(30);
  }
    
  //Turn on blue leds slowly
  for (int i=500; i<4095; i = i+5)
  {
  pwm.setPWM(13,1, i);
  }

  for (Pulse = SERVOMIN+180; Pulse < SERVOMAX; Pulse++)
  {
  Serial.print("Pulse: ");
  Serial.println(Pulse);
  pwm.setPWM(0, 0, Pulse);
  pwm.setPWM(2, 0, Pulse);
  pwm.setPWM(1, 0, Pulse);
  pwm.setPWM(4, 0, Pulse);
  pwm.setPWM(3, 0, Pulse);
  delay(40);
  }

  //turn off blue leds slowly
  for (int i=4094; i>500; i=i-5)
  {
  pwm.setPWM(13,1, i);
  }
    
  get_sonar();
  loop();
}

void get_sonar() {
  ///////////////////////////////////////////////////////////////////////////
  //Get reading from Sonar sensor
  //////////////////////////////////////////////////////////////////////////
  for (int i=0; i<10; i++)
  {
    unsigned int uS = sonar.ping();                   // Send ping, get ping time in microseconds (uS).
    int distance = sonar.convert_cm(uS);
     
    total = total - readings[readIndex];              // subtract the last reading:
    readings[readIndex] = distance;                   // sensor distance value
    total = total + readings[readIndex];              // add the reading to the total:
    readIndex = readIndex + 1;                        // advance to the next position in the array:
  
    if (readIndex >= numReadings) {                   // if we're at the end of the array...
      readIndex = 0;                                  // ...wrap around to the beginning:
    }
    
    average = total / numReadings;                    // calculate the average:
  
    val = map(average, 20, 200, 10, 90);              // scale it to use it with the servo (value between 0 and 90)
    //servoval = constrain(val, 10, 90);
    //myservo.write(servoval);                          // sets the servo position according to the scaled value
    Serial.print("Sonar Average Distance: ");
    Serial.print(average);
    Serial.print("      ");
    Serial.print("Val: ");
    Serial.println(val);
  }
}

void loop() {
  get_sonar();
  if (average<20)
  {
    shy_flower();
  }
  
  else 
  {
    happy_flower();
  }
  ////////////////////////////////////////////////////////////////////////////
}
