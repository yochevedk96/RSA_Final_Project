int threshold = 5;
int direction_of_rotation; //hold value of 1 for right and -1 for left
void setup() {
  Serial.begin(9600);
}


void loop() {
  int value1 = analogRead(A0);
  Serial.println("Analog value right: ");
  Serial.println(value1);
  delay(1000);
  int value2 = analogRead(A1);
  Serial.println("Analog value left: ");
  Serial.println(value2);
  delay(1000);
  if (abs(value1 - value2) > threshold) {
    direction_of_rotation = find_direction(value1, value2);
  }
  Serial.println("Direction of rotation: ");
  Serial.println(direction_of_rotation);
}

int find_direction(int value1, int value2) {
  if (value1 - value2 > 0) { //if difference is positive, the right photoresistor is higher so move to the right
    return 1;
  }
  else { //if difference is negative, the left photoresistor is higher so move to the left
    return -1;
  }
}
