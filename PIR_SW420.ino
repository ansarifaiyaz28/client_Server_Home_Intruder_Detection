#include <SoftwareSerial.h>

SoftwareSerial esp8266(4,5);
String ssid = "LAB605";
String pwd = "lab605lab605";
String extractTime = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Arduino...OK");
  delay(1000);
  esp8266.begin(9600);
//  delay(1000);
  esp8266.write("AT+UART_DEF=9600,8,1,0,0\r\n");
  delay(1000);
  Serial.println("AP Dissconnected..."+quit_ap());
  Serial.println("Ap Connected..."+joint_ap(ssid, pwd));
  Serial.println("IP:   "+getesp8266getip());
  Serial.println("mode:   "+set_mode(1));
  Serial.println("set_mux:   "+set_mux(1));   //0=single connection && 1=multiple connection
  Serial.println("set_server:   "+set_server(1, 80));
  Serial.println("connected to:   "+start_tcp(0,"www.google.com",80));
  Serial.println("DATE:"+msend_data(0,"GET /"));
//  Serial.println(isodate(timme));
  Serial.println(extractTime);
  arrayIndex(extractTime);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (esp8266.available()) {
//    Serial.println("TRUE");
    delay(1000);
  }
  if (esp8266.isListening()) {
//    Serial.println("Port FOUR is listening!"); 
    delay(1000);
  }
  if (esp8266.overflow()) {
//    Serial.println("SoftwareSerial overflow!"); 
  }
//  Serial.println(esp8266.peek());
//  delay(5000);
//  Serial.println(esp8266.read());
//  delay(1000);
}

String getesp8266_response()
{
  String str="";  
  char c;  
  
  while (esp8266.available()) {  //is true when data is received and stored in buffer
    c=esp8266.read();  //data is read as character
    str.concat(c);  //Concatenation character 
    delay(10);  
  }
//  Serial.println(str);
  str.trim();  //remove head and tail whitespace
//  Serial.println(str);
  return str;
}

String quit_ap() {
  esp8266.println("AT+CWQAP");  //Quit Access Point
  esp8266.flush();  //Wait for the sequence to transmit
  delay(1000);
  String str=getesp8266_response();  //get the ESP8266 response string
//  Serial.println(str);
  if (str.indexOf("AT+CWQAP") != -1) {return "OK";}
  else {return "NG";}
}

String joint_ap(String ssid, String pwd) {
  esp8266.println("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"");  //connection
  esp8266.flush();
  delay(7000);
  String str=getesp8266_response();  //get the ESP8266 response string
//  Serial.println(str);
  if (str.indexOf("AT+CWJAP") != -1) {return "OK";}
  else {return "NG";}
}
String getesp8266getip()
{
  esp8266.println("AT+CIFSR");  
  esp8266.flush();  //waiting for the sequence to be transmitted
  delay(1000);
  String str = getesp8266_response();
//  Serial.println(str);
  if (str.indexOf("AT+CIFSR") != -1) {
    String staip = str.substring(str.indexOf("STAIP")+1);   //store ","192.168.50.142"+CIFSR:STAMAC,"60:01:" to staip
    return staip.substring(staip.indexOf(",")+1,staip.indexOf("+")-1);    //returns value between first "," and "+" ex.- 192.168.50.142    
  }
  else {return "NG";}
}

String set_mode(byte mode) {
  esp8266.println("AT+CWMODE=" + String(mode));  //set working mode
  esp8266.flush();  //waiting for the sequence to be transmitted
  delay(1000);
  String str=getesp8266_response();  
//  Serial.println("\n" + str);
  if (str.indexOf("AT+CWMODE") != -1 ) {
    if (mode==1){return "Station Mode";}
    else if (mode==2){return "SoftAp Mode";}
    else if (mode==3) {return "Station + SoftAp Mode";}
    else {return "Not Valid";}
  }
  else {return "NG";}
}

String set_mux(byte mux) {  //0=single, 1=multiple
  esp8266.println("AT+CIPMUX=" + String(mux));  //set connection mode
  esp8266.flush();  //waiting for the sequence to be transmitted
  delay(1000);
  String str=getesp8266_response();  
//  Serial.println(str);
  if (str.indexOf("AT+CIPMUX") != -1) {
    if (mux==0){return "Single Connection";}
    else if (mux==1){return "Multiple Connection";}
    else {return "Not Valid Connection";}
  }
  else {return "NG";}
}

String set_server(byte mode, int port) {  //0=single, 1=multipl
  esp8266.println("AT+CIPSERVER=" + String(mode) +"," + String(port));  
  esp8266.flush();  
  delay(1000);
  String str=getesp8266_response();  
//  Serial.println(str);
  if (str.indexOf("OK") != -1) {return "OK";}
  else {return "NG";}
}

String start_tcp(byte link ,String address, int port) {
  esp8266.println("AT+CIPSTART=" + String(link)+",\"TCP\",\"" + address + "\"," + String(port)); 
  esp8266.flush();  
  delay(1000);
  String str=getesp8266_response();  
//  Serial.println(str);
  if (str.indexOf("OK") != -1) {return address;}
  else {return "None";}
}

String getresponse(int timeout)
{
    String str="";  
  char c;  
   unsigned long t=millis();
    while(millis()<(t+timeout))
    {
      if (esp8266.available()) {
         c=esp8266.read();
          str.concat(c);
//         Serial.print(c);
          }
      }
       str.trim();
  return str;   
}

String msend_data(byte link,String s) { 
  String s1=s + "\r\n";  //\r\n is added to "GET /" to make it 7 byte
  esp8266.println("AT+CIPSEND="+String(link)+","+ String(s1.length())); 
  esp8266.flush();  
  delay(2000);
  String str=getesp8266_response();  
//  Serial.println(str);
  delay(1000);
  esp8266.println(s1); //send data value of s1(seven byte)
  if (str.indexOf(">") != -1) {  
    esp8266.flush();  
    String res = getresponse(5000);
//    Serial.println(res);                    //}
    if (res.indexOf("+IPD") != -1) {
      extractTime = res.substring(res.indexOf("Date:")+11,res.indexOf("GMT"));
//      Serial.println(extractTime.length());
      return res.substring(res.indexOf("Date:")+11,res.indexOf("GMT"));
    }  //successfully disconnected automatically after transmission
    else {  //unsuccessful transmission must be disconnected by itself
      close_ip(0);  //turn off IP connection
      return "NG";
    }   
  }
  else {  //unsuccessful transmission must be disconnected by itself
    close_ip(0); 
    return "NG";
  }
}

String close_ip(byte port) {
  esp8266.println("AT+CIPCLOSE="+String(port));  
  esp8266.flush();  
  delay(1000);
//  String str=getesp8266_response();  
//  if (str.indexOf("OK") != -1) {return "OK";}
//  else {return "NG";}
}

void arrayIndex(String date)
{   
//    int i=0:
    for(int i=0;i<date.length(); i++)
    {
        Serial.print(i);
        Serial.println(date[i]);
    }  
}
