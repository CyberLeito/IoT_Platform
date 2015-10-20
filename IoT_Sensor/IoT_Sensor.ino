#include <SoftwareSerial.h>
#define DEBUG true 

//#######################################################
String WiFi_SSID = "\"jaheen_wifi\"";
String WiFi_Pass = "\"veyofushi\"";
String HOSTIP = "\"192.168.0.112\""; //IP of raspberry pi
String HOSTPort = "90"; // webserver port on raspberry pi
String DName = "Weather Report";
String Dtype = "Sensor"; 

String dev1 ="Air Quality!";//device name followed by an exclamation mark
String dev2 ="Temperature!";
//String dev3 ="Fan:10!";     // add more sensors with 'dev4' 'dev5'  'dev6'
String DList =dev1; //add all the sensors you have included dev1 + dev2 + dev3
//#######################################################

SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String Data ="";
String DlistX="CIFSR";

int len_str= 0;
//int len_xra= 0;
int len_total=0; // calculated length - length that may vary

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
  
  for(int i=4;i<14;i++){
    pinMode(i,OUTPUT);
  }
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);
  digitalWrite(13,LOW);
     
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  //sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);
  sendData("AT+CWJAP="+WiFi_SSID+","+WiFi_Pass+"\r\n",7000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
//  ----------------------------------------------------------------------------------------------
  Some();

  DlistX+=DList;
  Data+="CIFSR";
  Data+=DName;
  Data+="CIFSR";
  Data+=Dtype;
  Data+=DlistX;
  //len_xra=Dtype.length();
  //len_xra+=DName.length();
  //-------------------------------------------------
  int Data_len=Data.length();
  //int Dlist_len=DlistX.length();
  
  Serial.print("DataLength : ");//16 --+2 +125 = 143
  Serial.println(Data_len);
  //----------------------------------------------------

  len_total+=len_str;
  len_total+=Data_len;
  len_total-=7;
  //len_total+=len_xra;
  String Length = String(len_total);
  
//  Serial.print("Total_Length: ");
//  Serial.println(len_total);
//Serial.println(Length);
  
  
// -------------------------------------------------------------------------------------------- 
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  //sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  sendData("AT+CIPSTART=4,\"TCP\","+HOSTIP+","+HOSTPort+"\r\n",1000,DEBUG);
  sendData("AT+CIPSTATUS\r\n",1000,DEBUG);
  sendData("AT+CIPSEND=4,"+Length+"\r\n",1000,DEBUG);
  
  String GetComm ="GET http://";
  GetComm+=HOSTIP;
  GetComm+=":";
  GetComm+=HOSTPort;
  GetComm+="/connect.php?data=";
  GetComm+=Data;
  GetComm+="\r\n";
  
  sendData(GetComm,1000,DEBUG);
  //sendData("GET http://"+ShostIP+":"+Port"/connect.php?data="+Data+"\r\n",1000,DEBUG);
  sendData(" HTTP/1.1\r\n",3000,DEBUG);
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  sendData("AT+CIPSTATUS\r\n",1000,DEBUG);

}
  
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
 
    
    if(esp8266.find("+IPD,"))
    {
     delay(2000); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int conx = esp8266.read();
//     Serial.println(conx);
     
     int connectionId = conx-48; // subtract 48 because the read() function returns 
            // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
     Serial.println(connectionId);
     esp8266.find("pin="); // advance cursor to "pin="
     
     int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
     pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
     
//     if(pinNumber == 20){
//       Serial.println(pinNumber);
//       Serial.println("RESETIING");
//       setup();
//       }
//     else
         digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    
     
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,1000,DEBUG); // close connection
     //////////////////////////////////////////////////
    }
  }
}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

void Some(){
  String response = "";
  const int timeout =1000;
  bool debug = true;
  
  esp8266.print("AT+CIFSR\r\n"); // send the read character to the esp8266
    
  long int time = millis();
    
  while( (time+timeout) > millis())
  {
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      if(c != '\n' && c !='\r' && c !='"'){
        response+=c;
      }
      
    }  
  }
  if(debug)
    {
//      Serial.println(response);
      Data = response;
//      Serial.println("Value inserted");
      len_str=response.length();
      Serial.print("LenSTR: ");
      Serial.println(len_str);
    }
}
