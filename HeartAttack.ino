#define USE_ARDUINO_INTERRUPTS true
#include<LiquidCrystal.h>
#include <PulseSensorPlayground.h>  

PulseSensorPlayground pulseSensor;
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

//변수선언
String sec = "2000";
int i = 0;
int btn = 2;
int bzz = 7;
int pulse = 0;
bool emerge = false;
bool over = false;
String cprManual[][3] = {{"    Calling", "      119", sec},
                   {"   Requesting", "for help", "*"},  //CPR 메뉴얼 텍스트배열
                   {"This patient is", "in cardiac arrest", sec}, 
                   {"    He needs", "    your help", sec},
                   {"  Press to show", "CPR steps", "*"},
                   {"Place the one ha", "nd in the center", sec},
                   {"of the chest", "", "*"},
                   {"Place the heel o", "f the other hand", sec},
                   {"directly on top", "of the first", sec},
                   {"of the first", "", "*"},
                   {" Interlace your", "fingers", "*"},
                   {" Position your", "   shoulders", sec},
                   {" directly over", "yourhands", "*"},
                   {" Keep your arm", "    straight", sec},
                   {" Push 5cm down", "", "*"},
                   {"  Now CPR with", "following beats", sec},
                   {"   until 119", "    arrives", sec},
                   {"     Ready?", "", "*"},
                   {"       3", "", "1000"},
                   {"       2", "", "1000"},
                   {"       1", "", "1000"},
                   {"       GO", "", "500"}};


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(btn, INPUT_PULLUP);
  pinMode(bzz, OUTPUT);
  pulseSensor.analogInput(pulse);   
  pulseSensor.setThreshold(550);
  if (pulseSensor.begin()) {
    Serial.println("Start");
  }
}

void loop() {
  if(emerge == true){  //환자가 위급상황인 경우
    if(i==21){
      repeatBuzz(30);
      rest(10);
      if(over==true){  //시스템 종료
        lcd.clear();
        }
      }
    else{  //CPR메뉴얼 표시단계
      printTxt();
      cprNext();
      i += 1;
      }
    }
  else{  //평소상태(BPM표시)
    int myBPM = pulseSensor.getBeatsPerMinute();
    if (pulseSensor.sawStartOfBeat()) {
      Serial.println("asd");
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("BPM:");
      lcd.setCursor(6, 1);
      lcd.print(myBPM);
      if (Serial.available() > 0 || myBPM == 0){ //심장이 멈췄을때 위급상황으로 전환
        int inData = Serial.read();
        if (inData == 'e'){
          emerge=true;
          }
        }
      }
    delay(1000);
    }
}

void printTxt() {  //LCD에 글씨표시하는 함수
  lcd.clear();
  for(int j=0;j<2;j++){
    lcd.setCursor(0, j);
    lcd.print(cprManual[i][j]);
    }
  
}
void blinkAndWait(){  //텍스트를 깜빡여주는 함수
  long prev = 0;
  bool m = true;
  while (digitalRead(btn) == HIGH){
    long current = millis();
    if(current - prev >= 1000){
        prev = current;
        m = !m;
        lcd.setCursor(9, 1);
        if(m == true){
          digitalWrite(bzz, LOW);
          lcd.print("       ");
          }
        else{
          if(i == 1){digitalWrite(bzz, HIGH);} 
          lcd.print("(PRESS)");
          }
        }

    }
  digitalWrite(bzz, LOW);
  while(digitalRead(btn) == LOW){
    delay(50);
    }
  
  }

void cprNext(){
  if(cprManual[i][2] == "*"){
    blinkAndWait();
    }
  else{   
    delay(cprManual[i][2].toInt());
    }
  }
  
void repeatBuzz(int del){  //CPR 박자 알려주는 함수
  long prev = 0;
  int k = del;
  while (k>0 && over == false){
    long current = millis();
    if(current - prev >= 600){
      prev = current;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("CPR    Press");
      lcd.setCursor(2, 1);
      lcd.print("       to stop");
      lcd.setCursor(2, 1);
      lcd.print(k);
      digitalWrite(bzz, HIGH);
      delay(50);
      digitalWrite(bzz, LOW);
      k -= 1;
      }
    if(digitalRead(btn) == LOW){
      over = true;
      }
    }
  }

void rest(int tme){  //CPR한 후 쉬는 함수
  long prev = 0;
  int k = tme;
  while (k>0 && over == false){
    long current = millis();
    if(current - prev >= 1000){
      prev = current;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Rest   Press");
      lcd.setCursor(2, 1);
      lcd.print("       to stop");
      lcd.setCursor(2, 1);
      lcd.print(k);
      k -= 1;
      }
    if(digitalRead(btn) == LOW){
      over = true;
      }
    }
  }
