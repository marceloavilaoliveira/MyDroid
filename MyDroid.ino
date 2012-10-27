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
int left_blue_eye_pin = 11;
int left_red_eye_pin = 12;
// int prox_sensor_pin = A2;

// POSITIONS
int head_pos;
int body_pos;
int left_arm_pos;
int right_arm_pos;
int head_res_pos = 90;
int body_res_pos = 90;
int left_arm_res_pos = 160;
int right_arm_res_pos = 0;
// int prox_sensor_pos = 0;

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

  motor1.attach(head_pin);
  motor2.attach(body_pin);
  motor3.attach(left_arm_pin);
  motor4.attach(right_arm_pin);

  setup_bluetooth();

  reset(0);
}

void setup_bluetooth()
{
  Serial1.begin(38400);                    // Set baud rate
  Serial1.print("\r\n+STWMOD=0\r\n");      // Set to work in slave mode
  Serial1.print("\r\n+STNA=Avila147\r\n"); // Set name
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

//void check_prox() {
//  int sum = 0;  
//  for (int i = 0; i < 20; i++)
//  {
//    prox_sensor_pos = analogRead(prox_sensor_pin);
//    sum = sum + prox_sensor_pos;
//  }
//  prox_sensor_pos = sum / 20; 
//
//  if (prox_sensor_pos > 400) {
//    move_scary();
//  } else if (prox_sensor_pos > 200) {
//    move_angry();
//  } else if (prox_sensor_pos > 100) {
//    move_say_hi();
//  }
//}

void move(int time, int pos1, int new_pos1, int pos2, int new_pos2, int pos3, int new_pos3, int pos4, int new_pos4) {
  boolean continue1 = true;
  boolean continue2 = true;
  boolean continue3 = true;
  boolean continue4 = true;
  boolean moved = false;

  if (new_pos1 != 999) {
    motor1.write(pos1);
  } else {
    continue1 = false;
  }
  if (new_pos2 != 999) {
    motor2.write(pos2);
  } else {
    continue2 = false;
  }
  if (new_pos3 != 999) {
    motor3.write(pos3);
  } else {
    continue3 = false;
  }
  if (new_pos4 != 999) {
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
}

void reset(int mode) {
  // MODE:
  // 0 = SET *_POS=RESET AND GOES TO RESET POSITION
  // 1 = GOES TO RESET POSITION
  // 2 = GOES TO LAST POSITION
  if (mode == 0) {
    head_pos=head_res_pos;
    body_pos=body_res_pos;
    left_arm_pos=left_arm_res_pos;
    right_arm_pos=right_arm_res_pos;
  }

  if (mode == 1) {
    motor1.write(head_res_pos);
    motor2.write(body_res_pos);
    motor3.write(left_arm_res_pos);
    motor4.write(right_arm_res_pos);
  } else {
    motor1.write(head_pos);
    motor2.write(body_pos);
    motor3.write(left_arm_pos);
    motor4.write(right_arm_pos);
  }

  if (mode == 0) {
    happy_eyes(100, 3);
  } else {
    delay(300);
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
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
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
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
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
    analogWrite(left_blue_eye_pin, 255);
    analogWrite(right_red_eye_pin, 255);
    analogWrite(left_red_eye_pin, 100);
    delay(time);
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 100);
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  analogWrite(right_red_eye_pin, 255);
  analogWrite(left_red_eye_pin, 255);
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void move_check() {
  reset(1);

  move(30, head_res_pos, 20, 999, 999, 999, 999, 999, 999);
  move(30, 20, 160, 999, 999, 999, 999, 999, 999);
  move(30, 160, head_res_pos, 999, 999, 999, 999, 999, 999);

  move(30, 999, 999, body_res_pos, 20, 999, 999, 999, 999);
  move(30, 999, 999, 20, 160, 999, 999, 999, 999);
  move(30, 999, 999, 160, body_res_pos, 999, 999, 999, 999);

  move(30, 999, 999, 999, 999, left_arm_res_pos, 0, 999, 999);
  move(30, 999, 999, 999, 999, 0, left_arm_res_pos, 999, 999);

  move(30, 999, 999, 999, 999, 999, 999, right_arm_res_pos, 150);
  move(30, 999, 999, 999, 999, 999, 999, 150, right_arm_res_pos);

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

  reset(2);
}

//============================================================================//
// FUNCTIONS (ADVANCED ACTIONS)                                               //
//============================================================================//

void move_say_hi() {
  reset(1);

  move(0, 999, 999, 999, 999, 999, 999, right_arm_res_pos, 150);
  happy_eyes(800, 1);
  move(40, 999, 999, 999, 999, 999, 999, 150, right_arm_res_pos);

  reset(2);
}

void move_say_bye() {
  reset(1);

  move(10, head_res_pos, 60, 999, 999, 999, 999, right_arm_res_pos, 150);
  delay(100);
  analogWrite(left_blue_eye_pin, 255);
  move(10, 999, 999, 999, 999, 999, 999, 150, 110);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, 110, 150);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, 150, 110);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, 110, 150);
  analogWrite(left_blue_eye_pin, 240);
  delay(800);
  move(40, 60, head_res_pos, 999, 999, 999, 999, 150, right_arm_res_pos);

  reset(2);
}

void move_say_yes() {
  reset(1);

  move(10, 999, 999, 999, 999, left_arm_res_pos, 70, right_arm_res_pos, 90);
  happy_eyes(100, 7);
  move(40, 999, 999, 999, 999, 70, left_arm_res_pos, 90, right_arm_res_pos);

  reset(2);
}

void move_say_no() {
  reset(1);

  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  analogWrite(right_red_eye_pin, 240);  
  analogWrite(left_red_eye_pin, 240);         

  move(10, head_res_pos, 30, 999, 999, 999, 999, right_arm_res_pos, 60);
  move(10, 30, 140, 999, 999, 999, 999, 60, 150);
  move(20, 140, 40, 999, 999, 999, 999, 999, 999);
  move(20, 40, 130, 999, 999, 999, 999, 999, 999);
  move(20, 130, 50, 999, 999, 999, 999, 150, 70);
  move(20, 50, head_res_pos, 999, 999, 999, 999, 70, right_arm_res_pos);
  delay(200);

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  reset(2);
}

void move_happy() {
  reset(1);

  move(0, head_res_pos, 120, body_res_pos, 60, left_arm_res_pos, 50, right_arm_res_pos, 50);

  for (int i = 0; i < 5; i++) {
    analogWrite(right_blue_eye_pin, 100);  
    analogWrite(left_blue_eye_pin, 100);         
    move(20, 120, 60, 60, 120, 50, 110, 50, 110);
    analogWrite(right_blue_eye_pin, 255);  
    analogWrite(left_blue_eye_pin, 255);         
    move(20, 60, 120, 120, 60, 110, 50, 110, 50);
  }
  move(0, 120, head_res_pos, 60, body_res_pos, 50, left_arm_res_pos, 50, right_arm_res_pos);

  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  reset(2);
}

void move_angry() {
  reset(1);

  move(0, head_res_pos, 110, body_res_pos, 70, left_arm_res_pos, 20, right_arm_res_pos, 150);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);

  for (int i = 0; i < 5; i++) {
    analogWrite(right_red_eye_pin, 100);  
    analogWrite(left_red_eye_pin, 100);         
    move(20, 110, 70, 70, 110, 20, 0, 150, 130);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(20, 70, 110, 110, 70, 0, 20, 130, 150);
  }
  move(0, 110, head_res_pos, 70, body_res_pos, 20, left_arm_res_pos, 150, right_arm_res_pos);

  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  reset(2);
}

void move_scary() {
  reset(1);

  move(0, head_res_pos, 70, body_res_pos, 40, left_arm_res_pos, 0, right_arm_res_pos, 150);
  for (int i = 0; i < 4; i++) {
    analogWrite(right_blue_eye_pin, 100);
    analogWrite(left_blue_eye_pin, 255);
    analogWrite(right_red_eye_pin, 255);
    analogWrite(left_red_eye_pin, 100);
    move(20, 70, 110, 40, 140, 0, 10, 150, 140);
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 100);
    analogWrite(right_red_eye_pin, 100);
    analogWrite(left_red_eye_pin, 255);
    move(20, 110, 70, 140, 40, 10, 0, 140, 150);
  }
  move(0, 70, head_res_pos, 40, body_res_pos, 0, left_arm_res_pos, 150, right_arm_res_pos);

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);

  reset(2);
}

void move_look_left() {
  reset(1);

  move(10, head_res_pos, 140, 999, 999, 999, 999, 999, 999);
  happy_eyes(300, 1);
  delay(500);
  move(40, 140, 90, body_res_pos, 140, 999, 999, 999, 999);
  delay(800);
  move(40, 90, head_res_pos, 140, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_look_right() {
  reset(1);

  move(10, head_res_pos, 40, 999, 999, 999, 999, 999, 999);
  happy_eyes(300, 1);
  delay(500);
  move(40, 40, 90, body_res_pos, 40, 999, 999, 999, 999);
  delay(800);
  move(40, 90, head_res_pos, 40, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_turn_left() {
  reset(1);

  move(10, 999, 999, body_res_pos, 140, 999, 999, 999, 999);
  happy_eyes(300, 1);
  delay(500);
  move(40, head_res_pos, 140, 140, body_res_pos, 999, 999, 999, 999);
  delay(800);
  move(40, 140, head_res_pos, 90, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_turn_right() {
  reset(1);

  move(10, 999, 999, body_res_pos, 40, 999, 999, 999, 999);
  happy_eyes(300, 1);
  delay(500);
  move(40, head_res_pos, 40, 40, body_res_pos, 999, 999, 999, 999);
  delay(800);
  move(40, 40, head_res_pos, 90, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_point_left() {
  reset(1);

  move(50, head_res_pos, 10, 999, 999, 999, 999, 999, 999);
  move(50, 10, 160, 999, 999, 999, 999, 999, 999);
  move(10, 160, 130, 999, 999, 999, 999, 999, 999);
  delay(500);
  move(10, 130, 90, body_res_pos, 130, left_arm_res_pos, 70, 999, 999);
  happy_eyes(100, 7);
  move(40, 90, head_res_pos, 130, body_res_pos, 70, left_arm_res_pos, 999, 999);

  reset(2);
}

void move_point_right() {
  reset(1);

  move(50, head_res_pos, 160, 999, 999, 999, 999, 999, 999);
  move(50, 160, 10, 999, 999, 999, 999, 999, 999);
  move(10, 10, 50, 999, 999, 999, 999, 999, 999);
  delay(500);
  move(10, 50, 90,  body_res_pos, 50, 999, 999, right_arm_res_pos, 90);
  happy_eyes(100, 7);
  move(40, 90, head_res_pos, 50, body_res_pos, 999, 999, 90, right_arm_res_pos);

  reset(2);
}

void move_workout_arms() {
  reset(1);

  move(40, 999, 999, 999, 999, 999, 999, right_arm_res_pos, 160);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(20, 999, 999, 999, 999, 160, 0, 160, 0);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(20, 999, 999, 999, 999, 0, 160, 0, 160);
  }

  analogWrite(right_red_eye_pin, 240);  
  analogWrite(left_red_eye_pin, 240);         
  move(20, 999, 999, 999, 999, 160, 0, 160, 0);
  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  delay(500);
  move(40, 999, 999, 999, 999, 0, left_arm_res_pos, 999, 999);

  reset(2);
}

void move_workout_neck() {
  reset(1);

  move(40, 999, 999, 999, 999, left_arm_res_pos, 70, right_arm_res_pos, 150);
  delay(500);
  move(50,  head_res_pos, 30, 999, 999, 999, 999, 999, 999);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(30, 30, 150, 999, 999, 70, 0, 150, 90);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(30, 150, 30, 999, 999, 0, 70, 90, 150);
  }

  delay(500);
  move(50, 30, head_res_pos, 999, 999, 999, 999, 999, 999);
  delay(500);
  move(40, 999, 999, 999, 999, 999, 999, 150, 90);
  move(40, 999, 999, 999, 999, 70, left_arm_res_pos, 90, right_arm_res_pos);

  reset(2);
}

void move_workout_waist() {
  reset(1);

  move(40, 999, 999, 999, 999, left_arm_res_pos, 70, right_arm_res_pos, 90);
  delay(500);
  move(50, head_res_pos, 30, body_res_pos, 150, 999, 999, 999, 999);
  delay(500);

  for (int i = 0; i < 3; i++) {
    analogWrite(right_red_eye_pin, 240);  
    analogWrite(left_red_eye_pin, 240);         
    move(30, 30, 150, 150, 30, 999, 999, 999, 999);
    analogWrite(right_red_eye_pin, 255);  
    analogWrite(left_red_eye_pin, 255);         
    move(30, 150, 30, 30, 150, 999, 999, 999, 999);
  }

  delay(500);
  move(50, 30, head_res_pos, 150, head_res_pos, 999, 999, 999, 999);
  delay(500);
  move(40, 999, 999, 999, 999, 70, left_arm_res_pos, 90, right_arm_res_pos);

  reset(2);
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

  reset(1);

  move(40, head_res_pos, 90, body_res_pos, 90, left_arm_res_pos, 70, right_arm_res_pos, 90);
  delay(500);

  for (int i = 0; i < 10; i++) {
    body2 = step_aux * random(int(50 / step_aux + 1), int(80 / step_aux + 1));
    head2 = 90 + 2 * (90 - body2);
    right_arm2 = step_aux * random(int(30 / step_aux + 1), int(80 / step_aux + 1));
    left_arm2 = right_arm2 - 10;
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 255);
    move(30, head1, head2, body1, body2, left_arm1, left_arm2, right_arm1, right_arm2);

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
    move(30, head1, head2, body1, body2, left_arm1, left_arm2, right_arm1, right_arm2);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
 }

  analogWrite(right_blue_eye_pin, 240);  
  analogWrite(left_blue_eye_pin, 240);         

  move(40, head1, 90, body1, 90, left_arm1, 70, right_arm1, 90);
  delay(500);
  move(40, 90, head_res_pos, 90, body_res_pos, 70, left_arm_res_pos, 90, right_arm_res_pos);

  reset(2);
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

  reset(1);

  move(30, head_res_pos, head1, body_res_pos, body1, left_arm_res_pos, left_arm1, right_arm_res_pos, right_arm1);
  delay(100);

  for (int i = 0; i < 15; i++) {
    head2 = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
    body2 = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
    left_arm2 = step_aux * random(0, int(160 / step_aux + 1));
    right_arm2 = step_aux * random(0, int(150 / step_aux + 1));
    time_to_move=random(10, 101);
    time_to_stay=random(10, 501);

    move(time_to_move, head1, head2, body1, body2, right_arm1, right_arm2, left_arm1, left_arm2);
    delay(time_to_stay);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
  }

  move(30, head2, head_res_pos, body2, body_res_pos, left_arm2, left_arm_res_pos, right_arm2, right_arm_res_pos);

  reset(2);
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

  reset(1);

  move(30, head_pos, head1, body_pos, body1, left_arm_pos, left_arm1, right_arm_pos, right_arm1);
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

    move(time_to_move_1, head1, head2, body1, body2, left_arm1, left_arm2, right_arm1, right_arm2);
    delay(time_to_stay);
    move(time_to_move_2, head2, head1, body2, body1, left_arm2, left_arm1, right_arm2, right_arm1);
  }

  reset(2);
}

void move_head(int direc) {
  int head_pos_aux = head_pos + direc * step_aux_2;
  if (head_pos_aux > 19 && head_pos_aux < 161) {
    move(10, head_pos, head_pos_aux, 999, 999, 999, 999, 999, 999);
    head_pos = head_pos_aux;
  }
}

void move_body(int direc) {
  int body_pos_aux = body_pos + direc * step_aux_2;
  if (body_pos_aux > 19 && body_pos_aux < 161) {
    move(10, 999, 999, body_pos, body_pos_aux, 999, 999, 999, 999);
    body_pos = body_pos_aux;
  }
}

void move_left_arm(int direc) {
  int left_arm_pos_aux = left_arm_pos + direc * step_aux_2;
  if (left_arm_pos_aux > 0 && left_arm_pos_aux < 161) {
    move(10, 999, 999, 999, 999, left_arm_pos, left_arm_pos_aux, 999, 999);
    left_arm_pos = left_arm_pos_aux;
  }
}

void move_right_arm(int direc) {
  int right_arm_pos_aux = right_arm_pos + direc * step_aux_2;
  if (right_arm_pos_aux > 0 && right_arm_pos_aux < 151) {
    move(10, 999, 999, 999, 999, 999, 999, right_arm_pos, right_arm_pos_aux);
    right_arm_pos = right_arm_pos_aux;
  }
}

//============================================================================//
// MAIN                                                                       //
//============================================================================//

void loop() {

  check_blink();
//  check_prox();

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
      // RESET BLINK TIME
      previous_time = millis();
    }
  }

  switch (option) {
    case 1:
      reset(0);
      break;
    case 2:
      move_happy();
      break;
    case 3:
      move_angry();
      break;
    case 4:
      move_scary();
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
    case 21:
      move_check();
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
