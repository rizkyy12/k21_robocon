#define R_PWM1 13
#define L_PWM1 12
#define ENCOA1 3

float Kp, Ti, Td, Ki, Kd;
float interval_elapsed;
float interval_limit;

unsigned long lasttimepub, now;

float PID1;
float et1, et1_prev;
float eint1, eint1_prev, eint1_update;
float edif1;
float setPointVel1; 
float SV1, PV1;
float setPointRpm1 = 5000;
int MV1;
unsigned long t1;
double t_1, Ts1;
float maxRpm = 800;
volatile int encoderPos_1 = 0, encReal_Pos1 = 0;
float V1, rpm_m1, ppr = 7;
unsigned long previousMillis1 = 0, previousMillis2 = 0;
const long interval1, interval2 = 100;

void setup() {
  Serial.begin(9600);

  pinMode(R_PWM1, OUTPUT);
  pinMode(L_PWM1, OUTPUT);
  pinMode(ENCOA1, INPUT_PULLUP);
//  pinMode(ENCOB1, INPUT_PULLUP);

  attachInterrupt (digitalPinToInterrupt (ENCOA1), readEncoder1, RISING);

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

  t1 = millis();
  delay(10);

}

void loop() {
  read_rpm();
  now = millis();

  unsigned long currentMillis1 = millis();
  unsigned long currentMillis2 = millis();

  SV1 = abs(setPointRpm1);
  PV1 = rpm_m1;

  t1 = millis();
  Ts1 = (t1 - t_1) / 10;

  et1 = SV1 - PV1;
  eint1_update = ((et1 + et1_prev) * Ts1) / 2;
  eint1 = eint1_prev + eint1_update;
  edif1 = (et1 - et1_prev) / Ts1;

  PID1 = Kp * et1 + Ki * eint1 + Kd * edif1;

  if (PID1 > maxRpm) {
    PID1 = maxRpm;
  }
  else if (PID1 < 0) {
    PID1 = 0;
  }

  else {
    PID1 = PID1;
  }

  PID1 = PID1 / maxRpm;
  MV1 = PID1 * 255;
  analogWrite(R_PWM1, MV1);
  interval_elapsed += Ts1;
  
  if (interval_elapsed >= interval_limit){
    Serial.print("SV: "); Serial.print(SV1); Serial.print("\t");
    Serial.print("PV: "); Serial.print(PV1); Serial.print("\t");
    Serial.print("MV: "); Serial.println(MV1);
  }

  et1_prev = et1;
  eint1_prev = eint1;
  t_1 = t1;
  delay(100);
}

void read_rpm(){
  previousMillis1 = millis();

    if (previousMillis1 - previousMillis2 >= 100){
      rpm_m1 = abs((encoderPos_1 / ppr) * 600);
      encoderPos_1 = 0;
      previousMillis2 = previousMillis1;
    }
}

void readEncoder1() {
  int a = digitalRead(ENCOA1);
  if (a > 0) {
    encoderPos_1--;
//    encReal_Pos1--;
  }
  else {
    encoderPos_1++;
//    encReal_Pos1++;
  }
}
