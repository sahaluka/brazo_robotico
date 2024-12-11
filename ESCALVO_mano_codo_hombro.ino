#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial BTserial(2, 3);  // RX | TX
Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);
//    HOMBRO     CODO
Servo Servo_H, Servo_C;
const int NUM_SERVOS = 5;

// Ancho de pulso en cuentas para posición 0° y posición 180° de cada servo
unsigned int servoMinPulse[NUM_SERVOS] = {125, 125, 125, 125, 125};
unsigned int servoMaxPulse[NUM_SERVOS] = {800, 800, 800, 800, 800};

// Variables para almacenar los ángulos de los servos
int servoAngle[NUM_SERVOS] = {0, 0, 0, 0, 0};
int ServoRoll = 0;
int ServoPitch = 0;

void setServo(uint8_t n_servo, int angulo) {
  // Limitar el ángulo dentro del rango de 0 a 180
  angulo = constrain(angulo, 0, 180);



  // Mapear el ángulo al rango de pulsos del servo
  int duty = map(angulo, 0, 180, servoMinPulse[n_servo], servoMaxPulse[n_servo]);
  servos.setPWM(n_servo, 0, duty);
}

void setup() {
  Serial.begin(19200);
  BTserial.begin(19200);
  servos.begin();
  servos.setPWMFreq(90);
  Servo_H.attach(6);// HOMBRO
  Servo_C.attach(5);// CODO
  delay(1000);
}

void loop() {
  if (BTserial.available()) {
    String mensaje = BTserial.readStringUntil('\n');

    // Separar los datos del mensaje
    String datos[NUM_SERVOS +2 ];// 5 servos + 2 datos extras
    int index = 0;
    int lastIndex = 0;

    for (int i = 0; i < mensaje.length(); i++) {
      if (isDigit(mensaje.charAt(i)) || mensaje.charAt(i) == '-') {
        lastIndex = i;
        while (i < mensaje.length() && (isDigit(mensaje.charAt(i)) || mensaje.charAt(i) == '-')) {
          i++;
        }
        datos[index] = mensaje.substring(lastIndex, i);
        index++;
      }
    }

    if (index == NUM_SERVOS + 2) {
      // Almacenar los datos en las variables de ángulo de los servos
      for (int i = 0; i < NUM_SERVOS; i++) {
        servoAngle[i] = datos[i].toInt();
      }
      ServoRoll = datos[NUM_SERVOS].toInt();
      ServoPitch = datos[NUM_SERVOS + 1].toInt();
    }
  }

  // Controlar los servos con los ángulos almacenados
  for (int i = 0; i < NUM_SERVOS; i++) {
    setServo(i, servoAngle[i]);
  }
  Servo_H.write(ServoRoll);          // hombro
  Servo_C.write(ServoPitch);          //codo
 
  // Imprimir los valores de los ángulos de los servos por el monitor serial
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print("S");
    Serial.print(i+1);
    Serial.print(":");
    Serial.print(servoAngle[i]);
    Serial.print("\t");
  }
  Serial.print("H:");
  Serial.print(ServoRoll);
  Serial.print("\t");
  Serial.print("C:");
  Serial.print(ServoPitch);
  Serial.println("\t");

  delayMicroseconds(100);
}
