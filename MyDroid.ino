//============================================================================//
//                        Marcelo Avila de Oliveira                           //
//                                                                            //
//          MyDroid.ino - Version 2.3.0 - Android Figure Automation           //
//============================================================================//

//============================================================================//
// DEFINITIONS                                                                //
//============================================================================//

// TURN ON DEBUG MODE
//#define DEBUG

//============================================================================//
// LIBRARIES                                                                  //
//============================================================================//

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
int count_pin = 5;
int right_blue_eye_pin = 9;
int right_red_eye_pin = 10;
int left_blue_eye_pin = 11;
int left_red_eye_pin = 12;
int head_pin = 24;
int body_pin = 26;
int left_arm_pin = 28;
int right_arm_pin = 30;
int prox_sensor_pin = A2;

// POSITIONS
// HEAD
int head_min = 20;
int head_avg = 90;
int head_max = 160;
int head_res_pos = head_avg;
int head_pos;
// BODY
int body_min = 20;
int body_avg = 90;
int body_max = 160;
int body_res_pos = body_avg;
int body_pos;
// LEFT ARM
int left_arm_min = 160;
int left_arm_avg = 70;
int left_arm_max = 0;
int left_arm_res_pos = left_arm_min;
int left_arm_pos;
// RIGHT ARM
int right_arm_min = 0;
int right_arm_avg = 90;
int right_arm_max = 150;
int right_arm_res_pos = right_arm_min;
int right_arm_pos;

// TIME
unsigned long current_time = millis();
unsigned long sleep_interval = 30000;
unsigned long time_to_sleep;
unsigned long time_to_blink;

// STATUS
boolean sleep = true;
boolean autom = false;

// MISCELANEOUS
int step_move_prog = 5;
int step_move_free = 10;
int count_state = 0;
int count_state_prev = 0;
float prox_sensor;

//============================================================================//
// FUNCTIONS (SETUP)                                                          //
//============================================================================//

void setup() {
  // INITIALIZE PINS
  pinMode(count_pin, INPUT);
  pinMode(right_blue_eye_pin, OUTPUT);
  pinMode(right_red_eye_pin, OUTPUT);
  pinMode(left_blue_eye_pin, OUTPUT);
  pinMode(left_red_eye_pin, OUTPUT);
  motor1.attach(head_pin);
  motor2.attach(body_pin);
  motor3.attach(left_arm_pin);
  motor4.attach(right_arm_pin);
  pinMode(prox_sensor_pin, INPUT);

  // INITIATE SERIAL COMMUNICATION
  Serial.begin(9600);

  // INITIATE BLUETOOTH COMMUNICATION
  setup_bluetooth();

  // INITIATE RANDOM NUMBER GENERATOR
  randomSeed(analogRead(0));

  // RESET ROBOT
  reset(0);

  set_eyes(254, 254, 255, 255);
}

void setup_bluetooth() {
  Serial1.begin(38400);                    // Set baud rate
  Serial1.print("\r\n+STWMOD=0\r\n");      // Set to work in slave mode
  Serial1.print("\r\n+STNA=Avila147\r\n"); // Set name
  Serial1.print("\r\n+STOAUT=1\r\n");      // Permit Paired device to connect me
  Serial1.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
  delay(2000);                             // This delay is required.
  Serial1.print("\r\n+INQ=1\r\n");         // Make the slave inquirable 
  delay(2000);                             // This delay is required.
  while (Serial1.available()) {            // Clear data
    delay(50);
    Serial1.read();
  }
}

//============================================================================//
// FUNCTIONS (BASIC)                                                          //
//============================================================================//

void read_prox_sensor() {
  prox_sensor = 0;
  int num_check = 20;
  for (int i = 0; i < num_check; i++)
  {
    prox_sensor = prox_sensor + analogRead(prox_sensor_pin);
  }
  prox_sensor = prox_sensor / num_check;
}

void set_eyes(int left_red, int right_red, int left_blue, int right_blue) {
  analogWrite(left_red_eye_pin, left_red);
  analogWrite(right_red_eye_pin, right_red);
  analogWrite(left_blue_eye_pin, left_blue);
  analogWrite(right_blue_eye_pin, right_blue);
}

void blink_eyes() {
  set_eyes(255, 255, 100, 100);
  delay(50);
  set_eyes(255, 255, 255, 255);
  delay(300);
  set_eyes(255, 255, 240, 240);
}

void check_blink() {
  current_time = millis();
  if (current_time > time_to_blink) {
    time_to_blink = current_time + random(3000, 7000);
    blink_eyes();
  }
}

void check_proximity() {
  read_prox_sensor();
  if (prox_sensor > 100) {
    check_sleep(1);
  }
}

void check_count() {
  count_state = digitalRead(count_pin);

  if (count_state != count_state_prev) {
    count_state_prev = count_state;

    if (count_state == HIGH) {
      check_sleep(1);

      // WRITE A COUNT SIGNAL TO BLUETOOTH
      Serial1.print(1);
    }
  }
}

void check_serial() {
  boolean ok = false;
  char read_aux1;
  char read_aux2[20];
  int char_num = 0;
  int option = 999;
  while (Serial.available() && !ok) {
    read_aux1 = Serial.read();
    delay(50);

    if (read_aux1 == '|') {
      option = atoi(read_aux2);
      ok = true;
    } else {
      read_aux2[char_num] = read_aux1;
      char_num++;
    }
  }

  ok = false;
  char_num = 0;
  int parameter = 0;
  char read_aux3[20];
  while (Serial.available() && !ok) {
    read_aux1 = Serial.read();
    delay(50);

    if (read_aux1 == '|') {
      parameter = atoi(read_aux3);
      ok = true;
    } else {
      read_aux3[char_num] = read_aux1;
      char_num++;
    }
  }
  
  if (option != 999) {
    execute_option(option, parameter);
  }
}

void check_bluetooth() {
  int option = 999;
  int parameter = 0;
  boolean ok = false;
  while (Serial1.available() && !ok) {
    if (Serial1.read() == 147) {
      delay(50);
      option = Serial1.read();
      delay(50);
      parameter = Serial1.read();

      ok = true;
      execute_option(option, parameter);
    }
  }
}

void check_sleep(int mode) {
  // MODE:
  // 0 = CHECK IF IT'S TIME TO SLEEP
  // 1 = AN EVENT TO WAKE OR STAY WAKED JUST HAPPEN
  if (mode) {
    if (sleep) {
      // WAKE
      change_sleep();
    } else {
      // RESET TIME TO SLEEP
      time_to_sleep = millis() + sleep_interval;
    }
  } else {
    if (!sleep && millis() > time_to_sleep) {
      // SLEEP
      change_sleep();
    }
  }
}

void change_sleep() {
  if (sleep) {
    // WAKE
    reset(2);
    set_eyes(255, 255, 240, 240);
    time_to_blink = millis() + random(3000, 7000);
    time_to_sleep = millis() + sleep_interval;
  } else {
    // SLEEP
    reset(1);
    set_eyes(254, 254, 255, 255);
  }
  sleep=!sleep;
}

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
          pos1 += step_move_prog;
        } else {
          pos1 -= step_move_prog;
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
          pos2 += step_move_prog;
        } else {
          pos2 -= step_move_prog;
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
          pos3 += step_move_prog;
        } else {
          pos3 -= step_move_prog;
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
          pos4 += step_move_prog;
        } else {
          pos4 -= step_move_prog;
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
  // 0 = SET *_POS AND GO TO RESET POSITION
  // 1 = GO TO RESET POSITION
  // 2 = GO TO LAST POSITION
  // 3 = SET *_POS AND GO TO RANDOM POSITION
  if (mode == 0) {
    head_pos=head_res_pos;
    body_pos=body_res_pos;
    left_arm_pos=left_arm_res_pos;
    right_arm_pos=right_arm_res_pos;
  } else if (mode == 3) {
    head_pos=random(head_min, head_max+1);
    body_pos=random(body_min, body_max+1);
    left_arm_pos=random(left_arm_max, left_arm_min+1);
    right_arm_pos=random(right_arm_min, right_arm_max+1);
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
  set_eyes(255, 255, 255, 255);
  for (int i = 0; i < repeat; i++) {
    set_eyes(255, 255, 100, 100);
    delay(time);
    set_eyes(255, 255, 255, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  set_eyes(255, 255, 240, 240);
}

void angry_eyes(int time, int repeat) {
  set_eyes(255, 255, 255, 255);
  for (int i = 0; i < repeat; i++) {
    set_eyes(100, 100, 255, 255);
    delay(time);
    set_eyes(255, 255, 255, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  set_eyes(255, 255, 240, 240);
}

void scary_eyes(int time, int repeat) {
  set_eyes(255, 255, 255, 255);
  for (int i = 0; i < repeat; i++) {
    set_eyes(100, 255, 255, 100);
    delay(time);
    set_eyes(255, 100, 100, 255);
    if (i != repeat-1) {
      delay(time);
    }
  }
  set_eyes(255, 255, 240, 240);
}

void move_check() {
  reset(1);

  // HEAD
  move(30, head_res_pos, head_min, 999, 999, 999, 999, 999, 999);
  move(30, head_min, head_max, 999, 999, 999, 999, 999, 999);
  move(30, head_max, head_res_pos, 999, 999, 999, 999, 999, 999);
  // BODY
  move(30, 999, 999, body_res_pos, body_min, 999, 999, 999, 999);
  move(30, 999, 999, body_min, body_max, 999, 999, 999, 999);
  move(30, 999, 999, body_max, body_res_pos, 999, 999, 999, 999);
  // LEFT ARM
  move(30, 999, 999, 999, 999, left_arm_res_pos, left_arm_max, 999, 999);
  move(30, 999, 999, 999, 999, left_arm_max, left_arm_res_pos, 999, 999);
  // RIGHT ARM
  move(30, 999, 999, 999, 999, 999, 999, right_arm_res_pos, right_arm_max);
  move(30, 999, 999, 999, 999, 999, 999, right_arm_max, right_arm_res_pos);
  // EYES
  set_eyes(255, 255, 255, 255);
  delay(300);
  set_eyes(240, 240, 255, 255);
  delay(300);
  set_eyes(255, 255, 240, 240);

  reset(2);
}

//============================================================================//
// FUNCTIONS (ADVANCED ACTIONS)                                               //
//============================================================================//

void move_say_hi() {
  reset(1);

  move(0, 999, 999, 999, 999, 999, 999, right_arm_res_pos, right_arm_max);
  happy_eyes(800, 1);
  move(40, 999, 999, 999, 999, 999, 999, right_arm_max, right_arm_res_pos);

  reset(2);
}

void move_say_bye(boolean going_sleep) {
  reset(1);

  move(10, head_res_pos, 60, 999, 999, 999, 999, right_arm_res_pos, right_arm_max);
  delay(100);
  set_eyes(255, 255, 255, 240);
  move(10, 999, 999, 999, 999, 999, 999, right_arm_max, 110);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, 110, right_arm_max);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, right_arm_max, 110);
  delay(100);
  move(10, 999, 999, 999, 999, 999, 999, 110, right_arm_max);
  set_eyes(255, 255, 240, 240);
  delay(800);
  move(40, 60, head_res_pos, 999, 999, 999, 999, right_arm_max, right_arm_res_pos);

  if (! going_sleep) {
    reset(2);
  }
}

void move_say_yes() {
  reset(1);

  move(10, 999, 999, 999, 999, left_arm_res_pos, left_arm_avg, right_arm_res_pos, right_arm_avg);
  happy_eyes(100, 7);
  move(40, 999, 999, 999, 999, left_arm_avg, left_arm_res_pos, right_arm_avg, right_arm_res_pos);

  reset(2);
}

void move_say_no() {
  reset(1);

  set_eyes(240, 240, 255, 255);

  move(10, head_res_pos, 30, 999, 999, 999, 999, right_arm_res_pos, 60);
  move(10, 30, 140, 999, 999, 999, 999, 60, right_arm_max);
  move(20, 140, 40, 999, 999, 999, 999, 999, 999);
  move(20, 40, 130, 999, 999, 999, 999, 999, 999);
  move(20, 130, 50, 999, 999, 999, 999, right_arm_max, 70);
  move(20, 50, head_res_pos, 999, 999, 999, 999, 70, right_arm_res_pos);
  delay(200);

  set_eyes(255, 255, 240, 240);

  reset(2);
}

void move_happy() {
  reset(1);

  move(0, head_res_pos, 120, body_res_pos, 60, left_arm_res_pos, 50, right_arm_res_pos, 50);

  for (int i = 0; i < 5; i++) {
    set_eyes(255, 255, 100, 100);
    move(20, 120, 60, 60, 120, 50, 110, 50, 110);
    set_eyes(255, 255, 255, 255);
    move(20, 60, 120, 120, 60, 110, 50, 110, 50);
  }
  move(0, 120, head_res_pos, 60, body_res_pos, 50, left_arm_res_pos, 50, right_arm_res_pos);

  set_eyes(255, 255, 240, 240);

  reset(2);
}

void move_angry() {
  reset(1);

  move(0, head_res_pos, 110, body_res_pos, 70, left_arm_res_pos, 20, right_arm_res_pos, right_arm_max);
  set_eyes(255, 255, 255, 255);

  for (int i = 0; i < 5; i++) {
    set_eyes(100, 100, 255, 255);
    move(20, 110, 70, 70, 110, 20, left_arm_max, right_arm_max, 130);
    set_eyes(255, 255, 255, 255);
    move(20, 70, 110, 110, 70, left_arm_max, 20, 130, right_arm_max);
  }
  move(0, 110, head_res_pos, 70, body_res_pos, 20, left_arm_res_pos, right_arm_max, right_arm_res_pos);

  set_eyes(255, 255, 240, 240);

  reset(2);
}

void move_scary() {
  reset(1);

  move(0, head_res_pos, 70, body_res_pos, 40, left_arm_res_pos, left_arm_max, right_arm_res_pos, right_arm_max);
  for (int i = 0; i < 4; i++) {
    set_eyes(100, 255, 255, 100);
    move(20, 70, 110, 40, 140, left_arm_max, 20, right_arm_max, 130);
    set_eyes(255, 100, 100, 255);
    move(20, 110, 70, 140, 40, 20, left_arm_max, 130, right_arm_max);
  }
  move(0, 70, head_res_pos, 40, body_res_pos, left_arm_max, left_arm_res_pos, right_arm_max, right_arm_res_pos);

  set_eyes(255, 255, 240, 240);

  reset(2);
}

void move_look_left() {
  reset(1);

  move(10, head_res_pos, 130, 999, 999, 999, 999, 999, 999);
  delay(100);
  happy_eyes(700, 1);
  move(40, 130, head_avg, body_res_pos, 130, 999, 999, 999, 999);
  delay(800);
  move(40, 999, 999, 130, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_look_right() {
  reset(1);

  move(10, head_res_pos, 30, 999, 999, 999, 999, 999, 999);
  delay(100);
  happy_eyes(700, 1);
  move(40, 30, head_avg, body_res_pos, 30, 999, 999, 999, 999);
  delay(800);
  move(40, 999, 999, 30, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_look_around() {
  reset(1);

  move(50, head_res_pos, head_min, 999, 999, 999, 999, 999, 999);
  move(50, head_min, head_avg, 999, 999, 999, 999, 999, 999);
  delay(200);
  move(50, head_avg, head_max, 999, 999, 999, 999, 999, 999);
  move(30, head_max, head_res_pos, 999, 999, 999, 999, 999, 999);

  reset(2);
}

void move_turn_left() {
  reset(1);

  move(10, 999, 999, body_res_pos, 130, 999, 999, 999, 999);
  delay(100);
  happy_eyes(700, 1);
  move(40, head_res_pos, 130, 130, body_res_pos, 999, 999, 999, 999);
  delay(800);
  move(40, 130, head_res_pos, 999, 999, 999, 999, 999, 999);

  reset(2);
}

void move_turn_right() {
  reset(1);

  move(10, 999, 999, body_res_pos, 30, 999, 999, 999, 999);
  delay(100);
  happy_eyes(700, 1);
  move(40, head_res_pos, 30, 30, body_res_pos, 999, 999, 999, 999);
  delay(800);
  move(40, 30, head_res_pos, 999, 999, 999, 999, 999, 999);

  reset(2);
}

void move_turn_around() {
  reset(1);

  move(50, 999, 999, body_res_pos, body_min, 999, 999, 999, 999);
  move(50, 999, 999, body_min, body_avg, 999, 999, 999, 999);
  delay(200);
  move(50, 999, 999, body_avg, body_max, 999, 999, 999, 999);
  move(30, 999, 999, body_max, body_res_pos, 999, 999, 999, 999);

  reset(2);
}

void move_point_left() {
  reset(1);

  move(50, head_res_pos, 20, 999, 999, 999, 999, 999, 999);
  move(50, 20, 160, 999, 999, 999, 999, 999, 999);
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
  move(50, 160, 20, 999, 999, 999, 999, 999, 999);
  move(10, 20, 50, 999, 999, 999, 999, 999, 999);
  delay(500);
  move(10, 50, 90,  body_res_pos, 50, 999, 999, right_arm_res_pos, 90);
  happy_eyes(100, 7);
  move(40, 90, head_res_pos, 50, body_res_pos, 999, 999, 90, right_arm_res_pos);

  reset(2);
}

void move_workout_arms() {
  reset(1);

  move(40, 999, 999, 999, 999, 999, 999, right_arm_res_pos, right_arm_max);
  delay(500);

  for (int i = 0; i < 3; i++) {
    set_eyes(240, 240, 240, 240);
    move(20, 999, 999, 999, 999, left_arm_res_pos, left_arm_max, right_arm_max, 0);
    set_eyes(255, 255, 240, 240);
    move(20, 999, 999, 999, 999, left_arm_max, left_arm_min, 0, right_arm_max);
  }

  set_eyes(240, 240, 240, 240);
  move(20, 999, 999, 999, 999, left_arm_min, left_arm_max, right_arm_max, right_arm_res_pos);
  set_eyes(255, 255, 240, 240);
  delay(500);
  move(40, 999, 999, 999, 999, left_arm_max, left_arm_res_pos, 999, 999);

  reset(2);
}

void move_workout_neck() {
  reset(1);

  move(40, 999, 999, 999, 999, left_arm_res_pos, left_arm_avg, right_arm_res_pos, right_arm_max);
  delay(500);
  move(50,  head_res_pos, 30, 999, 999, 999, 999, 999, 999);
  delay(500);

  for (int i = 0; i < 3; i++) {
    set_eyes(240, 240, 240, 240);
    move(30, 30, 150, 999, 999, left_arm_avg, left_arm_max, right_arm_max, right_arm_avg);
    set_eyes(255, 255, 240, 240);
    move(30, 150, 30, 999, 999, left_arm_max, left_arm_avg, right_arm_avg, right_arm_max);
  }

  delay(500);
  move(50, 30, head_res_pos, 999, 999, 999, 999, 999, 999);
  delay(500);
  move(40, 999, 999, 999, 999, 999, 999, right_arm_max, right_arm_avg);
  move(40, 999, 999, 999, 999, left_arm_avg, left_arm_res_pos, right_arm_avg, right_arm_res_pos);

  reset(2);
}

void move_workout_waist() {
  reset(1);

  move(40, 999, 999, 999, 999, left_arm_res_pos, left_arm_avg, right_arm_res_pos, right_arm_avg);
  delay(500);
  move(50, head_res_pos, 30, body_res_pos, 150, 999, 999, 999, 999);
  delay(500);

  for (int i = 0; i < 3; i++) {
    set_eyes(240, 240, 240, 240);
    move(30, 30, 150, 150, 30, 999, 999, 999, 999);
    set_eyes(255, 255, 240, 240);
    move(30, 150, 30, 30, 150, 999, 999, 999, 999);
  }

  delay(500);
  move(50, 30, head_res_pos, 150, head_res_pos, 999, 999, 999, 999);
  delay(500);
  move(40, 999, 999, 999, 999, left_arm_avg, left_arm_res_pos, right_arm_avg, right_arm_res_pos);

  reset(2);
}

// set_eyes(xxxxx);

void move_dance() {
  int head1 = head_res_pos;
  int head2;
  int body1 = body_res_pos;
  int body2;
  int left_arm1 = left_arm_avg;
  int left_arm2;
  int right_arm1 = right_arm_avg;
  int right_arm2;

  reset(1);

  move(40, 999, 999, 999, 999, left_arm_res_pos, left_arm_avg, right_arm_res_pos, right_arm_avg);
  delay(500);

  for (int i = 0; i < 10; i++) {
    body2 = step_move_prog * random(int(50 / step_move_prog + 1), int(80 / step_move_prog + 1));
    head2 = 90 + 2 * (90 - body2);
    right_arm2 = step_move_prog * random(int(30 / step_move_prog + 1), int(80 / step_move_prog + 1));
    left_arm2 = right_arm2 - 10;
    analogWrite(right_blue_eye_pin, 255);
    analogWrite(left_blue_eye_pin, 255);
    move(30, head1, head2, body1, body2, left_arm1, left_arm2, right_arm1, right_arm2);

    head1 = head2;
    body1 = body2;
    right_arm1 = right_arm2;
    left_arm1 = left_arm2;
 
    body2 = step_move_prog * random(int(100 / step_move_prog + 1), int(130 / step_move_prog + 1));
    head2 = 90 + 2 * (90 - body2);
    right_arm2 = step_move_prog * random(int(100 / step_move_prog + 1), int(160 / step_move_prog + 1));
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
  move(40, 999, 999, 999, 999, 70, left_arm_res_pos, 90, right_arm_res_pos);

  reset(2);
}

void move_random_1() {
  int head1 = head_res_pos;
  int head2;
  int body1 = body_res_pos;
  int body2;
  int left_arm1 = left_arm_res_pos;
  int left_arm2;
  int right_arm1 = right_arm_res_pos;
  int right_arm2;
  int time_to_move;
  int time_to_stay;

  reset(1);

  for (int i = 0; i < 15; i++) {
    head2 = step_move_prog * random(int(head_min / step_move_prog + 1), int(head_max / step_move_prog + 1));
    body2 = step_move_prog * random(int(body_min / step_move_prog + 1), int(body_max / step_move_prog + 1));
    left_arm2 = step_move_prog * random(left_arm_max, int(left_arm_min / step_move_prog + 1));
    right_arm2 = step_move_prog * random(right_arm_min, int(right_arm_max / step_move_prog + 1));
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
  int head1 = head_res_pos;
  int head2;
  int body1 = body_res_pos;
  int body2;
  int left_arm1 = left_arm_res_pos;
  int left_arm2;
  int right_arm1 = right_arm_res_pos;
  int right_arm2;
  int time_to_move_1;
  int time_to_move_2;
  int time_to_stay;

  reset(1);

  for (int i = 0; i < 15; i++) {
    head2 = step_move_prog * random(int(head_min / step_move_prog + 1), int(head_max / step_move_prog + 1));
    if (random(1,3) == 1) {
      body2 = step_move_prog * random(int(body_min / step_move_prog + 1), int(head_max / step_move_prog + 1));
    } else {
      body2 = body1;
    }
    if (random(1,4) == 1) {
      left_arm2 = step_move_prog * random(left_arm_max, int(left_arm_min / step_move_prog + 1));
    } else {
      left_arm2 = left_arm1;
    }
    if (random(1,4) == 1) {
      right_arm2 = step_move_prog * random(right_arm_min, int(right_arm_max / step_move_prog + 1));
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
  int head_pos_aux = head_pos + direc * step_move_free;
  if (head_pos_aux >= head_min && head_pos_aux <= head_max) {
    move(10, head_pos, head_pos_aux, 999, 999, 999, 999, 999, 999);
    head_pos = head_pos_aux;
  }
}

void move_body(int direc) {
  int body_pos_aux = body_pos + direc * step_move_free;
  if (body_pos_aux >= body_min && body_pos_aux <= body_max) {
    move(10, 999, 999, body_pos, body_pos_aux, 999, 999, 999, 999);
    body_pos = body_pos_aux;
  }
}

void move_left_arm(int direc) {
  int left_arm_pos_aux = left_arm_pos + direc * step_move_free;
  if (left_arm_pos_aux >= left_arm_max && left_arm_pos_aux <= left_arm_min) {
    move(10, 999, 999, 999, 999, left_arm_pos, left_arm_pos_aux, 999, 999);
    left_arm_pos = left_arm_pos_aux;
  }
}

void move_right_arm(int direc) {
  int right_arm_pos_aux = right_arm_pos + direc * step_move_free;
  if (right_arm_pos_aux >= right_arm_min && right_arm_pos_aux <= right_arm_max) {
    move(10, 999, 999, 999, 999, 999, 999, right_arm_pos, right_arm_pos_aux);
    right_arm_pos = right_arm_pos_aux;
  }
}

void move_auto() {
  int head2;
  int body2;
  int left_arm2;
  int right_arm2;
  int time_prox;

  read_prox_sensor();
  if (prox_sensor > 400) {
    move_angry();
  } else if (prox_sensor > 100) {
    head2 = step_move_prog * random(int(head_min / step_move_prog + 1), int(head_max / step_move_prog + 1));
    if (random(1,3) == 1) {
      body2 = step_move_prog * random(int(body_min / step_move_prog + 1), int(body_max / step_move_prog + 1));
    } else {
      body2 = body_pos;
    }
    if (random(1,4) == 1) {
      left_arm2 = step_move_prog * random(left_arm_max, int(left_arm_min / step_move_prog + 1));
    } else {
      left_arm2 = left_arm_pos;
    }
    if (random(1,4) == 1) {
      right_arm2 = step_move_prog * random(right_arm_min, int(right_arm_max / step_move_prog + 1));
    } else {
      right_arm2 = right_arm_pos;
    }

    read_prox_sensor();
    time_prox = 60 - prox_sensor / 7;
    if (time_prox < 0) { time_prox = 0; };
    move(time_prox, head_pos, head2, body_pos, body2, left_arm_pos, left_arm2, right_arm_pos, right_arm2);

    read_prox_sensor();
    time_prox = 500 - prox_sensor;
    if (time_prox < 100) { time_prox = 100; };
    delay(time_prox);

    read_prox_sensor();
    time_prox = 60 - prox_sensor / 7;
    if (time_prox < 0) { time_prox = 0; };
    move(time_prox, head2, head_pos, body2, body_pos, left_arm2, left_arm_pos, right_arm2, right_arm_pos);
  } else {
    switch (random(1, 2001)) {
      case 1:
        move_look_around();
        break;
      case 2:
        move_turn_around();
        break;
    }
  }
}

void move_simon(int mode, int time) {
  int head1 = head_avg;
  int body1 = body_avg;
  int left_arm1 = left_arm_avg;
  int right_arm1 = right_arm_avg;

  switch (mode) {
    case 1:
      move(time, head_avg, head_min, 999, 999, 999, 999, 999, 999);
      head1 = head_min;
      break;
    case 2:
      move(time, head_avg, head_max, 999, 999, 999, 999, 999, 999);
      head1 = head_max;
      break;
    case 3:
      move(time, 999, 999, body_avg, body_min, 999, 999, 999, 999);
      body1 = body_min;
      break;
    case 4:
      move(time, 999, 999, body_avg, body_max, 999, 999, 999, 999);
      body1 = body_max;
      break;
    case 5:
      move(time, 999, 999, 999, 999, left_arm_avg, left_arm_max, 999, 999);
      left_arm1 = left_arm_max;
      break;
    case 6:
      move(time, 999, 999, 999, 999, left_arm_avg, left_arm_min, 999, 999);
      left_arm1 = left_arm_min;
      break;
    case 7:
      move(time, 999, 999, 999, 999, 999, 999, right_arm_avg, right_arm_max);
      right_arm1 = right_arm_max;
      break;
    case 8:
      move(time, 999, 999, 999, 999, 999, 999, right_arm_avg, right_arm_min);
      right_arm1 = right_arm_min;
      break;
    case 9:
      motor1.write(head_avg);
      motor2.write(body_avg);
      motor3.write(left_arm_avg);
      motor4.write(right_arm_avg);
      happy_eyes(100, 7);
      break;
  }
  delay(500);
  if (mode != 9) {
    move(time, head1, 90, body1, 90, left_arm1, 70, right_arm1, 90);
  }
}

void move_count(int number) {
  int head_pos = map(number/100, 0, 3, head_avg, head_min);
  int right_arm_pos =  map(number%100/10, 0, 9, right_arm_min, right_arm_max);
  int left_arm_pos = map(number%10, 0, 9, left_arm_min, left_arm_max);
  
  set_eyes(100, 100, 255, 255);

  motor1.write(head_pos);
  motor2.write(body_avg);
  motor3.write(left_arm_pos);
  motor4.write(right_arm_pos);
  delay(100);

  set_eyes(255, 255, 240, 240);
}

void execute_option(int option, int parameter) {
  #ifdef DEBUG
    Serial.print("Option: ");
    Serial.println(option);
    Serial.print("Parameter: ");
    Serial.println(parameter);
  #endif

  check_sleep(1);

  // THIS IS TO AVOID TO BLINK JUST AFTER EXECUTING A OPTION
  time_to_blink = millis() + random(3000, 7000);

  switch (option) {
    case 1:
      autom=!autom;
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
      move_say_bye(0);
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
    case 61:
      move_simon(1, parameter);
      break;
    case 62:
      move_simon(2, parameter);
      break;
    case 63:
      move_simon(3, parameter);
      break;
    case 64:
      move_simon(4, parameter);
      break;
    case 65:
      move_simon(5, parameter);
      break;
    case 66:
      move_simon(6, parameter);
      break;
    case 67:
      move_simon(7, parameter);
      break;
    case 68:
      move_simon(8, parameter);
      break;
    case 69:
      move_simon(9, 0);
      break;
    case 71:
      happy_eyes(300, parameter);
      break;
    case 72:
      angry_eyes(300, parameter);
      break;
    case 75:
      move_count(parameter);
      break;
    case 100:
      reset(parameter);
      break;
  }
}

//============================================================================//
// MAIN                                                                       //
//============================================================================//

void loop() {
  check_sleep(0);

  check_proximity();
  check_count();
  check_serial();
  check_bluetooth();

  if (! sleep) {
    check_blink();

    if (autom) {
      move_auto();
    }
  }
}

