//============================================================================//
//                        Marcelo Ávila de Oliveira                           //
//                                                                            //
//            MyDroid.ino - Android Figure Control using Arduino              //
//============================================================================//

//============================================================================//
// DEFINITIONS                                                                //
//============================================================================//

// TURN ON DEBUG MODE
// #define DEBUG
// ADD INFRARED REMOTE SUPPORT
// #define IR

//============================================================================//
// LIBRARIES                                                                  //
//============================================================================//

// IR LIBRARY
#ifdef IR
  #include <IRremote.h>
#endif
// SERVO LIBRARY
#include <Servo.h>

//============================================================================//
// VARIABLES                                                                  //
//============================================================================//

// MOTORS
Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

// PINS
int head_pin = 24;
int body_pin = 26;
int left_arm_pin = 28;
int right_arm_pin = 30;
int right_blue_eye_pin = 9;
int right_red_eye_pin = 10;
int left_blue_eye_pin =11;
int left_red_eye_pin = 12;

// POSITIONS
int head_pos;
int body_pos;
int left_arm_pos;
int right_arm_pos;

// TIME
unsigned long current_time;
unsigned long previous_time = 0;
unsigned long time_to_blink = 3000;

// MISCELANEOUS
int step_aux = 5;
int step_aux_2 = 10;
int option;

// IR
#ifdef IR
  IRrecv irrecv(22);
  decode_results results;
#endif

//============================================================================//
// FUNCTIONS (SETUP)                                                          //
//============================================================================//

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  #ifdef IR
    irrecv.enableIRIn();
  #endif

  randomSeed(analogRead(0));
  setup_bluetooth();
  reset(0);
}

void setup_bluetooth()
{
  Serial1.begin(38400);                    // Set baud rate
  Serial1.print("\r\n+STWMOD=0\r\n");      // Set to work in slave mode
  Serial1.print("\r\n+STNA=Avila147\r\n"); // Set name"
  Serial1.print("\r\n+STOAUT=1\r\n");      // Permit Paired device to connect me
  Serial1.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
  delay(2000);                             // This delay is required.
  Serial1.print("\r\n+INQ=1\r\n");         // Make the slave inquirable 
  delay(2000);                             // This delay is required.
  while (Serial1.available()) {            // Clear data
    Serial1.read();
  }
}

//============================================================================//
// FUNCTIONS (BASIC ACTIONS)                                                  //
//============================================================================//

void blink_eyes() {
  analogWrite(right_blue_eye_pin, 200);
  analogWrite(left_blue_eye_pin, 200);
  delay(50);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  delay(300);
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void check_blink() {
  current_time = millis();
  if (current_time - previous_time > time_to_blink) {
    previous_time = current_time;
    time_to_blink = random(2000, 7000);
    blink_eyes();
  }
}

void move(boolean wait, int time, int pin1, int pos1, int new_pos1, int pin2, int pos2, int new_pos2, int pin3, int pos3, int new_pos3, int pin4, int pos4, int new_pos4) {
  boolean continue1 = true;
  boolean continue2 = true;
  boolean continue3 = true;
  boolean continue4 = true;
  boolean moved = false;

  if (pin1 != 0) {
    motor1.attach(pin1);
    motor1.write(pos1);
  } else {
    continue1 = false;
  }
  if (pin2 != 0) {
    motor2.attach(pin2);
    motor2.write(pos2);
  } else {
    continue2 = false;
  }
  if (pin3 != 0) {
    motor3.attach(pin3);
    motor3.write(pos3);
  } else {
    continue3 = false;
  }
  if (pin4 != 0) {
    motor4.attach(pin4);
    motor4.write(pos4);
  } else {
    continue4 = false;
  }

  do
  {
    if (continue1) {
      if (pos1 == new_pos1) {
        continue1 = false;
      } else {
        if (pos1 < new_pos1) {
          pos1 += step_aux;
        } else {
          pos1 -= step_aux;
        }
        motor1.write(pos1);
        moved = true;
      }
    }
    if (continue2) {
      if (pos2 == new_pos2) {
        continue2 = false;
      } else {
        if (pos2 < new_pos2) {
          pos2 += step_aux;
        } else {
          pos2 -= step_aux;
        }
        motor2.write(pos2);
        moved = true;
      }
    }
    if (continue3) {
      if (pos3 == new_pos3) {
        continue3 = false;
      } else {
        if (pos3 < new_pos3) {
          pos3 += step_aux;
        } else {
          pos3 -= step_aux;
        }
        motor3.write(pos3);
        moved = true;
      }
    }
    if (continue4) {
      if (pos4 == new_pos4) {
        continue4 = false;
      } else {
        if (pos4 < new_pos4) {
          pos4 += step_aux;
        } else {
          pos4 -= step_aux;
        }
         motor4.write(pos4);
        moved = true;
      }
    }

    if (moved) {
      delay(time);
      moved = false;
    }
  } while (continue1 || continue2 || continue3 || continue4);

  if (wait == true) {
    delay(100);
  }

  if (pin1 != 0) {
    motor1.detach();
  }
  if (pin2 != 0) {
    motor2.detach();
  }
  if (pin3 != 0) {
    motor3.detach();
  }
  if (pin4 != 0) {
    motor4.detach();
  }
}

void reset(int mode) {
  // MODE:
  // 0 = SET *_POS=RESET AND GOES TO RESET POSITION
  // 1 = GOES TO RESET POSITION
  // 2 = GOES TO *_POS
  motor1.attach(head_pin);
  motor2.attach(body_pin);
  motor3.attach(left_arm_pin);
  motor4.attach(right_arm_pin);

  if (mode == 0) {
    head_pos=90;
    body_pos=90;
    left_arm_pos=160;
    right_arm_pos=0;
  }

  if (mode == 1) {
    motor1.write(90);
    motor2.write(90);
    motor3.write(160);
    motor4.write(0);
  } else {
    motor1.write(head_pos);
    motor2.write(body_pos);
    motor3.write(left_arm_pos);
    motor4.write(right_arm_pos);
  }

  delay(500);

  motor1.detach();
  motor2.detach();
  motor3.detach();
  motor4.detach();

  if (mode == 0) {
    happy_eyes(100, 3);
  }
}

void happy_eyes(int time, int repeat) {
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  for (int i = 0; i < repeat; i++) {
    analogWrite(right_blue_eye_pin, 100);
    analogWrite(left_blue_eye_pin, 100);
    delay(time);
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void angry_eyes(int time, int repeat) {
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  for (int i = 0; i < repeat; i++) {
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 100);
    delay(time);
    analogWrite(right_red_eye_pin, 255);
    analogWrite(left_red_eye_pin, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void scary_eyes(int time, int repeat) {
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  for (int i = 0; i < repeat; i++) {
    analogWrite(right_blue_eye_pin, 100);
    analogWrite(left_blue_eye_pin, 100);
    analogWrite(right_red_eye_pin, 255);
    analogWrite(left_red_eye_pin, 255);
    delay(time);
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 255);
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 100);
    if (i != repeat-1) {
      delay(time);
    }
  }
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void move_check() {
  move(true, 30, head_pin, head_pos, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, head_pin, 10, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, head_pin, 170, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  move(true, 30, body_pin, body_pos, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, body_pin, 10, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, body_pin, 170, body_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  move(true, 30, right_arm_pin, right_arm_pos, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, right_arm_pin, 170, right_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  move(true, 30, left_arm_pin, left_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, left_arm_pin, 0, left_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  delay(300);

  analogWrite(right_red_eye_pin, 240);  
  analogWrite(left_red_eye_pin, 240);         
  delay(300);

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

//============================================================================//
// FUNCTIONS (ADVANCED ACTIONS)                                               //
//============================================================================//

void move_say_hi() {
  move(true, 10, right_arm_pin, right_arm_pos, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  happy_eyes(300, 1);
  delay(500);
  move(true, 30, right_arm_pin, 160, right_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_say_bye() {
  move(true, 10, right_arm_pin, right_arm_pos, 160, head_pin, head_pos, 60, 0, 0, 0, 0, 0, 0);
  analogWrite(left_blue_eye_pin, 255);
  move(true, 10, right_arm_pin, 160, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 10, right_arm_pin, 120, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 10, right_arm_pin, 160, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 10, right_arm_pin, 120, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  analogWrite(left_blue_eye_pin, 240);
  delay(500);
  move(true, 30, right_arm_pin, 170, right_arm_pos, head_pin, 60, head_pos, 0, 0, 0, 0, 0, 0);
}

void move_say_yes() {
  move(true, 10, right_arm_pin, right_arm_pos, 90, left_arm_pin, left_arm_pos, 70, 0, 0, 0, 0, 0, 0);
  happy_eyes(100, 7);
  move(true, 30, right_arm_pin, 90, right_arm_pos, left_arm_pin, 70, left_arm_pos, 0, 0, 0, 0, 0, 0);
}

void move_say_no() {
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  analogWrite(right_red_eye_pin, 240);  
  analogWrite(left_red_eye_pin, 240);         

  move(false, 10, head_pin, head_pos, 30, right_arm_pin, right_arm_pos, 60, 0, 0, 0, 0, 0, 0);
  move(false, 10, head_pin, 30, 140, right_arm_pin, 60, 170, 0, 0, 0, 0, 0, 0);
  move(false, 20, head_pin, 140, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(false, 20, head_pin, 40, 130, right_arm_pin, 170, 80, 0, 0, 0, 0, 0, 0);
  move(false, 20, head_pin, 130, 50, right_arm_pin, 80, 0, 0, 0, 0, 0, 0, 0);
  move(true, 20, head_pin, 50, head_pos, right_arm_pin, 0, right_arm_pos, 0, 0, 0, 0, 0, 0);

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void move_look_left() {
  move(true, 10, head_pin, head_pos, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  happy_eyes(300, 1);
  delay(500);
  move(true, 30, head_pin, 130, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_look_right() {
  move(true, 10, head_pin, head_pos, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  happy_eyes(300, 1);
  delay(500);
  move(true, 30, head_pin, 50, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_turn_left() {
  move(true, 10, body_pin, body_pos, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  happy_eyes(300, 1);
  delay(500);
  move(true, 30, body_pin, 130, body_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_turn_right() {
  move(true, 10, body_pin, body_pos, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  happy_eyes(300, 1);
  delay(500);
  move(true, 30, body_pin, 50, body_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_point_left() {
  move(false, 50, head_pin, head_pos, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(false, 50, head_pin, 10, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 10, head_pin, 170, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  delay(500);

  move(true, 10, head_pin, 130, 90, body_pin, body_pos, 130, left_arm_pin, left_arm_pos, 70, 0, 0, 0);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);

  for (int i = 0; i < 7; i++) {
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 100);
    delay(100);
    analogWrite(right_red_eye_pin, 240);
    analogWrite(left_red_eye_pin, 240);
    delay(100);
  }

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  move(true, 30, head_pin, 90, head_pos, body_pin, 130, body_pos, left_arm_pin, 70, left_arm_pos, 0, 0, 0);
}

void move_point_right() {
  move(false, 50, head_pin, head_pos, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(false, 50, head_pin, 170, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 10, head_pin, 10, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  delay(500);

  move(true, 10, head_pin, 50, 90, body_pin, body_pos, 50, right_arm_pin, right_arm_pos, 90, 0, 0, 0);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);

  for (int i = 0; i < 7; i++) {
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 100);
    delay(100);
    analogWrite(right_red_eye_pin, 240);
    analogWrite(left_red_eye_pin, 240);
    delay(100);
  }

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  move(true, 30, head_pin, 90, head_pos, body_pin, 50, body_pos, right_arm_pin, 90, right_arm_pos, 0, 0, 0);
}

void move_workout_arms() {
  move(true, 30, right_arm_pin, right_arm_pos, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, left_arm_pin, left_arm_pos, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(true, 20, right_arm_pin, 160, 0, left_arm_pin, 160, 0, 0, 0, 0, 0, 0, 0);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(true, 20, right_arm_pin, 0, 160, left_arm_pin, 0, 160, 0, 0, 0, 0, 0, 0);
  }

  analogWrite(right_red_eye_pin, 240);  
  analogWrite(left_red_eye_pin, 240);         
  move(true, 20, right_arm_pin, 160, 0, left_arm_pin, 160, 0, 0, 0, 0, 0, 0, 0);
  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  delay(500);
 
  move(true, 30, right_arm_pin, 0, right_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 30, left_arm_pin, 0, left_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move_workout_neck() {
  move(true, 40, right_arm_pin, right_arm_pos, 160, left_arm_pin, left_arm_pos, 70, 0, 0, 0, 0, 0, 0);
  delay(500);
  move(true, 50, head_pin, head_pos, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(true, 30, head_pin, 30, 150, right_arm_pin, 160, 90, left_arm_pin, 70, 0, 0, 0, 0);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(true, 30, head_pin, 150, 30, right_arm_pin, 90, 160, left_arm_pin, 0, 70, 0, 0, 0);
  }

  delay(500);
  move(true, 50, head_pin, 30, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  delay(500);
  move(false, 40, right_arm_pin, 160, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  move(true, 40, right_arm_pin, 90, right_arm_pos, left_arm_pin, 70, left_arm_pos, 0, 0, 0, 0, 0, 0);
}

void move_workout_waist() {
  move(true, 40, right_arm_pin, right_arm_pos, 90, left_arm_pin, left_arm_pos, 70, 0, 0, 0, 0, 0, 0);
  delay(500);
  move(true, 50, head_pin, head_pos, 30, body_pin, body_pos, 150, 0, 0, 0, 0, 0, 0);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(true, 30, head_pin, 30, 150, body_pin, 150, 30, 0, 0, 0, 0, 0, 0);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(true, 30, head_pin, 150, 30, body_pin, 30, 150, 0, 0, 0, 0, 0, 0);
  }

  delay(500);
  move(true, 50, head_pin, 30, head_pos, body_pin, 150, head_pos, 0, 0, 0, 0, 0, 0);
  delay(500);
  move(true, 40, right_arm_pin, 90, right_arm_pos, left_arm_pin, 70, left_arm_pos, 0, 0, 0, 0, 0, 0);
}

void move_dance() {
  int head1 = 90;
  int head2;
  int body1 = 90;
  int body2;
  int left_arm1 = 90;
  int left_arm2;
  int right_arm1 = 70;
  int right_arm2;

  move(true, 30, head_pin, head_pos, 90, body_pin, body_pos, 90, left_arm_pin, left_arm_pos, 70, right_arm_pin, right_arm_pos, 90);
  delay(500);

  for (int i = 0; i < 10; i++) {
    body2 = step_aux * random(int(50 / step_aux + 1), int(80 / step_aux + 1));
    head2 = 90 + 2 * (90 - body2);
    right_arm2 = step_aux * random(int(30 / step_aux + 1), int(80 / step_aux + 1));
    left_arm2 = right_arm2 - 10;

    analogWrite(right_blue_eye_pin, 255);  
    analogWrite(left_blue_eye_pin, 255);         

    move(false, 30, head_pin, head1, head2, body_pin, body1, body2, left_arm_pin, left_arm1, left_arm2, right_arm_pin, right_arm1, right_arm2);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
 
    body2 = step_aux * random(int(100 / step_aux + 1), int(130 / step_aux + 1));
    head2 = 90 + 2 * (90 - body2);
    right_arm2 = step_aux * random(int(100 / step_aux + 1), int(150 / step_aux + 1));
    left_arm2 = right_arm2 - 10;

    analogWrite(right_blue_eye_pin, 240);  
    analogWrite(left_blue_eye_pin, 240);         

    move(false, 30, head_pin, head1, head2, body_pin, body1, body2, left_arm_pin, left_arm1, left_arm2, right_arm_pin, right_arm1, right_arm2);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
 }

  analogWrite(right_blue_eye_pin, 240);  
  analogWrite(left_blue_eye_pin, 240);         

  move(true, 30, head_pin, head1, 90, body_pin, body1, 90, left_arm_pin, left_arm1, 70, right_arm_pin, right_arm1, 90);
  delay(500);
  move(true, 30, head_pin, 90, head_pos, body_pin, 90, body_pos, left_arm_pin, 70, left_arm_pos, right_arm_pin, 90, right_arm_pos);
}

void move_random_1() {
  int head1 = 90;
  int head2;
  int body1 = 90;
  int body2;
  int left_arm1 = 160;
  int left_arm2;
  int right_arm1 = 0;
  int right_arm2;
  int time_to_move;
  int time_to_stay;

  move(true, 30, head_pin, head_pos, head1, body_pin, body_pos, body1, left_arm_pin, left_arm_pos, left_arm1, right_arm_pin, right_arm_pos, right_arm1);
  delay(100);

  for (int i = 0; i < 15; i++) {
    head2 = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
    body2 = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
    left_arm2 = step_aux * random(0, int(160 / step_aux + 1));
    right_arm2 = step_aux * random(0, int(170 / step_aux + 1));

    time_to_move=random(10, 101);
    time_to_stay=random(10, 501);

    move(true, time_to_move, head_pin, head1, head2, body_pin, body1, body2, right_arm_pin, right_arm1, right_arm2, left_arm_pin, left_arm1, left_arm2);
    delay(time_to_stay);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
  }

  move(true, 30, head_pin, head2, 90, body_pin, body2, 90, left_arm_pin, left_arm2, 160, right_arm_pin, right_arm2, 0);
  delay(200);
  move(true, 30, head_pin, 90, head_pos, body_pin, 90, body_pos, left_arm_pin, 160, left_arm_pos, right_arm_pin, 0, right_arm_pos);
}

void move_random_2() {
  int head1 = 90;
  int head2;
  int body1 = 90;
  int body2;
  int left_arm1 = 160;
  int left_arm2;
  int right_arm1 = 0;
  int right_arm2;
  int time_to_move_1;
  int time_to_move_2;
  int time_to_stay;

  move(true, 30, head_pin, head_pos, head1, body_pin, body_pos, body1, left_arm_pin, left_arm_pos, left_arm1, right_arm_pin, right_arm_pos, right_arm1);
  delay(100);

  for (int i = 0; i < 15; i++) {
    head2 = step_aux * random(int(20 / step_aux + 1), int(160 / step_aux + 1));
    if (random(1,3) == 1) {
      body2 = step_aux * random(int(20 / step_aux + 1), int(160 / step_aux + 1));
    } else {
      body2 = body1;
    }
    if (random(1,4) == 1) {
      left_arm2 = step_aux * random(0, int(160 / step_aux + 1));
    } else {
      left_arm2 = left_arm1;
    }
    if (random(1,4) == 1) {
      right_arm2 = step_aux * random(0, int(160 / step_aux + 1));
    } else {
      right_arm2 = right_arm1;
    }

    time_to_move_1 = random(10, 101);
    time_to_move_2 = random(10, 101);
    time_to_stay = random(10, 501);

    move(true, time_to_move_1, head_pin, head1, head2, body_pin, body1, body2, right_arm_pin, right_arm1, right_arm2, left_arm_pin, left_arm1, left_arm2);
    delay(time_to_stay);
    move(true, time_to_move_2, head_pin, head2, head1, body_pin, body2, body1, right_arm_pin, right_arm2, right_arm1, left_arm_pin, left_arm2, left_arm1);
  }

  delay(200);
  move(true, 30, head_pin, head1, head_pos, body_pin, body1, body_pos, left_arm_pin, left_arm1, left_arm_pos, right_arm_pin, right_arm1, right_arm_pos);
}

void move_head(int direc) {
  int head_pos_aux = head_pos + direc * step_aux_2;
  if (head_pos_aux > 19 && head_pos_aux < 161) {
//    motor1.write(head_pos_aux);
    move(true, 10, head_pin, head_pos, head_pos_aux, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    head_pos = head_pos_aux;
  }
}

void move_body(int direc) {
  int body_pos_aux = body_pos + direc * step_aux_2;
  if (body_pos_aux > 19 && body_pos_aux < 161) {
//    motor2.write(body_pos_aux);
    move(true, 10, body_pin, body_pos, body_pos_aux, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    body_pos = body_pos_aux;
  }
}

void move_left_arm(int direc) {
  int left_arm_pos_aux = left_arm_pos + direc * step_aux_2;
  if (left_arm_pos_aux > 0 && left_arm_pos_aux < 161) {
//    motor3.write(left_arm_pos_aux);
    move(true, 10, left_arm_pin, left_arm_pos, left_arm_pos_aux, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    left_arm_pos = left_arm_pos_aux;
  }
}

void move_right_arm(int direc) {
  int right_arm_pos_aux = right_arm_pos + direc * step_aux_2;
  if (right_arm_pos_aux > 0 && right_arm_pos_aux < 151) {
//    motor4.write(right_arm_pos_aux);
    move(true, 10, right_arm_pin, right_arm_pos, right_arm_pos_aux, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    right_arm_pos = right_arm_pos_aux;
  }
}

//============================================================================//
// MAIN                                                                       //
//============================================================================//

void loop() {

  check_blink();

// IR
// delay(200);
//  if (irrecv.decode(&results)) {
//    if (results.value != 0) {
//      option = results.value;
//      if (option >= 3136) {
//        option = option - 3136;
//      }
//      if (option >= 1088) {
//        option = option - 1088;
//      }

  option = 999;

  if (Serial1.available() > 1) {
    if (Serial1.read() == 147) {
      option = Serial1.read();
      #ifdef DEBUG
        Serial.println(option);
      #endif
    }
  }

  switch (option) {
    case 1:
      reset(0);
      break;
    case 2:
      happy_eyes(100, 7);
      break;
    case 3:
      angry_eyes(100, 7);
      break;
    case 4:
      scary_eyes(100, 7);
      break;
    case 5:
      move_say_hi();
      break;
    case 6:
      move_say_bye();
      break;
    case 7:
      move_say_yes();
      break;
    case 8:
      move_say_no();
      break;
    case 9:
      move_look_left();
      break;
    case 10:
      move_look_right();
      break;
    case 11:
      move_turn_left();
      break;
    case 12:
      move_turn_right();
      break;
    case 13:
      move_point_left();
      break;
    case 14:
      move_point_right();
      break;
    case 15:
      move_workout_arms();
      break;
    case 16:
      move_workout_neck();
      break;
    case 17:
      move_workout_waist();
      break;
    case 18:
      move_dance();
      break;
    case 19:
      move_random_1();
      break;
    case 20:
      move_random_2();
      break;
    case 51:
      move_head(-1);
      break;
    case 52:
      move_head(1);
      break;
    case 53:
      move_body(-1);
      break;
    case 54:
      move_body(1);
      break;
    case 55:
      move_left_arm(-1);
      break;
    case 56:
      move_left_arm(1);
      break;
    case 57:
      move_right_arm(1);
      break;
    case 58:
      move_right_arm(-1);
      break;
  }
}

// IR
//    irrecv.resume();
//  }
//}
