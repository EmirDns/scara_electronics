#include <AccelStepper.h>
#include "ros.h"
#include "std_msgs/Float64MultiArray.h"

//Defines
#define MOTOR_NUM 1

//Class Declarations
AccelStepper myStepper(1,2,4);  //2 -> STEP , 3 -> DIR
 
//Function Declarations
void multiArrToFloatArr(std_msgs::Float64MultiArray &given_multi_arr, float*dest_float_arr);
void scaraCommandCallBack(const std_msgs::Float64MultiArray &scara_command_array);

//Variables
unsigned long old_time=0;
std_msgs::Float64MultiArray scara_feedback_array;
float scara_command_float_arr[MOTOR_NUM];
std_msgs::Float64MultiArray deneme_array;


//ROS Classes
ros::NodeHandle nh;
ros::Publisher scara_feedback_publisher("scara_feedback_topic", &scara_feedback_array);
ros::Subscriber<std_msgs::Float64MultiArray> scara_command_subscriber("deneme_topic", scaraCommandCallBack);

ros::Publisher deneme_pub("deneme_topic", &deneme_array);

void setup()
{   
    nh.initNode();
    nh.advertise(scara_feedback_publisher);
    nh.subscribe(scara_command_subscriber);
    nh.advertise(deneme_pub);
    
    scara_feedback_array.data=(float*)malloc(sizeof(float)*MOTOR_NUM);
    scara_feedback_array.data_length=MOTOR_NUM;

    deneme_array.data=(float*)malloc(sizeof(float)*MOTOR_NUM);
    deneme_array.data_length=MOTOR_NUM;
    for(int i=0;i<MOTOR_NUM;i++){
        deneme_array.data[i]=-1600;
    }

    myStepper.setMaxSpeed(10000);
}


void loop()
{       
    myStepper.moveTo(scara_command_float_arr[0]);
    myStepper.setSpeed(1000);
    myStepper.runSpeedToPosition();

    if(millis()-old_time>10){
        old_time=millis();
        nh.spinOnce();
        scara_feedback_array.data[0]=myStepper.currentPosition();
        scara_feedback_publisher.publish(&scara_feedback_array);
        deneme_pub.publish(&deneme_array);
    }
}

//Function Descriptions
void multiArrToFloatArr(std_msgs::Float64MultiArray given_multi_arr, float*dest_float_arr){
    for(int i=0;i<MOTOR_NUM;i++){
        dest_float_arr[i]=given_multi_arr.data[i];
    }
}

void scaraCommandCallBack(const std_msgs::Float64MultiArray &scara_command_array){  
    multiArrToFloatArr(scara_command_array, scara_command_float_arr);
}
