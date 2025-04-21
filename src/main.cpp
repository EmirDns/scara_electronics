#include <Arduino.h>
#include "AccelStepper.h"
#include "MultiStepper.h"

#define BAUD_RATE 9600

#define MOTOR_NUM 2
#define MAPPING_COEFF 999
#define ONE_TURN_STEP 1600

#define MAX_SPEED 10000
#define DESIRED_SPEED 1000

AccelStepper stepperOne(1,2,3);  //2 -> STEP , 3 -> DIR
AccelStepper stepperTwo(1,4,5);  //4 -> STEP , 5 -> DIR
MultiStepper scara;

void commandReader(void);
void assignCommandArray(String given_string);
void mapCommandArray(void);

String command_string;
float unmapped_command_array[MOTOR_NUM];
float mapped_command_array[MOTOR_NUM];

long positions[MOTOR_NUM];

void setup() {
  Serial.begin(BAUD_RATE);
  stepperOne.setMaxSpeed(DESIRED_SPEED);
  stepperTwo.setMaxSpeed(DESIRED_SPEED);

  scara.addStepper(stepperOne);
  scara.addStepper(stepperTwo);
}

void loop() {
  
  commandReader();
  for(int i=0;i<MOTOR_NUM;i++){
    positions[i]=mapped_command_array[i];
  }
  scara.moveTo(positions);
  scara.runSpeedToPosition();
  
}

void commandReader(void){
  static bool receive_flag=false;
  char inc_char=Serial.read();
  delay(1);
  if(Serial.available()>0){
    if(inc_char=='S'){
      command_string="";
      receive_flag=true;
    }
    if(receive_flag && inc_char!='S' && inc_char != 'F'){
      command_string+=inc_char;
    }
    if(inc_char=='F'){
      assignCommandArray(command_string);
      mapCommandArray();
      Serial.println(mapped_command_array[0]);
      Serial.println(mapped_command_array[1]);
      receive_flag=false;
      command_string="";
    }
  }
}

void assignCommandArray(String given_string){
  String str_buffer;
  char direction_char;
  int direction;
  for(int i=0;i<MOTOR_NUM;i++){
    direction_char=given_string[4*i];
    if(direction_char=='0'){
      direction=-1;
    }
    if(direction_char=='1'){
      direction=1;
    }
    for(int j=i*4;j<(i*4)+3;j++){
      str_buffer+=given_string[j+1];    
    }
    unmapped_command_array[i]=direction*str_buffer.toInt();
    str_buffer="";
  }
}

void mapCommandArray(void){
  for(int i=0;i<MOTOR_NUM;i++){
    mapped_command_array[i]=unmapped_command_array[i]*ONE_TURN_STEP/MAPPING_COEFF;
  }
}
