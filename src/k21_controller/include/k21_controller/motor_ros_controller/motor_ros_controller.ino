#include <ros.h>
#include <std_msgs/Float64.h>

#define R_PWM1 13
#define L_PWM1 12
#define ENCOA1 3

//PID Motor 1 - 4 Wheel PG 45 KNY 21

unsigned long lasttimepub, now;

// -- PID M1
float PID1, PID2;
float et1, et1_prev;
float eint1, eint1_prev, eint1_update;
float edif1;

float setPointRpm1;
float setPointVel1;

float Kp, Ti, Td, Ki, Kd;
float SV1, PV1;
int MV1;

unsigned long t;
double t_prev, Ts;
unsigned long previousMillis1 = 0, previousMillis2 = 0;
const long interval1, interval2 = 100;

float interval_elapsed;
float interval_limit;

int maxRpm = 800;

volatile int encoderPos_1;
float rpm_m1, pwm1, pid1_m1, ppr = 7;
long pm = 0, pm2 = 0;

// ============== ROS =============
ros::NodeHandle nh;
std_msgs::Float64 vel1_state;
std_msgs::Float64 pid1_state;
std_msgs::Float64 rpm1_state;

ros::Publisher vel1("/vel_m1", &vel1_state);
ros::Publisher pid1("/pid_m1", &pid1_state);
ros::Publisher rpm1("/rpm_m1", &rpm1_state);

void setPoint_Cb1(std_msgs::Float64 &msg_motor) {
  setPointVel1 = msg_motor.data;
  if (setPointVel1 <= 3.0 and setPointVel1 >= -3.0) {
    setPointRpm1 = 0;
  }
  else {
    setPointRpm1 = setPointVel1 * 9.5492965964254;
  }
}

ros::Subscriber<std_msgs::Float64> m1("/k_21/motor1", setPoint_Cb1);

void setup() {
  pinMode(R_PWM1, OUTPUT);
  pinMode(L_PWM1, OUTPUT);
  pinMode(ENCOA1, INPUT_PULLUP);
  //pinMode(ENCOB1, INPUT_PULLUP);

  attachInterrupt (digitalPinToInterrupt (ENCOA1), readEncoder1, RISING);

  nh.initNode();
  nh.advertise(vel1);
  nh.advertise(rpm1);
  nh.advertise(pid1);
  nh.subscribe(m1);

  Kp = 0.009121;  //0.01021
  Ti = 4.957;   //8.057
  Td = 1.071;   //1.071

  if (Ti == 0) {
    Ki = 0;
  }
  else {
    Ki = Kp / Ti;
  }
  //--> Hitung Kd
  Kd = Kp * Td;

  et1_prev = 0;
  eint1_prev = 0;

  interval_limit = 0.05;
  interval_elapsed = 0;

  t = millis();

}

void loop() {
  now = millis();
  
  unsigned long currentMillis1 = millis();
  unsigned long currentMillis2 = millis();

  pid_m1();

  if (currentMillis1 - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis1;

    if (setPointVel1 > 0.00001) {
      vel1_state.data = MV1;
      vel1.publish(&vel1_state);
      pid1_state.data = PID1;
      pid1.publish(&pid1_state);
      analogWrite(R_PWM1, MV1);
      analogWrite(L_PWM1, 0);
    }

    else if (setPointVel1 < -0.000001) {
      vel1_state.data = (MV1 * -1);
      vel1.publish(&vel1_state);
      pid1_state.data = (PID1 * -1);
      pid1.publish(&pid1_state);
      analogWrite(R_PWM1, 0);
      analogWrite(L_PWM1, MV1);
    }
    else {
      vel1_state.data = 0;
      vel1.publish(&vel1_state);
      pid1_state.data = 0;
      pid1.publish(&pid1_state);
      analogWrite(R_PWM1, 0);
      analogWrite(L_PWM1, 0);
    }
    
    nh.spinOnce();

    if (currentMillis2 - previousMillis2 >= interval2) {
      previousMillis2 = currentMillis2;
      rpm_m1 = abs((encoderPos_1 / ppr) * 600);
      rpm1_state.data = setPointRpm1;
      rpm1.publish(&rpm1_state);
      encoderPos_1 = 0;
    }
    nh.spinOnce();
    delay(10);
  }
}
