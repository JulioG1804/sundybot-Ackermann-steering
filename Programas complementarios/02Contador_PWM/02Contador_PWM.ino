
const int pwmPin = 7;

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin, INPUT);
}

void loop() {
  unsigned long duracion = pulseIn(pwmPin, HIGH);
  Serial.println(duracion);
  //delay(10);
}
