const int Hpins[] = { 2,  3,  4,  5,  6,  7,  8,  9}; //-
const int Vpins[] = {10, 11, 12, 13, A0, A1, A2, A3}; //+

//10 11 12 13  6  7  8  9
// 2  3  4  5 A0 A1 A2 A3

void clearLED() {
  int i;
  for(i = 0; i < 8; i++) {
    digitalWrite(Vpins[i], LOW);
    digitalWrite(Hpins[i], LOW);
  }
}

void printLED(unsigned char *arr, int t) { // t = 1 for 8ms
  int i, j, k;
  unsigned char v, l;
  for(k = 0; k < t; k++) {
    for(i = 0; i < 8; i++) {    //Vertical
      digitalWrite(Vpins[(i + 7) % 8], LOW);
      digitalWrite(Vpins[i], HIGH);
      v = arr[i];
      
      for(j = 0; j < 8; j++) {  //Horizental
        l = v & (1 << (7 - j));
        l = (l > 0) ? (LOW) : (HIGH);
        digitalWrite(Hpins[j], l);
      }
      delay(1);
    }
  }
  clearLED();
}

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 8; i++) {
    pinMode(Hpins[i], OUTPUT);
    pinMode(Vpins[i], OUTPUT);
  }
  
}

unsigned char test1[] = {
  0xC3,
  0xC3,
  0xC3,
  0xFF,
  0xFF,
  0xC3,
  0xC3,
  0xC3
};

unsigned char test2[] = {
  0x7E,
  0x7E,
  0x40,
  0x7E,
  0x7E,
  0x40,
  0x7E,
  0x7E
};

unsigned char test3[] = {
  0x60,
  0x60,
  0x60,
  0x60,
  0x60,
  0x60,
  0x7E,
  0x7E
};

unsigned char test4[] = {
  0xFF,
  0xFF,
  0xC3,
  0xC3,
  0xC3,
  0xC3,
  0xFF,
  0xFF
};

unsigned char zero[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

void scrollLeft(unsigned char *arr, unsigned rightMost) {
  int i = 0;
  for(i = 0; i < 8; i++) {
    arr[i] = (unsigned char)(arr[i] << 1 | ((rightMost & (1 << (7 - i))) >> (7 - i)));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  printLED(test1, 120);
  printLED(zero, 5);
  printLED(test2, 120);
  printLED(zero, 5);
  printLED(test3, 120);
  printLED(zero, 5);
  printLED(test3, 120);
  printLED(zero, 5);
  printLED(test4, 120);
  printLED(zero, 5);

  /*for(int i = 0; i < 8; i++) {
    digitalWrite(Vpins[i], HIGH);
    digitalWrite(Hpins[i], LOW);
  }*/
}
