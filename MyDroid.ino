//============================================================================//
//                        Marcelo Ávila de Oliveira                           //
//                                                                            //
//            MyDroid.ino - Android Figure Control using Arduino              //
//============================================================================//

#include <IRremote.h>
#include <Servo.h> 

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

int head_pin = 24;
int body_pin = 26;
int left_arm_pin = 28;
int right_arm_pin = 30;
int right_blue_eye_pin = 6;
int right_red_eye_pin = 7;
int left_blue_eye_pin =11;
int left_red_eye_pin = 12;

int step_aux = 5;
int blink_num = 0;
int time_to_blink = 35;
int auto_move_num;
int time_to_auto_move;
int remote_button;
boolean auto_move = false;

int head_pos;
int body_pos;
int left_arm_pos;
int right_arm_pos;

IRrecv irrecv(22);

decode_results results;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  irrecv.enableIRIn();
  reset(0);
}

void blink_eye() {
  analogWrite(right_blue_eye_pin, 220);
  analogWrite(left_blue_eye_pin, 220);
  delay(50);
  analogWrite(right_blue_eye_pin, 255);
  analogWrite(left_blue_eye_pin, 255);
  delay(200);
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void bye() {
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

void check() {
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

void dance() {
  int pos1;
  int pos2;
  move(true, 10, head_pin, head_pos, 90, body_pin, body_pos, 90, right_arm_pin, right_arm_pos, 0, left_arm_pin, left_arm_pos, 160);
  move(true, 20, right_arm_pin, 0, 120, left_arm_pin, 160, 60, 0, 0, 0, 0, 0, 0);
  pos2 = 90;
  for (int i = 0; i < 7; i++) {
    pos1 = pos2;
    pos2 = pos1 - 5 * (i + 1);
    move(true, 20, right_arm_pin, 120, 60, left_arm_pin, 120, 60, body_pin, pos1, pos2, 0, 0, 0);
    pos1 = pos2;
    pos2 = pos1 - 5 * (i + 1);
    move(true, 20, right_arm_pin, 60, 120, left_arm_pin, 60, 120, body_pin, pos1, pos2, 0, 0, 0);
  }

  shine_blue(300);
  move(true, 20, right_arm_pin, 120, right_arm_pos, left_arm_pin, 120, left_arm_pos, head_pin, 60, head_pos, 0, 0, 0);

}

void dance_2() {
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

  for (int i = 0; i < 20; i++) {
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

void look_left() {
  move(true, 10, head_pin, head_pos, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, head_pin, 130, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void look_right() {
  move(true, 10, head_pin, head_pos, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, head_pin, 50, head_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void move(boolean wait, int time, int pin1, int pos1, int new_pos1, int pin2, int pos2, int new_pos2, int pin3, int pos3, int new_pos3, int pin4, int pos4, int new_pos4) {
  boolean continue1 = true;
  boolean continue2 = true;
  boolean continue3 = true;
  boolean continue4 = true;
  boolean moved = false;

  if (pin1 != 0) {
    motor1.attach(pin1);
  } else {
    continue1 = false;
  }
  if (pin2 != 0) {
    motor2.attach(pin2);
  } else {
    continue2 = false;
  }
  if (pin3 != 0) {
    motor3.attach(pin3);
  } else {
    continue3 = false;
  }
  if (pin4 != 0) {
    motor4.attach(pin4);
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

void move_auto() {
  int old_head_pos = head_pos;
  int old_body_pos = body_pos;
  int old_left_arm_pos = left_arm_pos;
  int old_right_arm_pos = right_arm_pos;
  int time_to_move = random(30, 121);
  head_pos = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
  body_pos = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
  left_arm_pos = step_aux * random(0, int(160 / step_aux + 1));
  right_arm_pos = step_aux * random(0, int(170 / step_aux + 1));

  move(false, time_to_move, head_pin, old_head_pos, head_pos, body_pin, old_body_pos, body_pos, right_arm_pin, old_right_arm_pos, right_arm_pos, left_arm_pin, old_left_arm_pos, left_arm_pos);
}

void move_auto_2() {
  int new_head_pos;
  int new_body_pos;
  int new_left_arm_pos;
  int new_right_arm_pos;
  int time_to_move_1 = random(30, 101);
  int time_to_move_2 = random(30, 101);
  int time_to_stay = random(10, 501);

  new_head_pos = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
  if (random(1,4) == 1) {
    new_body_pos = step_aux * random(int(30 / step_aux + 1), int(150 / step_aux + 1));
  } else {
    new_body_pos = body_pos;
  }
  if (random(1,4) == 1) {
    new_left_arm_pos = step_aux * random(0, int(160 / step_aux + 1));
  } else {
    new_left_arm_pos = left_arm_pos;
  }
  if (random(1,4) == 1) {
    new_right_arm_pos = step_aux * random(0, int(170 / step_aux + 1));
  } else {
    new_right_arm_pos = right_arm_pos;
  }

  move(true, time_to_move_1, head_pin, head_pos, new_head_pos, body_pin, body_pos, new_body_pos, right_arm_pin, right_arm_pos, new_right_arm_pos, left_arm_pin, left_arm_pos, new_left_arm_pos);
  delay(time_to_stay);
  move(true, time_to_move_2, head_pin, new_head_pos, head_pos, body_pin, new_body_pos, body_pos, right_arm_pin, new_right_arm_pos, right_arm_pos, left_arm_pin, new_left_arm_pos, left_arm_pos);
}

void point_right() {
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

void point_left() {
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

void reset(int mode) {
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

  analogWrite(right_red_eye_pin, 255);  
  analogWrite(left_red_eye_pin, 255);         
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void say_no() {
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

void say_yes() {
  move(true, 10, right_arm_pin, right_arm_pos, 90, left_arm_pin, left_arm_pos, 70, 0, 0, 0, 0, 0, 0);

  for (int i = 0; i < 7; i++) {
    shine_blue(100);
    delay(100);
  }

  move(true, 30, right_arm_pin, 90, right_arm_pos, left_arm_pin, 70, left_arm_pos, 0, 0, 0, 0, 0, 0);
}

void shine_blue(int time) {
  analogWrite(right_blue_eye_pin, 100);
  analogWrite(left_blue_eye_pin, 100);
  delay(time);
  analogWrite(right_blue_eye_pin, 240);
  analogWrite(left_blue_eye_pin, 240);
}

void stand_left_arm() {
  move(true, 10, left_arm_pin, left_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, left_arm_pin, 0, left_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void stand_right_arm() {
  move(true, 10, right_arm_pin, right_arm_pos, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, right_arm_pin, 160, right_arm_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void turn_left() {
  move(true, 10, body_pin, body_pos, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, body_pin, 130, body_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void turn_right() {
  move(true, 10, body_pin, body_pos, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  shine_blue(300);
  delay(500);
  move(true, 30, body_pin, 50, body_pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void workout_arms() {
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

void workout_waist() {
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

void workout_neck() {
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

void loop() {
  delay(200);

  if (blink_num == time_to_blink) {
    blink_eye();
    time_to_blink = random(20, 51);
    blink_num = 0;
  } else {
    blink_num++;
  }

  if (auto_move) {
    if (auto_move_num == time_to_auto_move) {
      move_auto_2();
      time_to_auto_move = random(0, 11);
      auto_move_num = 0;
    } else {
      auto_move_num++;
    }
  }

  if (irrecv.decode(&results)) {
    if (results.value != 0) {
      remote_button = results.value;
      if (remote_button >= 3136) {
        remote_button = remote_button - 3136;
      }
      if (remote_button >= 1088) {
        remote_button = remote_button - 1088;
      }
//      Serial.println(remote_button, DEC);
      switch (remote_button) {
        case 0:
          point_right();
          break;
        case 1:
          say_yes();
          break;
        case 2:
          stand_right_arm();
          break;
        case 3:
          stand_left_arm();
          break;
        case 4:
          say_no();
          break;
        case 5:
          look_right();
          break;
        case 6:
          look_left();
          break;
        case 7:
          bye();
          break;
        case 8:
          turn_right();
          break;
        case 9:
          turn_left();
          break;
        case 13:
          point_left();
          break;
        case 15:
          workout_arms();
          break;
        case 18:
          workout_waist();
          break;
        case 38:
          workout_neck();
          break;
        case 20:
          check();
          break;
        case 34:
          reset(0);
          break;
        case 36:
          dance_2();
          break;
        case 57:
          if (auto_move) {
            auto_move = false;
            reset(0);
          } else {
            auto_move = true;
            auto_move_num = 0;
            time_to_auto_move = 0;
          }
          break;
      }
    }
    irrecv.resume();
  }
}
