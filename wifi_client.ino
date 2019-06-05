#include <SoftwareSerial.h>
SoftwareSerial esp8266(4, 5); //  Rx/Tx
int ledsw420 = 2;
int pirled = 3;
int buzzer = 6;
int sw420 = 12;
int pir = 13;
String ssid = "lab605";
String pwd = "LAB605LAB605";
String address = "192.168.0.18";
int port = 8000;
//int link = 0;
unsigned long t = millis();
void setup() {
  pinMode(ledsw420, OUTPUT);
  pinMode(pirled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200); // baud rate of eps
  esp8266.write("AT+UART_DEF=9600,8,1,0,0\r\n");
  delay(333);
  esp8266.begin(9600);
  Serial.println("SDK version:    " + sendAT("AT+GMR", 1000));
  Serial.println("Mode " + sendAT("AT+CWMODE=1", 1000));
  Serial.println("AP Dissconnect" + sendAT("AT+CWQAP", 1000));
  Serial.println("Join Ap" + sendAT("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"", 7000));
  Serial.println("IP:   " + sendAT("AT+CIFSR", 1000));
 
  Serial.println("RST   " + sendAT("AT+RST", 5000));
  Serial.println("Cipmux:   " + sendAT("AT+CIPMUX=0", 1000));
  //Serial.println("IP:   "+sendAT("AT+CIFSR",1000));
  Serial.println("Send Server:   " + sendserver("Test"));
}

void loop() {
  boolean dsw420 = getsw420(1000);
  boolean dpir = getpirled(1000);
  if (dsw420 == true && dpir == true)
  {
    Serial.println(sendserver("Warning!! Someone's there!!"));
  }
  else if (dsw420 == true && dpir == false)
  {
    Serial.println(sendserver("Warning!!"));
  }
  else if (dsw420 == false && dpir == true)
  {
    Serial.println(sendserver("Someone's there!"));
  }
}
boolean getsw420(int timeinterval)
{
  int mearunt = 0;
  unsigned long t = millis();
  while (millis() < (t + timeinterval))
  {
    if (digitalRead(sw420) == HIGH)
    {
      mearunt++;
    }
  }
  if (mearunt > 1000)
  {
    digitalWrite(ledsw420, HIGH);
    digitalWrite(buzzer, HIGH);
    return true;
  }
  else
  {
    digitalWrite(ledsw420, LOW);
    digitalWrite(buzzer, LOW);
    return false;
  }
}
boolean getpirled(int timeinterval)
{
  int mearunt = 0;
  unsigned long t = millis();
  while (millis() < (t + timeinterval))
  {
    if (digitalRead(pir) == HIGH)
    {
      mearunt++;
    }
  }
  if (mearunt > 200)
  {
    digitalWrite(pirled, HIGH);
    return true;
  }
  else
  {
    digitalWrite(pirled, LOW);
    return false;
  }
}
String getesp8266_response()
{
  String str = ""; 
  char c;  

  while (esp8266.available()) {  
    c = esp8266.read(); 
    str.concat(c);  
    delay(10);  
  }
  str.trim();  
  return str;

}
String sendAT(String atcommand, int itime)
{
  esp8266.println(atcommand);  
  esp8266.flush();  
  delay(itime);
  String str = getesp8266_response();
  return str;
}
String close_ip(byte port) {
  esp8266.println("AT+CIPCLOSE=" + String(port)); //
  esp8266.flush();  
  delay(100);
  String str = getesp8266_response(); 
  if (str.indexOf("OK") != -1) {
    return "OK";
  }
  else {
    return "NG";
  }
}
String sendserver(String s)
{
  esp8266.println("AT+CIPSTART=\"TCP\",\"" + address + "\"," + String(port));
  esp8266.flush();  
  delay(100);
  String str = getesp8266_response();
  Serial.println(str);
  String s1 = s + "\r\n"; //add new line
  esp8266.println("AT+CIPSEND=" + String(s1.length()));
  esp8266.flush();  //wait for signal to be tranmistted
  delay(100);
  String str1 = getesp8266_response();
  if (str1.indexOf(">") != -1) {  //receive > start transferring data
    esp8266.println(s1); //transfer data
    esp8266.flush();  
    delay(100);
    String res = getesp8266_response();
    if (res.indexOf("SEND") != -1) {
      return "ok";
    }  //successfully disconnected automatically after transmission
    else {  //unsuccessful transmission must be disconnected by itself
      close_ip(8000); 
      return "fail";
    }
  }
  else {  
    close_ip(8000);  
    return "fail";
  }
}
