
const int HALL_B = 2;       // Pin digital 2
const int HALL_C = 3;       // Pin digital 3
volatile int contador = 0;  //contador de pulsos

void setup() {
  pinMode(HALL_B, INPUT);  // Configurar pin 2 como entrada
  pinMode(HALL_C, INPUT);  // Configurar pin 3 como entrada
  Serial.begin(9600);      // Inicializar comunicación serial a 9600 baudios

  //attachInterrupt(0, interrupcion, RISING);  //flancos de subida pin 2 (HALL_B)
  //attachInterrupt(1, interrupcion, RISING);  //flancos de subida pin 3 (HALL_C)
  attachInterrupt(0, interrupcion, CHANGE);  //flancos de subida pin 2 (HALL_B)
  attachInterrupt(1, interrupcion, CHANGE);  //flancos de subida pin 3 (HALL_C)
}

void loop() {
  int stateB = digitalRead(HALL_B);  // Leer estado del pin 2
  int stateC = digitalRead(HALL_C);  // Leer estado del pin 3

  //Enviar datos al Monitor Serie de Arduino
  Serial.print("HALL B: ");
  Serial.print(stateB);
  Serial.print(", HALL C: ");
  Serial.print(stateC);
  Serial.print(", PULSOS: ");
  Serial.println(contador);
}

void interrupcion() {
    contador++;
}