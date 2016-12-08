#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11,  5,  4,  3,  2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

char lastState = 0;
void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  int c, l, i, v, val;

  while(Serial.available()) {
    c = Serial.read();
    switch(c) {
      case 'S': //Score
        while(!Serial.available());
        l = Serial.read();
        i = 0;
        val = 0;
        for(i = 0; i < l; i++) {
          while(!Serial.available());
          v  = Serial.read();
          val = v << ((1 - i) * 8);
        }
        if(lastState != 'S') {
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 0);
        }
        lcd.print(val);
        break;
      case 'G': //Gameover
        while(!Serial.available());
        l = Serial.read();
        i = 0;
        val = 0;
        for(i = 0; i < l; i++) {
          while(!Serial.available());
          v  = Serial.read();
          val = v << ((1 - i) * 8);
        }
        if(lastState != 'G') {
          lcd.print(val);
          lcd.setCursor(0, 1);
          lcd.print("Game Over");
        }
        break;
      case 'P': //Prestart
        while(!Serial.available());
        l = Serial.read();
        i = 0;
        val = 0;
        for(i = 0; i < l; i++) {
          while(!Serial.available());
          v  = Serial.read();
          val = v << ((1 - i) * 8);
        }
        if(lastState != 'P') {
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print("Press button");
          lcd.setCursor(0, 1);
          lcd.print("to start");
        }
      /*case 'N': //Reset Jump
        digitalWrite(13, LOW);
        while(!Serial.available());
        l = Serial.read();
        i = 0;
        for(i = 0; i < l; i++) {
          while(!Serial.available());
          v  = Serial.read();
        }
        break;*/
      default:
        break;
    }
    lastState = c;
  }
  
  if(analogRead(A4) > 512) {
    Serial.write('J');
  }
  delay(1);
}
