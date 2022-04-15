void readEncoder1() {
  int a = digitalRead(ENCOA1);
  if (a > 0) {
    encoderPos_1--;
  }
  else {
    encoderPos_1++;
  }
}
