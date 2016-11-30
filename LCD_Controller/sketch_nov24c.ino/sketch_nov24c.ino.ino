#include <time.h>

#define MAX_MAP_HEIGHT 6

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

/**
 * Map
 */

void copyMap(const unsigned char *src, unsigned char *dst) {
  int i = 0;
  for(i = 0; i < 8; i++) { dst[i] = src[i]; }
}

void orMap(const unsigned char *src, unsigned char *dst) {
  int i = 0;
  for(i = 0; i < 8; i++) { dst[i] |= src[i]; }
}

void clearMap(unsigned char *m) {
  int i = 0;
  for(i = 0; i < 8; i++) { m[i] = 0; }
}

unsigned char currentMap[8];
unsigned char player[8];
unsigned char air[8];
unsigned char finalMap[8];

unsigned char mapInit[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 8; i++) {
    pinMode(Hpins[i], OUTPUT);
    pinMode(Vpins[i], OUTPUT);
  }
  srand(time(0));
  
  copyMap(mapInit, currentMap);
  clearMap(player);
  clearMap(air);
  clearMap(finalMap);
}

void scrollLeft(unsigned char *arr, unsigned rightMost) {
  int i = 0;
  for(i = 0; i < 8; i++) {
    arr[i] = (unsigned char)(arr[i] << 1 | ((rightMost & (1 << (7 - i))) >> (7 - i)));
  }
}

void nextScrollMap(unsigned char *m) {
  int r, i;
  unsigned char currentHeight = 0;
  unsigned char generatedHeight;

  i = 0;
  for(i = 0; i < 8; i++)
    currentHeight |= ((m[i] & 0x01) << (7 - i));

  r = rand();
  if(r & 0x00000002)
    generatedHeight = currentHeight;
  else if(currentHeight >= (1 << (MAX_MAP_HEIGHT - 1)) || currentHeight <= 1)
    generatedHeight = currentHeight;
  else if(r & 0x00000001)
    generatedHeight = (currentHeight << 1) | currentHeight;
  else
    generatedHeight = (currentHeight >> 1);

  scrollLeft(m, generatedHeight);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
