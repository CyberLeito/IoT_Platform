/* Alarma de Gas Metano

By: http://elprofegarcia.com/
Connect with the Arduino Gas Sensor MQ4 and when the level of concentration of methane gas
Exceeds a limit an audible alarm is activated, the level of gas metado can be displayed by
Serial Monitor.
connections:
ARDUINO      MQ4      Buzzer
5V           Vcc
GND          GND
A0           A0
Pin 13                  +
GND                    GND
*/

int valor_limite= 200;                  // limit at which alarm is activated    
                                        // Set the value after displaying the level with the Serial Monitor
void setup() { 
  Serial.begin(9600);                   // Enable Serial port at 9600 baud
  pinMode(13,OUTPUT);                   // Sets the output for Buzzer Pin 13
}

void loop() { 
  Serial.println(analogRead(A0));       // Send the Serial the sensor value read MQ4 
 
  if(analogRead(A0) > valor_limite){    //  If the measurement of methane gas is greater than limit value
       digitalWrite(13, HIGH);          // Turn the buzzer connected to Pin 13
   }
   else{                                // If you are under the limit value off the Buzzer
      digitalWrite(13, LOW); 
   }
  delay (300);                          // Wait 300ms for the next step
}
