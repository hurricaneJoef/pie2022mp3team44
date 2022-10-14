#include <Adafruit_MotorShield.h>

#define cleft_pin A2 // c stands for center
#define crigt_pin A1
#define fleft_pin A0 // f stands for far
#define frigt_pin A3 // i spelt it rigt b/c i wanted it to match the 4 letters of left

#define cleft_led_pin 4 // the pin to turn on the emiter
#define fleft_led_pin 5
#define crigt_led_pin 7
#define frigt_led_pin 6

#define left_motor_pin 2 // the connecter the motor is hooked up to
#define right_motor_pin 1
#define thresh_on 690 //  for previous bang bang code
#define thresh_off 400
#define isave .9 // for the i factor


float p =0.2;//for pid controller
float d =0.0;
float i =0.00;
int cweght =2; //  the weight of the center sensors
int fweght =4; //   the weight of the far sensors
int fspeed =40; // forward speed
int tspeed = 60; //  turning speed


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
pinMode(cleft_pin,INPUT);// init all sensor pins
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
digitalWrite(fleft_led_pin,HIGH);// set the pins high
digitalWrite(cleft_led_pin,HIGH); //  thought i might need to multiplex them but i didnt need to
digitalWrite(crigt_led_pin,HIGH);
digitalWrite(frigt_led_pin,HIGH);
left->run(BACKWARD);
right->run(BACKWARD);
left->setSpeed(fspeed);
right->setSpeed(fspeed); //lurch it so i know its on
delay(100);
left->run(RELEASE);
right->run(RELEASE);
delay(5000); // wait to start
}

void motors(int l,int r){// get around the issue that you cant set a neg speed
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
  left->setSpeed(abs(l)); // and print the speeds
  right->setSpeed(abs(r));
  Serial.print(",|,  ");
  Serial.print(l);
  Serial.print(", ");
  Serial.print(r);
}


int last_t=0;//for d
float d_val;
int i_holder; //keeps track of the i without the coef
void loop() {
  if(Serial.available()>0){//handle serial data
    char c = Serial.read();
    switch(c){
      case 'p'://set p coef
        p = Serial.parseFloat();
        break;
      case 'd'://set p coef
        d = Serial.parseFloat();
        break;
      case 'i'://set p coef
        i = Serial.parseFloat();
        break;
      case 'c'://set center weight coef
        cweght = Serial.parseInt();
        break;
      case 'f'://set far weight coef
        fweght = Serial.parseInt();
        break;
      case 's': //set forward speed
        fspeed = Serial.parseInt();
        break;
      case 't'://set turning speed
        tspeed = Serial.parseInt();
        break;
      case 'v': // print code that i can copy and paste into arduino
                // i could have saved in eeprom but was lazy
        Serial.print("float p =");
        Serial.print(p);
        Serial.println(";");
        Serial.print("float d =");
        Serial.print(d);
        Serial.println(";");
        Serial.print("float i =");
        Serial.print(i);
        Serial.println(";");
        Serial.print("int cweght =");
        Serial.print(cweght);
        Serial.println(";");
        Serial.print("int fweght =");
        Serial.print(fweght);
        Serial.println(";");
        Serial.print("int fspeed =");
        Serial.print(fspeed);
        Serial.println(";");
        Serial.print("int tspeed =");
        Serial.print(tspeed);
        Serial.println(";");
        delay(8000);
        break;
    }
  }
  // put your main code here, to run repeatedly:
  int val_cl = analogRead(cleft_pin);
  int val_fr = analogRead(frigt_pin);
  int val_cr = analogRead(crigt_pin);//read once save time
  int val_fl = analogRead(fleft_pin);
  Serial.print(val_fl);
  Serial.print(", ");
  Serial.print(val_cl);
  Serial.print(", ");
  Serial.print(val_cr);
  Serial.print(", ");
  Serial.print(val_fr);//print vals
  int f_ratio;
  int c_ratio;
  c_ratio = (val_cr-val_cl)*cweght;//get ratios
  f_ratio = (val_fr-val_fl)*fweght;
  Serial.print(",  ");
  Serial.print(c_ratio);
  Serial.print(", ");
  Serial.print(f_ratio);
  int cr_clean = round(c_ratio*(f_ratio/abs(f_ratio)));// this was to remove wierd vals and prep for (f/c)+f+c idea that didnt pan out
  if(cr_clean == 0){
    cr_clean = cweght;
  }
  int t = (//make a sum of c and f differneces 
  //        (f_ratio*10/cr_clean)+// didnt pan out 
          c_ratio+
          f_ratio
          );
  float t_ratio = (p*t);//p value
  
  d_val = (d*(t-last_t));// getting d from last value of t
  last_t = t;// set last t for d next run
  i_holder = round(t+isave*i_holder);//save the integration total for later
  float i_val = (i_holder*i);//get i val
  int lsped = (fspeed);// set values for old code no longer used
  int rsped = (fspeed);
  float mdiff = (t_ratio+d_val+i_val)/500;// get diff to turn by
  Serial.print(",  ");
  Serial.print(mdiff);
  //if(mdiff > 0){
    rsped = round(fspeed-tspeed*mdiff);// set speeds
  //}
  //if(mdiff < 0){
    lsped = round(fspeed+tspeed*mdiff);
  //}
  
  //int rsped = round(right_speed-right_speed*(t_ratio+d_val+i_val));
  motors(lsped,rsped);// and push speeds to motors
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
  Serial.println(".");// end serial line
}
