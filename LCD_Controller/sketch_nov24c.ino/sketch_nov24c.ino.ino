#define MAX_MAP_HEIGHT 6

#define FD_MAX  80          //frame delay ( .64  s)
#define FD_MIN  2           //frame delay ( .016 s)
#define FD_STEP 3           //
#define FD_LEVEL_TIME 250   //            (5.0   s)
#define FD_LEVEL_TIME_INC 50  //          ( .4   s)

#define MAX_JUMP_HEIGHT 2

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

void printLED(const unsigned char *arr, int t) { // t = 1 for 8ms
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

const unsigned char mapInit[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
const unsigned char zero[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char intro[][8] = {
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA},
  {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}
};

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 8; i++) {
    pinMode(Hpins[i], OUTPUT);
    pinMode(Vpins[i], OUTPUT);
  }
  
  copyMap(mapInit, currentMap);
  clearMap(player);
  clearMap(air);
  clearMap(finalMap);

  Serial.begin(9600);
}

// [rightMost]
// bit    7 6 5 4 3 2 1 0
// height +             -
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

  r = analogRead(A4) + analogRead(A5);
  if(r & 0x0002)
    generatedHeight = currentHeight;
  else if(currentHeight >= (1 << (MAX_MAP_HEIGHT - 1))) {
    if(r & 0x0001)
      generatedHeight = currentHeight;
    else
      generatedHeight = (currentHeight >> 1);
  }
  else if(currentHeight <= 1) {
    if(r & 0x0001)
      generatedHeight = (currentHeight << 1) | currentHeight;
    else
      generatedHeight = currentHeight;
  }
  else if(r & 0x0001) {
    if(r & 0x0004 && (((currentHeight << 1) | 0x0001) >= (1 << (MAX_MAP_HEIGHT - 1)))) {
      currentHeight = (currentHeight << 1) | currentHeight;
      generatedHeight = (currentHeight << 1) | currentHeight;
    }
    else
      generatedHeight = (currentHeight << 1) | currentHeight;
  }
    
  else
    generatedHeight = (currentHeight >> 1);

  scrollLeft(m, generatedHeight);
}

unsigned int score = 0;
unsigned int currentLevel = 0;
unsigned int currentLevelCount = 0;
unsigned int nextLevelTime = FD_LEVEL_TIME;
unsigned int frameDelay(unsigned int score) {
  unsigned int currentLevelTime = currentLevelCount * (FD_MAX - currentLevel);

  if(currentLevel == FD_MAX - FD_MIN) return FD_MIN;
  
  if(currentLevelTime > nextLevelTime) {
    currentLevelCount = 0;
    if(currentLevel + FD_STEP > FD_MAX - FD_MIN)
      currentLevel = FD_MAX - FD_MIN;
    else
      currentLevel += FD_STEP;
    nextLevelTime += FD_LEVEL_TIME_INC;
  }
  
  return FD_MAX - currentLevel;
}

bool jumpKey;
void getUserInput() {
  int c;
  unsigned int i = 0;
  jumpKey = false;
  
  while(Serial.available()) {
    c = Serial.read();
    switch(c) {
      case 'J': //Jump
        jumpKey = true;
        break;
      default:  //Not known
        break;
    }
  }
}

void drawPlayer(unsigned char *p, unsigned int h) {
  clearMap(p);

  if(h <= 7)
    p[7 - h] = 64;
  if(h <= 6)
    p[6 - h] = 64;
}

unsigned int playerCurrentHeight = 2;
void gameOver() {
  printLED(currentMap, FD_MAX);
  printLED(zero, FD_MAX);
  drawPlayer(player, playerCurrentHeight);
  printLED(player, FD_MAX);
  printLED(zero, FD_MAX);
  
  Serial.write('G');
  Serial.write(0);
}

bool go = false;
bool jumping = false;
unsigned int jumpStat = 0;
void doGame() {
  unsigned int fd = frameDelay(score);
  unsigned char playerGroundHeight = 0;

  //Get Ground Height
  for(int i = 0; i < 8; i++) 
    playerGroundHeight |= (((currentMap[i] & 0x40) >> 6) << (7 - i));
  for(int i = 0; i < 8; i++)
    if(playerGroundHeight & (1 << (7 - i))) {
      playerGroundHeight = 8 - i;
      break;
    }

  //Check if player was prevously jumping and touch the ground now
  if(playerCurrentHeight == playerGroundHeight && (jumping && jumpStat == 0)) {
    jumping = false;
  }

  //Check if player should do free fall
  if(playerCurrentHeight != playerGroundHeight && !jumping) {
    jumping = true;
    jumpStat = 0;
  }

  //Player is in the air
  if(jumping) {
    //Check if player should go up
    if(jumpStat > 0) {
      jumpStat -= 1;
      playerCurrentHeight += 1;
    }
    else {
      playerCurrentHeight -= 1;
    }
    
    if(playerCurrentHeight < playerGroundHeight) {
      //Hit the terrian, game over
      go = true;
      return;
    }
  }

  //Print the map
  printLED(currentMap, fd >> 1);
  fd -= (fd >> 1);

  //Flash the player
  drawPlayer(player, playerCurrentHeight);
  copyMap(currentMap, finalMap);
  orMap(player, finalMap);
  printLED(finalMap, fd);

  //Generate New Map
  nextScrollMap(currentMap);

  getUserInput();
  if(jumpKey && !jumping) {
    jumping = true;
    jumpStat = 2;
  }

  score += 1;
  Serial.write('S');
  Serial.write(2);
  Serial.write(score >> 8);
  Serial.write(score & 0xFF);
  currentLevelCount += 1;
}

bool gameStart = false;
void loop() {
  if(!gameStart) {
    getUserInput();
    if(jumpKey) {
      gameStart = true;
      return;
    }
    for(int i = 0; i < 2; i++) {
      printLED(intro[i], 125);
    }
  }
  else if(go) {
    gameOver();
    return;
  }
  else {
    doGame();
  }
}
