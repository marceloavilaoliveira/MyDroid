<h1 align="center">MyDroid</h1>
<h4 align="center">Android Vinyl Action Figure Automation</h4>

## INFORMATION

See the project details at [PCBWay > Projects](https://www.pcbway.com/project/shareproject/Android_Vinyl_Action_Figure_Automation_b8069f17.html)

## VERSIONS

### 2.5.0 (13/07/13)

* Add button to turn on/off the "auto mode"
* Add function to play tones (two types)
* Add function to play tones when changing auto mode

### 2.4.0 (21/05/13)

* Do some "auto mode" improvements
* Add control by the Serial Port
* Create variables for head_min, head_avg, head_max, body_min etc
* Create a perl program prototype to send commands for Arduino using serial port
* Add sound library and a play_sound function to play sounds
* Add 4 sounds: Ta-Ta-Ta-Ta-Ta Ta-Ta, Star Wars, Imperial March and James Bond
* Add Ta-Ta sound to move_check function
* Add move_monitor_left, move_monitor_right and move_monitor_center functions
* Add MyDroid.pl script to send commands to MyDroid through Serial Port

### 2.3.0 (21/07/12)

* Create "Counter" feature
* Create "move_random()" function and added a "Rand." button to the "Free Moves" feature
* Add the reset(1) and reset(2) functions when sleeping and waking (respectively)
* Rewrite the reset, sleep/wake(check_sleep), check_proximity, check_count, check_bluetooth functions
* Add commands to wake the droid as soon as any android menu was accessed
* Do some adjusts in "Auto" movements

### 2.2.0 (30/06/12)

* Create "Simon" feature
* Add the possibility to pass two values (instead of one) by bluetooth
* Create "reset(1)" (go to reset position) and "reset(2)" (to to last position) functions
* Create "happy_eyes" and "angry_eyes" functions
* Fix bug in bluetooth communication (add "noise" because of read to fast)

### 2.1.0 (31/05/12)

* Create proximity sensor
* Create Sleep feature
* Create "Move Auto" feature
* Create "Move Look around" command

### 2.0.0 (25/04/12)

* Create control by Bluetooth

* Create Android App:

  "Prog Moves" function
  "Free Moves" function

* Change/improve commands:

  Blink eyes
  Reset
  Move Happy
  Move Angry
  Move Scary
  Move Say Hi
  Move Say Bye
  Move Say Yes
  Move Say No
  Move Look left
  Move Look right
  Move Turn left
  Move Turn right
  Move Point left
  Move Point right
  Move Workout arms
  Move Workout neck
  Move Workout waist
  Move Dance
  Move Random1
  Move Random2
  Move Check
  Free Moves

* Remove control by Infrared Remote

### 1.0.0 (14/03/12)

* Controled by Infrared Remote

* When turned On:

  Blink eyes
  Wait for commands

* Commands available:

  Blink eyes
  Reset
  Move Bye
  Move Check
  Move Dance1
  Move Dance2
  Move Look left
  Move Look right
  Move Point left
  Move Point right
  Move Random1
  Move Random2
  Move Say no
  Move Say yes
  Move Stand left arm
  Move Stand right arm
  Move Turn left
  Move Turn right
  Move Workout arms
  Move Workout neck
  Move Workout waist
