#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11,  5,  4,  3,  2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

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
        lcd.print(val);
        break;
      case 'G': //Gameover
        l = Serial.read();
        i = 0;
        for(i = 0; i < l; i++) {
          while(!Serial.available());
          v  = Serial.read();
        }
        break;
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
  }

  if(analogRead(A4) > 512) {
    Serial.write('J');
  }
  delay(1);
}
