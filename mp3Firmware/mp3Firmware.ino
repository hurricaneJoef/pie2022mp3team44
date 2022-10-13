#include <Adafruit_MotorShield.h>

#define cleft_pin A2
#define crigt_pin A1
#define fleft_pin A0
#define frigt_pin A3

#define cleft_led_pin 4
#define fleft_led_pin 5
#define crigt_led_pin 7
#define frigt_led_pin 6

#define left_motor_pin 2
#define right_motor_pin 1
#define thresh_on 690
#define thresh_off 400
#define left_speed 50
#define right_speed 50
#define cweght 2
#define fweght 4
#define tweght 50

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *left = AFMS.getMotor(left_motor_pin);
Adafruit_DCMotor *right = AFMS.getMotor(right_motor_pin);

// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(cleft_pin,INPUT);
pinMode(cleft_led_pin,OUTPUT);
pinMode(fleft_pin,INPUT);
pinMode(fleft_led_pin,OUTPUT);
pinMode(crigt_pin,INPUT);
pinMode(crigt_led_pin,OUTPUT);
pinMode(frigt_pin,INPUT);
pinMode(frigt_led_pin,OUTPUT);
 if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
digitalWrite(fleft_led_pin,HIGH);
digitalWrite(cleft_led_pin,HIGH);
digitalWrite(crigt_led_pin,HIGH);
digitalWrite(frigt_led_pin,HIGH);
left->run(BACKWARD);
right->run(BACKWARD);
left->setSpeed(left_speed);
right->setSpeed(right_speed);
delay(100);
left->run(RELEASE);
right->run(RELEASE);
delay(5000);
}

void motors(int l,int r){
  if(l<0){
    left->run(BACKWARD);
  }else{
    left->run(FORWARD);
  }
  if(r<0){
    right->run(BACKWARD);
  }else{
    right->run(FORWARD);
  }
  left->setSpeed(l);
  right->setSpeed(r);
  Serial.print("|  ");
  Serial.print(l);
  Serial.print(", ");
  Serial.print(r);
}


int last_dir = false;// false is right true is left

void loop() {
  // put your main code here, to run repeatedly:
  int val_cl = analogRead(cleft_pin);
  int val_fr = analogRead(frigt_pin);
  int val_cr = analogRead(crigt_pin);
  int val_fl = analogRead(fleft_pin);
  Serial.print(val_fl);
  Serial.print(", ");
  Serial.print(val_cl);
  Serial.print(", ");
  Serial.print(val_cr);
  Serial.print(", ");
  Serial.print(val_fr);
  int f_ratio;
  int c_ratio;
  c_ratio = (val_cr-val_cl)*cweght;
  f_ratio = (val_fr-val_fl)*fweght;
  Serial.print(",  ");
  Serial.print(c_ratio);
  Serial.print(", ");
  Serial.print(f_ratio);
  int cr_clean = round(10*c_ratio*(f_ratio/abs(f_ratio)));
  if(cr_clean == 0){
    cr_clean = 10*cweght;
  }
  int t_ratio = round((
   //               (f_ratio*10/cr_clean)+
                  c_ratio+
                  f_ratio
                  )/tweght);
  Serial.print(",  ");
  Serial.print(t_ratio);
  int lsped = round(left_speed+(t_ratio));
  int rsped = round(right_speed-(t_ratio));
  motors(lsped,rsped);
  /*
  if(val_fl > thresh_on){//on the line
    left->setSpeed(left_speed);
    right->setSpeed(right_speed);
    if(val_fr > thresh_on){
      right->run(RELEASE);
      right->run(BRAKE);
      left->run(RELEASE);
      left->run(BRAKE);
      delay(1000);
      right->run(FORWARD);
      left->run(FORWARD);
    }else if(val_fr< thresh_off){
      right->run(FORWARD);
      left->run(BACKWARD);
    }
  }else if(val_fl< thresh_off){// off the line
    if(val_fr > thresh_on){
      left->setSpeed(left_speed);
      right->setSpeed(right_speed);
      right->run(BACKWARD);
      left->run(FORWARD);
    }else if(val_fr< thresh_off){
        int ratio = val_cr-val_cl; 
        int lsped = round(left_speed+(ratio/oneoverp));
        int rsped = round(right_speed-(ratio/oneoverp));
        motors(lsped,rsped);
      
    }
  }//*/
  Serial.println(".");
}
