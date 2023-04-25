#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <AceTMI.h> 
#include <AceSegment.h>

using ace_tmi::SimpleTmi1637Interface;
using ace_segment::Tm1637Module;

const uint8_t CLK = 5;
const uint8_t DIO = 3;
const uint8_t Digits = 4;
const uint8_t Delay = 100;

// got this from documentation on the 7 segment display
 
using TmiInterface = SimpleTmi1637Interface;
TmiInterface tmiInterface(DIO, CLK, Delay);
Tm1637Module<TmiInterface, Digits> ledModule(tmiInterface);

const uint8_t NumPatterns = 10;
const uint8_t PATTERNS[NumPatterns] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
};

#define gridW              (8)
#define gridH              (16)
#define gridArea       (gridW*gridH)
#define ledPin        (6)

#define backwards           (0)

#define pieceW             (4)
#define pieceH             (4)

#define pieceTypes     (7)

#define deadzone  (10)    //joystick deadzone so pieces don't randomly move
#define joyPin        (2)

#define dropSpeed        (25)
#define speedIncrease   (20)  

#define displayPin 3
#define displayClock 5

int pauseButton = 7;
int joyX = A0;
int joyY = A1;

// got from electronoobs.com to help with the 7 segment display
//Variables
byte adr = 0x08;
byte num = 0x00;
int i = 0;
int score = 0;
bool Pause = false;
bool pause_once = false;
bool pause_pressed = false;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

//Function to shift data on the 7 segment module
void shift(byte send_to_address, byte send_this_data)
{
  shiftOut(displayPin, displayClock, MSBFIRST, send_to_address);
  shiftOut(displayPin, displayClock, MSBFIRST, send_this_data);
}


// got these from electronoobs.com. I had no idea how to make pieces using bytes.
// 1 color drawings of each piece in each rotation.
// Each piece is max 4 wide, 4 tall, and 4 rotations.
const char piece_I[] = {
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,

  0, 0, 0, 0,
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
};

const char piece_L[] = {
  0, 0, 1, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

const char piece_J[] = {
  1, 0, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,

};

const char piece_T[] = {
  0, 1, 0, 0,
  1, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

};

const char piece_S[] = {
  0, 1, 1, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  0, 1, 1, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,

  1, 0, 0, 0,
  1, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
};

const char piece_Z[] = {
  1, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,

  0, 0, 0, 0,
  1, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,

  0, 1, 0, 0,
  1, 1, 0, 0,
  1, 0, 0, 0,
  0, 0, 0, 0,
};

const char piece_O[] = {
  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,

  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};


// An array of pointers!
const char *pieces[pieceTypes] = {
  piece_S,
  piece_Z,
  piece_L,
  piece_J,
  piece_O,
  piece_T,
  piece_I,
};


const long piece_colors[pieceTypes] = {
  0x009900, // green S
  0xFF0000, // red Z
  0xFF8000, // orange L
  0x000044, // blue J
  0xFFFF00, // yellow O
  0xFF00FF, // purple T
  0x00FFFF,  // cyan I
};

//end of what I got from electronoobs.com

Adafruit_NeoPixel strip (gridArea, ledPin);

uint32_t white = strip.Color(150, 150, 150);

int prevButState = 0;
int prevJoyX = 0;
int prevRotation = 0;

int pieceId;
int pieceRotation;
int pieceX;
int pieceY;

char piecesAvailable[pieceTypes];

long lastMove;
long moveDelay;  

long lastDrop;
long dropDelay;  

long lastPiece;
long drawDelay; 

long grid[gridW * gridH];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(displayPin, OUTPUT);
  pinMode(displayClock, OUTPUT);
  digitalWrite(displayClock, HIGH);

  pinMode(pauseButton, INPUT_PULLUP);

  int i;
  strip.begin();
  strip.show();

  pinMode(joyPin, INPUT);

  for (i = 0; i < gridArea; ++i) {
    grid[i] = 0;
  }

  newPiece();

}


void loop() {
  // put your main code here, to run repeatedly:
  long t = millis();

  // Chris I am so sorry I had to hard code the score on the 
  // 7 digit display I couldn't think of anything else to do

    if (score == 0) {
      ledModule.setPatternAt(3, PATTERNS[0]);
    } else if (score == 10) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[1]);
    } else if (score == 20) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[2]);
    } else if (score == 30) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[3]);
    } else if (score == 40) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[4]);
    } else if (score == 50) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[5]);
    } else if (score == 60) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[6]);
    } else if (score == 70) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[7]);
    } else if (score == 80) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[8]);
    } else if (score == 90) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[9]);
    } else if (score == 100) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[0]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 110) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[1]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 120) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[2]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 130) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[3]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 140) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[4]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 150) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[5]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 160) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[6]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 170) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[7]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 180) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[8]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 190) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[9]);
      ledModule.setPatternAt(1, PATTERNS[1]);
    } else if (score == 200) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[0]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 210) {
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[1]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 220) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[2]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 230) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[3]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 240) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[4]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 250) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[5]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 260) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[6]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 270) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[7]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 280) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[8]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 290) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[9]);
      ledModule.setPatternAt(1, PATTERNS[2]);
    } else if (score == 300) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[0]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 310) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[1]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 320) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[2]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 330) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[3]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 340) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[4]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 350) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[5]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 360) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[6]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 370) {

      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[7]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 380) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[8]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 390) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[9]);
      ledModule.setPatternAt(1, PATTERNS[3]);
    } else if (score == 400) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[0]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 410) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[1]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 420) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[2]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 430) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[3]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 440) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[4]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 450) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[5]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 460) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[6]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 470) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[7]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 480) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[8]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 490) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[9]);
      ledModule.setPatternAt(1, PATTERNS[4]);
    } else if (score == 500) {
      
      ledModule.setPatternAt(3, PATTERNS[0]);
      ledModule.setPatternAt(2, PATTERNS[0]);
      ledModule.setPatternAt(1, PATTERNS[5]);
    }

  prevButState = pause_pressed;
  pause_pressed = debounce(pauseButton, prevButState);

  if (pause_pressed == true && prevButState == false) {
    Pause = !Pause;
  }

  if (Pause == true) {
    drawPause();
  } else {

    if (t - lastMove > moveDelay ) {
      lastMove = t;
      playerMovements();
    }

    if (t - lastDrop > dropDelay ) {
      lastDrop = t;
      dropPiece();
    }

    if (t - lastPiece > drawDelay ) {
      lastPiece = t;
      drawGrid();
    }
  }
}

boolean debounce(int aButtonPin, boolean aLastReading) {
  boolean currentReading = digitalRead(aButtonPin);

  if (currentReading == true && aLastReading == false) {
    delay(20);
  }

  return currentReading;
}

//had to get this function from electronoobs as well I was so confused
void p(int x, int y, long color) {
  int a = (gridH - 1 - y) * gridW;
  if ( ( y % 2 ) == backwards ) { // % is modulus.
    // y%2 is false when y is an even number - rows 0,2,4,6.
    a += x;
  } else {
    // y%2 is true when y is an odd number - rows 1,3,5,7.
    a += gridW - 1 - x;
  }
  a %= gridArea;
  strip.setPixelColor(a, color);
}

void drawGrid() {
  int x, y;
  for (y = 0; y < gridH; ++y) {
    for (x = 0; x < gridW; ++x) {
      if ( grid[y * gridW + x] != 0 ) {
        p(x, y, grid[y * gridW + x]);
      } else {
        p(x, y, 0);
      }
    }
  }
  strip.show();
}

void newPiece() {
  if (grid >= pieceTypes ) {
    int j;
    int k;
    int i;
    for (i = 0; i < pieceTypes; ++i) {
      do {
        j = rand() % pieceTypes;
        for (k = 0; k < i; ++k) {
          if (piecesAvailable[k] == j) break;
        }
      } while (k < i);
      piecesAvailable[i] = j;
    }

    pieceId = piecesAvailable + 1;
    pieceY = -4;
    pieceX = 3;
    pieceRotation = 0;
  }
}


void addPiece() {
  int x, y;

  const char *piece = pieces[pieceId] + (pieceRotation * pieceH * pieceW);

  for (y = 0; y < pieceH; ++y) {
    for (x = 0; x < pieceW; ++x) {
      int nx = pieceX + x;
      int ny = pieceY + y;
      if (ny < 0 || ny > gridH) continue;
      if (nx < 0 || nx > gridW) continue;
      if (piece[y * pieceW + x] == 1) {
        grid[ny * gridW + nx] = piece_colors[pieceId];
      }
    }
  }
}


void deleteRow(int y) {
  score = score + 10;

  int x;
  for (; y > 0; --y) {
    for (x = 0; x < gridW; ++x) {
      grid[y * gridW + x] = grid[(y - 1) * gridW + x];
    }
  }
  for (x = 0; x < gridW; ++x) {
    grid[x] = 0;
  }
}


void fallFaster() {
  if (dropDelay > dropSpeed) dropDelay -= speedIncrease;
}


void deleteEntireRow() {
  int x, y, c;
  char row_removed = 0;

  for (y = 0; y < gridH; ++y) {
    c = 0;
    for (x = 0; x < gridW; ++x) {
      if ( grid[y * gridW + x] > 0 ) c++;
    }
    if (c == gridW) {
      deleteRow(y);
      fallFaster();
    }
  }
}


void movePiece() {
  int dx = map(analogRead(joyX), 0, 1023, 512, -512);

  int new_px = 0;
  if (dx > deadzone) {
    new_px = -1;
  }
  if (dx < -deadzone) {
    new_px = 1;
  }

  if (new_px != prevJoyX && pieceFits(pieceX + new_px, pieceY, pieceRotation) == 1) {
    pieceX += new_px;
  }

  prevJoyX = new_px;
}

void rotate() {
  int turn = 0;
  int dy = map(analogRead(joyY), 0, 1023, 512, -512);

  if (dy > deadzone) {
    turn = 1;
  }

  if (turn == 1 && turn != prevRotation) {
    int new_pr = ( pieceRotation + 1 ) % 4;
    if (pieceFits(pieceX, pieceY, new_pr)) {
      pieceRotation = new_pr;
    } else {
      if (pieceFits(pieceX - 1, pieceY, new_pr)) {
        pieceX = pieceX - 1;
        pieceRotation = new_pr;
      } else if (pieceFits(pieceX + 1, pieceY, new_pr)) {
        pieceX = pieceX + 1;
        pieceRotation = new_pr;
      }
    }
  }
  prevRotation = turn;
}

int pieceFits(int px, int py, int pr) {
  if ( gridBoundaries(px, py, pr) ) return 0;
  if ( (px, py, pr) ) return 0;
  return 1;
}

int gridBoundaries(int px, int py, int pr) {
  int x, y;
  const char *piece = pieces[pieceId] + (pr * pieceH * pieceW);

  for (y = 0; y < pieceH; ++y) {
    for (x = 0; x < pieceW; ++x) {
      int nx = px + x;
      int ny = py + y;
      if (ny < 0) continue;
      if (piece[y * pieceW + x] > 0) {
        if (nx < 0) return 1;
        if (nx >= gridW ) return 1;
      }
    }
  }
  return 0;
}

void restartScreen()
{
  for (int i = 0; i < gridArea; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.setPixelColor(55, white);
  strip.setPixelColor(74, white);
  strip.setPixelColor(77, white);
  strip.setPixelColor(83, white);
  strip.setPixelColor(85, white);
  strip.setPixelColor(90, white);
  strip.setPixelColor(91, white);
  strip.setPixelColor(92, white);
  strip.setPixelColor(93, white);
  strip.setPixelColor(98, white);
  strip.setPixelColor(101, white);
  strip.setPixelColor(106, white);
  strip.setPixelColor(107, white);
  strip.setPixelColor(108, white);
  strip.setPixelColor(109, white);
  strip.show();
  pause_once = true;
  delay(1000);
}

void gameOverScreen()
{
  for (int i = 0; i < gridArea; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    delay(10);
    strip.show();
  }
}

void dropPiece() {
  if (pieceFits(pieceX, pieceY + 1, pieceRotation)) {
    pieceY++;
    addPiece();
  } else {
    addPiece();
    deleteEntireRow();
    if (gameOver() == 1) {
      gameOverScreen();
    }
    newPiece();
  }
}


void dropFaster() {
  int y = map(analogRead(joyY), 0, 1023, 512, -512);

  if (y < -deadzone) {
    dropPiece();
  }
}


void playerMovements() {
  movePiece();
  rotate();
  addPiece();
  dropFaster();
}


int gameOver() {
  int x, y;
  const char *piece = pieces[pieceId] + (pieceRotation * pieceH * pieceW);

  for (y = 0; y < pieceH; ++y) {
    for (x = 0; x < pieceW; ++x) {
      int ny = pieceY + y;
      int nx = pieceX + x;
      if (piece[y * pieceW + x] > 0) {
        if (ny < 0) return 1; 
      }
    }
  }

  return 0;
}

void drawPause()
{
  for (int i = 0; i < gridArea; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }

  strip.setPixelColor(53, white);
  strip.setPixelColor(58, white);
  strip.setPixelColor(66, white);
  strip.setPixelColor(67, white);
  strip.setPixelColor(68, white);
  strip.setPixelColor(69, white);
  strip.setPixelColor(74, white);
  strip.setPixelColor(77, white);
  strip.setPixelColor(82, white);
  strip.setPixelColor(83, white);
  strip.setPixelColor(84, white);
  strip.setPixelColor(85, white);
  strip.show(); 
  if (!pause_once)
  {
    pause_once = true;
    delay(1000);
  }
}
