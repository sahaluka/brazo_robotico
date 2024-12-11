

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
SoftwareSerial BTserial(3, 2);  // RX | TX

#define MPU 0x68  // Dirección I2C del MPU-6050

double AcX, AcY, AcZ;

int Pitch, Roll;
void initMPU() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // Registro PWR_MGMT_1
  Wire.write(0);     // Configuramos en cero (despierta el MPU-6050)
  Wire.endTransmission(true);
  delay(1000);
}

void readMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Empezamos con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);        // Requerimos un total de 6 registros
  AcX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}

double calculateAngle(double A, double B, double C) {
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B * B) + (C * C);
  DatoB = sqrt(DatoB);

  Value = atan2(DatoA, DatoB);
  Value = Value * 180 / 3.14;

  return (int)Value;
}

////////////////////////////////////////////////////////////////////
//Filtro Media Móvil como Pasa Bajos
//An=a*M+(1-a)*An
//alpha 1: Sin filtro

//alpha 0: Filtrado totalmente
//alpha clásico 0.05

#define beta 0.06

int Roll_filtrada = 0;   //hombro
int Pitch_filtrada = 0;  //codo

int ServoRoll = 0;
int ServoPitch = 0;

const float alpha = 0.04;

// Definición de constantes para los sensores
const int NUM_SENSORS = 5;
const int SENSOR_PINS[NUM_SENSORS] = { A0, A1, A2, A3, A6 };
//
const int SENSOR_RAW_MIN[NUM_SENSORS] = { 224, 246, 247, 209, 247 };
const int SENSOR_RAW_MAX[NUM_SENSORS] = { 300, 370, 359, 332, 332 };

const int SENSOR_FILTERED_MIN[NUM_SENSORS] = { 0, 0, 0, 0, 0 };
const int SENSOR_FILTERED_MAX[NUM_SENSORS] = { 1023, 1023, 1023, 1023, 1023 };

// Definición de constantes para los servos
const int NUM_SERVOS = 5;
const int SERVO_MIN[NUM_SERVOS] = { 0, 0, 0, 0, 0 };
const int SERVO_MAX[NUM_SERVOS] = { 179, 179, 179, 179, 179 };

const int SERVO_ANGLE_MIN[NUM_SERVOS] = { 0, 0, 0, 0, 0 };
const int SERVO_ANGLE_MAX[NUM_SERVOS] = { 130, 180, 180, 180, 180 };

// Variables para los sensores sin filtrar
int sensor_raw[NUM_SENSORS] = { 0 };

// Variables para los sensores filtrados
int sensor_filtered[NUM_SENSORS] = { 0 };

// Variables para los servos
int servo[NUM_SERVOS] = { 0 };

void setup() {
  Serial.begin(19200);
  BTserial.begin(19200);
  initMPU();  // Inicializamos el MPU6050
  delay(1000);
}

void loop() {
  // Lectura de los sensores
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensor_raw[i] = analogRead(SENSOR_PINS[i]);
  }

  // Filtro de media móvil para los sensores
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensor_filtered[i] = alpha * sensor_raw[i] + (1 - alpha) * sensor_filtered[i];
  }

  // Mapeo de los valores de los servos
  for (int i = 0; i < NUM_SERVOS; i++) {
    servo[i] = map(sensor_filtered[i], SENSOR_RAW_MIN[i], SENSOR_RAW_MAX[i], SERVO_MIN[i], SERVO_MAX[i]);
    servo[i] = constrain(servo[i], SERVO_ANGLE_MIN[i], SERVO_ANGLE_MAX[i]);
  }

  //Mpu6050

  readMPU();  // Adquirimos los ejes AcX, AcY, AcZ.

  Roll = calculateAngle(AcX, AcZ, AcY);   // Cálculo del ángulo Roll
  Pitch = calculateAngle(AcY, AcX, AcZ);  // Cálculo del ángulo Pitch

  // Filtro de media ponderada (filtro de paso bajo)
  Roll_filtrada = (beta * Pitch) + ((1 - beta) * Roll_filtrada);
  Pitch_filtrada = (beta * Roll) + ((1 - beta) * Pitch_filtrada);

  // Mapeo de los ángulos filtrados a los rangos de los servos
  ServoRoll = map(Roll_filtrada, -50, 50, 0, 180); //HOMBRO
  ServoPitch = map(Pitch_filtrada, -30, 30, 0, 180); //CODO

  // Limitación de los valores de los servos dentro de los rangos permitidos
  ServoRoll = constrain(ServoRoll, 1, 179);    // hombro
  ServoPitch = constrain(ServoPitch, 45, 125);  // codo

  // Envío de los datos por el puerto serie
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensor_filtered[i]);
    Serial.print("  ");
    Serial.print(servo[i]);
    Serial.print(" \t");
  }
  // Impresión de los valores por el puerto serie
  Serial.print("H_ROLL:");
  Serial.print(Roll_filtrada);
  Serial.print(" ");
  Serial.print(ServoRoll);
  Serial.print("\t");
  Serial.print("C_PITCH:");
  Serial.print(Pitch_filtrada);
  Serial.print(" ");
  Serial.print(ServoPitch);
  Serial.print("\t");

  // Envío de los datos por Bluetooth
  String mensaje = "";
  for (int i = 0; i < NUM_SENSORS; i++) {
    mensaje += String(servo[i]);
    mensaje += "   ";
  }
  mensaje += String(ServoRoll);
  mensaje += "   ";
  mensaje += String(ServoPitch);
  mensaje += "   ";

  BTserial.println(mensaje);
  Serial.print("msn enviado : ");
  Serial.println(mensaje);

  delayMicroseconds(1000);
}

