#include <SoftwareSerial.h>

SoftwareSerial BTserial(3, 2); // RX | TX MAESTRO
  // CONECTA DESDE EL HC-05 TX AL ARDUINO PIN DIGITAL 3. 
  // CONECTA DESDE EL HC-05 RX AL ARDUINO PIN DIGITAL 2,
//AT+NAME=GUANTE
//AT+ROLE=1
//AT+PSWD="1212"
//AT+UART=19200,0,0
//AT+CMODE=0
//AT+BIND=98D3,61,FD7DB8
    

 // MAC ESCLAVO +ADDR:21:13:1014F

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

