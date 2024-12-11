
#include <SoftwareSerial.h>

SoftwareSerial BTserial(2, 3); // RX | TX ESCLAVO
  // CONECTA DESDE EL HC-05 TX AL ARDUINO PIN DIGITAL 2. 
  // CONECTA DESDE EL HC-05 RX AL ARDUINO PIN DIGITAL  3  
// AT+NAME=BRAZO
// AT+ROLE=0
// AT+PSWD="1212"
// AT+UART=19200,0,0
// +ADDR:98D3,61,FD7DB8

char c = ' '; 
 
void setup()  
{
    Serial.begin(9600); 
    //LA VELOCIDAD DE COMUNICACION DEL  HC-05 POR DEFECTO DEL MODO AT ES 38400 EN ALGUNOS CASOS
    BTserial.begin(38400);  
}
 
void loop()
{
 
    
    if (BTserial.available())
    {  
        c = BTserial.read();
        Serial.write(c);
    }
 
    if (Serial.available())
    {
        c =  Serial.read();
        BTserial.write(c);  
    }
 
}
