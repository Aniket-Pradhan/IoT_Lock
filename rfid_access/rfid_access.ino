#include <SPI.h>
#include <MFRC522.h> 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); 

int lock = 2;
int buzz = 3;

String master;
String prog1 = "";
String prog2 = "";
String prog3 = "";
int key = 0;
 int t1 = 0;

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  pinMode(lock, OUTPUT);
  pinMode(buzz, OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  master = "23 E3 54 C3";
  prog1 = "";
  prog2 = "";
  prog3 = "";
  key = 0;
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
 

}
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == master) {
    progMode();
    Serial.println("Progged");
  }
  else {
    Serial.println(prog1);
    if (content.substring(1) == prog1 || content.substring(1) == prog2 || content.substring(1) == prog3){
      Serial.println("Authorized");
      digitalWrite(lock,HIGH);
      delay(15000);
      digitalWrite(lock,LOW);
    }
    else {
      digitalWrite(buzz,HIGH);
      Serial.println("UnAuth");
      delay(1000);
      digitalWrite(buzz,LOW);
    }
  }
  delay(3000);
  
}

String fix(String progg){
  String t = "";
  for(int i=0; i<progg.length(); i++){
    if(i != 0){
      t += progg[i];
    }
  }
  return(t);
}

void progMode(void) {
  Serial.println("programming now");
  delay(2000);
  String contentt= "";
  byte letter;
  mfrc522.PICC_IsNewCardPresent();
  mfrc522.PICC_ReadCardSerial();
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     contentt.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     contentt.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println("contentt " + contentt);
  contentt.toUpperCase();
  if(prog1 == "") {
    prog1 = contentt;
    prog1 = fix(prog1);
    Serial.println(prog1 + " Lol");
  }
  else if(prog2 == "") {
    Serial.println(prog2);
    prog2 = contentt;
    prog2 = fix(prog2);
  }
  else if(prog3 == "") {
    Serial.println(prog3);
    prog3 = contentt;
    prog3 = fix(prog3);
  }
  else {
    key += 1;
    if(key % 3 == 1) {
      Serial.println(prog1);
      prog1 = contentt;
      prog1 = fix(prog1);
    }
    else if(key %3 == 2) {
      Serial.println(prog2);
      prog2 = contentt;
      prog2 = fix(prog2);
    }
    else {
      Serial.println(prog3);
      prog3 = contentt;
      prog3 = fix(prog3);
    }
  }
}
