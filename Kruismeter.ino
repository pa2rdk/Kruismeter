// V1.0 2014/06/10  Initial number

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "esp_adc_cal.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define TFT_GREY 0x5AEB
#define DEG2RAD  0.0174532925
#define fwdPin  26
#define refPin  27

// int deg500[]  = {0, 3, 7, 11, 13, 16, 17, 19, 20, 21, 22, 26, 29, 33, 36, 38, 40, 41, 42, 43, 44, 47, 48, 50, 51, 53, 55, 57, 58, 60};
// int val500[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 350, 400, 500};
// bool shw500[] = {1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1,  0,  0,  0,  1,  0,  1,  0,  1, 1};

// int degFwd[]  = {0, 3, 7, 12, 15, 17, 19, 20, 22, 23, 24, 28, 32, 36, 39, 41, 43, 45, 46, 47, 48, 51, 53, 54, 56, 58, 60};
// int valFwd[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300};
// bool shwFwd[] = {1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0,  1,  0,  1,  0,  1,  0,  1};

// int degRef[]  = {0, 5, 9, 14, 18, 21, 23, 25, 27, 29, 30, 35, 39, 44, 48, 51, 55, 57, 59, 60};
// int valRef[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, 50, 70, 80, 90, 100};
// bool shwRef[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1};

int degFwd[]  = {0, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
int valFwd[]  = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 240, 260, 280, 300};
bool shwFwd[] = {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};

int degRef[]  = {0, 6, 13, 19, 25, 31, 37, 41, 45, 47, 49, 52, 55, 57, 60};
int valRef[]  = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 100};
bool shwRef[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1};

int fwdList[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int refList[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int bckColor = TFT_BLACK;
int txtColor = TFT_WHITE;
int needleColor = TFT_YELLOW;
int lastFwd = 0;
int lastRef = 0;
int peakFwd = 0;
long peakTime = millis();
//uint16_t* tft_buffer;

typedef struct {  // Frequency parts
  float x1;
  float y1;
  float x2;
  float y2;  
  float x3;
  float y3;
  float x4;
  float y4;
  int x;
  int y;
} SFreq;

SFreq lPos;
SFreq aPos;

void setup(void) {
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  Serial.begin(115200); // For debug
  tft.init();
  tft.setRotation(3);

  createSprite();
  sprite.pushSprite(0, 0);

  delay(1000);
  for (int i = 0; i <= 300; i += 15){
    sprite.pushSprite(0, 0);
    DrawMeter(i, i/3);
    delay(100);
  }

  delay(1000);
  for (int i = 300; i >= 0; i -= 15){
    sprite.pushSprite(0, 0);
    DrawMeter(i, i/3);
    delay(100);    
  }

  sprite.pushSprite(0, 0);
  tft.setTextColor(txtColor, bckColor);   
  // tft_buffer =  (uint16_t*) malloc( ((100) + 2) * ((100) + 2) * 2 );
  // tft.readRect(25,25,100,100,tft_buffer);
  // tft.pushRect(50,50,100,100,tft_buffer);
  
  //for (;;){}
  analogReadResolution(12);
}

void loop() {
    // int fwd = analogRead(fwdPin)*3300/4095;
    // int ref = analogRead(refPin)*3300/4095;    
    int fwd =analogReadMilliVolts(fwdPin)-128;
    int ref = analogReadMilliVolts(refPin)-128;
    Serial.printf("Fwd:%d - Ref:%d\r\n",fwd,ref);
    fwd = calcAverage(true, fwd);
    ref = calcAverage(false, ref);    
    DrawMeter(fwd, ref);
    delay(10);
}

int calcAverage(bool forward, int measure){
  measure /=10;
  int retVal = 0;
  if (forward){
      for (int x = 0; x<9; x++) fwdList[x] = fwdList[x + 1];
      fwdList[9] = measure;
      for (int x = 0; x<10; x++) retVal += fwdList[x];
  } else {
      for (int x = 0; x<9; x++) refList[x] = refList[x + 1];
      refList[9] = measure;
      for (int x = 0; x<10; x++) retVal += refList[x];
  }
  return retVal/10;
}

void DrawMeter(int fwd, int ref){

  bool peakChanged = false;
  if (peakTime+3000<millis() && fwd<peakFwd){
    peakFwd = 0;
    peakTime = millis();
    peakChanged = true;
  }
  if (fwd>peakFwd){
    peakFwd = fwd;
    peakTime = millis();
    peakChanged = true;
  }
  Serial.printf("fwd = %d (%d), ref = %d (%d), peak = %d (%d, %d)\r\n", fwd, lastFwd, ref, lastRef , fwd, peakFwd, peakChanged);

  if (fwd>lastFwd+1 || fwd<lastFwd-1|| ref>lastRef+1 || ref<lastRef-1 || peakChanged){
    Serial.print(".");
    lastFwd = fwd;
    lastRef = ref;
    sprite.pushSprite(0, 0);
    calcLine(320, 305, 285, 61, 310, fwd, TFT_BLACK, true, true, false);
    calcLine(0, 305, 14, 61, 310, ref, TFT_BLACK, false, true, false);

    float swr = (1 + sqrt(ref/float(fwd)))/(1 - sqrt(ref/float(fwd)));
    swr = round(swr*10)/10;

    tft.drawString("PWR : " + String(fwd)+'W', 15, 37, 2);     
    tft.drawString("PEAK: " + String(peakFwd)+'W', 15, 52, 2); 
    tft.drawString("PWR : " + String(ref)+'W', 230, 37, 2);  
    int color = swr>3?TFT_RED:txtColor; 
    tft.setTextColor(color, bckColor);
    tft.drawString("SWR : " + String(swr), 230, 52, 2);        

    if (fwd!=0) Serial.printf("fwd = %d, ref = %d, delen = %f, sqrt = %d, swr = %f\r\n", fwd, ref, ref/float(fwd), sqrt(ref/float(fwd)), swr);
    if (fwd!=0) tft.drawCentreString(String(int(round(ref*100/fwd))) + "% reflected power", 160, 192, 2);
    tft.setTextColor(txtColor, bckColor);
  }
}

void drawSWRLine(int fwd, int ref, unsigned int color, int linePart){
  calcLine(320, 305, 285, 61, 300, fwd, color, true, false, linePart == 1?true:false);
  calcLine(0, 305, 14, 61, 300, ref, color, false, false, linePart == 1?true:false);
}

void calcLine(int x1, int y1, int start_angle, int sub_angle, int r, int measure, unsigned int color, bool forward, bool doDraw, bool drawReverse){
  if (forward){
    int lIndex = lookUpDeg(degFwd, valFwd, sizeof(valFwd)/sizeof(valFwd[0]), measure);
    float sx = cos((start_angle + lIndex - 90) * DEG2RAD);
    float sy = sin((start_angle + lIndex - 90) * DEG2RAD);
    int x2 = sx * r + x1;
    int y2 = sy * r + y1;
    //tft.drawLine(x1, y1, x2, y2, color);
    if (doDraw) tft.drawLine(280, 230, x2, y2, needleColor);
    if (!doDraw){
      lPos = aPos;
      aPos.x1 = 280;
      aPos.y1 = 230;
      aPos.x2 = x2;
      aPos.y2 = y2;
    }
  } else {
    int lIndex = lookUpDeg(degRef, valRef, sizeof(valRef)/sizeof(valRef[0]), measure);
    float sx = cos((start_angle + sub_angle - lIndex - 90) * DEG2RAD);
    float sy = sin((start_angle + sub_angle - lIndex - 90) * DEG2RAD);
    int x2 = sx * r + x1;
    int y2 = sy * r + y1;
    //tft.drawLine(x1, y1, x2, y2, color);
    if (doDraw) tft.drawLine(40, 230, x2, y2, needleColor);
    if (!doDraw){
      aPos.x3 = 40;
      aPos.y3 = 230;
      aPos.x4 = x2;
      aPos.y4 = y2;
      calcCross();
      if (lPos.x1>0 && lPos.y1>0){
        Serial.printf("DrawLine: %d, %d, %d, %d\r\n", lPos.x, lPos.y, aPos.x, aPos.y);
        sprite.drawLine(lPos.x, lPos.y, aPos.x, aPos.y, color);
        if (drawReverse){
          Serial.printf("DrawReverse: %d, %d, %d, %d\r\n", lPos.x, lPos.y, aPos.x-lPos.x, aPos.y-lPos.y);
          sprite.drawLine(lPos.x, lPos.y, 2*lPos.x-aPos.x, 2*lPos.y-aPos.y, color);
        }
      }
    }
  }
}

void calcCross(){
  float x = ((aPos.x1*aPos.y2-aPos.y1*aPos.x2)*(aPos.x3-aPos.x4)-(aPos.x3*aPos.y4-aPos.y3*aPos.x4)*(aPos.x1-aPos.x2))/((aPos.x1-aPos.x2)*(aPos.y4-aPos.y3)-(aPos.x3-aPos.x4)*(aPos.y2-aPos.y1));
  float y = ((aPos.x1*aPos.y2-aPos.y1*aPos.x2)*(aPos.y3-aPos.y4)-(aPos.x3*aPos.y4-aPos.y3*aPos.x4)*(aPos.y1-aPos.y2))/((aPos.x1-aPos.x2)*(aPos.y4-aPos.y3)-(aPos.x3-aPos.x4)*(aPos.y2-aPos.y1));

  aPos.x = int(x*-1);
  aPos.y = int(y*-1);
  Serial.printf("aPos = %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d, %d\r\n", aPos.x1, aPos.y1, aPos.x2, aPos.y2, aPos.x3, aPos.y3, aPos.x4, aPos.y4, x, y, aPos.x, aPos.y);
}

int lookUpDeg(int deg[], int val[], int elements, int measure){
  float retDeg = 0;
  int pos = -1;
  for (int i = 0; i<elements; i++) {
    
    if (measure <= val[i]) {
      pos = i;
      break;
    }
  }
  if (pos>-1){
    int pos2 = pos;
    int dgr  = deg[pos];
    int dgr2 = dgr;
    if (pos>0) pos2 = pos - 1;
    dgr = deg[pos];
    dgr2 = deg[pos2];
    pos = val[pos];
    pos2 = val[pos2];
    if (pos2-pos != 0) retDeg = (((measure-pos2)/float(pos-pos2))*(dgr-dgr2))+dgr2;
  }
  return int(round(retDeg));
}

int drawSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int color, bool forward, bool withValues){
  float sx = 0;
  float sy = 0;
  int lIndex = 0;
  if (forward){
    sx = cos((start_angle - 90) * DEG2RAD);
    sy = sin((start_angle - 90) * DEG2RAD);
  } else {
    sx = cos((start_angle + sub_angle - 90) * DEG2RAD);
    sy = sin((start_angle + sub_angle - 90) * DEG2RAD);
  }

  int x1 = sx * r + x;
  int y1 = sy * r + y;

  if (forward){
    for (int i = start_angle; i < start_angle + sub_angle; i++) {
      int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;
      int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;

      sprite.drawLine(x1, y1, x2, y2, color);
      lIndex = lookUp(degFwd, sizeof(degFwd)/sizeof(degFwd[0]),  i-start_angle);

      if (withValues && lIndex>= 0){
        int x3 = cos((i - 90) * DEG2RAD) * r + x;
        int y3 = sin((i - 90) * DEG2RAD) * r + y;

        int x4 = cos((i - 90) * DEG2RAD) * (r + (shwFwd[lIndex]?8:4)) + x;
        int y4 = sin((i - 90) * DEG2RAD) * (r + (shwFwd[lIndex]?8:4)) + y;
        sprite.drawLine(x3, y3, x4, y4, color);
        sprite.setTextColor(TFT_GREEN, bckColor);
        if (shwFwd[lIndex]) sprite.drawCentreString(String(valFwd[lIndex]), x4-10, y4-4, 1);
      }

      x1 = x2;
      y1 = y2;
    }
  } else {
    for (int i = start_angle + sub_angle; i > start_angle; i--) {

      int x2 = cos((i - 1 - 90) * DEG2RAD) * r + x;
      int y2 = sin((i - 1 - 90) * DEG2RAD) * r + y;
      Serial.printf(" i = %d - x = %d, %d - y = %d, %d\r\n", i, x1, x2, y1, y2);

      sprite.drawLine(x1, y1, x2, y2, color);
      lIndex = lookUp(degRef, sizeof(degRef)/sizeof(degRef[0]), start_angle+sub_angle-i);

      if (withValues && lIndex>= 0){
        int x3 = cos((i - 90) * DEG2RAD) * r + x;
        int y3 = sin((i - 90) * DEG2RAD) * r + y;

        int x4 = cos((i - 90) * DEG2RAD) * (r + (shwRef[lIndex]?8:4)) + x;
        int y4 = sin((i - 90) * DEG2RAD) * (r + (shwRef[lIndex]?8:4)) + y;
        sprite.drawLine(x3, y3, x4, y4, color);
        sprite.setTextColor(TFT_RED, bckColor);
        if (shwRef[lIndex]) sprite.drawCentreString(String(valRef[lIndex]), x4+10, y4-4, 1);
      }

      x1 = x2;
      y1 = y2;
    }
  }
}

int lookUp(int deg[], int elements, int wantedval){
  int wantedpos = -1;
  for (int i = 0; i<elements; i++) {
    if (wantedval == deg[i]) {
      wantedpos = i;
      break;
    }
  }
  return wantedpos;
}

void createSprite(){
    sprite.setColorDepth(8);
  sprite.createSprite(320, 240);
  sprite.fillRect(0, 0, 320, 240, TFT_GREY);
  sprite.fillRect(3, 3, 314, 234, bckColor);

  drawSegment(320, 305, 285, 61, 300, TFT_BLUE, true, true);
  drawSegment(0, 305, 14, 61, 300, TFT_BLUE, false, true);

  // 4% 1:1.5
  Serial.println("SWRLine 1:1.5");
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(25, 1, TFT_GREEN, 0);
  drawSWRLine(125, 5, TFT_GREEN, 1);
  drawSWRLine(250, 10, TFT_GREEN, 0);  
  sprite.setTextColor(TFT_GREEN, bckColor);
  sprite.drawCentreString("1:1.5", aPos.x+6, aPos.y+33, 1);


  // 11% 1:2
  Serial.println("SWRLine 1:2");
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(9, 1, TFT_GREEN, 0);
  drawSWRLine(100, 11, TFT_GREEN, 1);
  drawSWRLine(163, 18, TFT_GREEN, 0);  
  sprite.setTextColor(TFT_GREEN, bckColor);
  sprite.drawCentreString("1:2", aPos.x+5, aPos.y+20, 1);  

  // 18, 5% 1:2.5
  Serial.println("SWRLine 1:2");
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(5, 1, TFT_GREEN, 0);
  drawSWRLine(40, 7, TFT_GREEN, 1);
  drawSWRLine(60, 11, TFT_GREEN, 0);   
  drawSWRLine(85, 16, TFT_GREEN, 0);  
  drawSWRLine(130, 24, TFT_GREEN, 0);   
  sprite.setTextColor(TFT_GREEN, bckColor);
  sprite.drawCentreString("1:2.5", aPos.x+5, aPos.y+15, 1); 
 
  // 25% 1:3
  Serial.println("SWRLine 1:3");
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(4, 1, TFT_RED, 0);
  drawSWRLine(20, 5, TFT_RED, 1);
  drawSWRLine(40, 10, TFT_RED, 0);
  drawSWRLine(120, 30, TFT_RED, 0);
  sprite.setTextColor(TFT_RED, bckColor);
  sprite.drawCentreString("1:3", aPos.x+1, aPos.y+15, 1);  

  // 36% 1:4
  Serial.println("SWRLine 1:4"); 
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(3, 1, TFT_RED, 0);  
  drawSWRLine(5, 2, TFT_RED, 1);   
  drawSWRLine(11, 4, TFT_RED, 0);
  drawSWRLine(25, 9, TFT_RED, 0);
  drawSWRLine(100, 36, TFT_RED, 0);
  drawSWRLine(105, 38, TFT_RED, 0);  
  sprite.setTextColor(TFT_RED, bckColor);
  sprite.drawCentreString("1:4", aPos.x+1, aPos.y+15, 1);   

  // 50%  1:6
  Serial.println("SWRLine 1:6"); 
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(2, 1, TFT_RED, 0);
  drawSWRLine(30, 15, TFT_RED, 1);
  drawSWRLine(60, 30, TFT_RED, 0);  
  drawSWRLine(85, 42, TFT_RED, 0);  
  sprite.setTextColor(TFT_RED, bckColor);
  sprite.drawCentreString("1:6", aPos.x+3, aPos.y+15, 1);

  // 70%  1:10
  Serial.println("SWRLine 1:6"); 
  aPos = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  drawSWRLine(4, 3, TFT_RED, 0);
  drawSWRLine(10, 7, TFT_RED, 1);  
  drawSWRLine(25, 18, TFT_RED, 0);
  drawSWRLine(40, 28, TFT_RED, 0);    
  drawSWRLine(70, 49, TFT_RED, 0);   
  sprite.setTextColor(TFT_RED, bckColor);
  sprite.drawCentreString("1:10", aPos.x+5, aPos.y+15, 1);

  sprite.setTextColor(TFT_GREY, bckColor);  
  sprite.drawCentreString("POWER/SWR METER", 160, 208, 4);
  sprite.fillRect(0, 230, 320, 10, TFT_GREY);
  sprite.drawCircle(40, 235, 4, txtColor);
  sprite.drawLine(38, 233, 42, 237, txtColor);
  sprite.drawCircle(280, 235, 4, txtColor);
  sprite.drawLine(278, 233, 282, 237, txtColor);    
  sprite.fillRect(60, 230, 200, 10, TFT_GREY);
  sprite.setTextColor(txtColor, TFT_GREY);  
  sprite.drawCentreString("by PI4RAZ", 160, 232, 1);
  sprite.drawRect(9, 27, 87, 45, TFT_GREY);
  sprite.drawRect(224, 27, 87, 45, TFT_GREY);   
  sprite.drawRect(10, 28, 85, 43, TFT_GREY);
  sprite.drawRect(225, 28, 85, 43, TFT_GREY);  
  sprite.setTextColor(TFT_GREEN, bckColor);
  sprite.drawString(" FWD ", 20, 20, 2);    
  sprite.setTextColor(TFT_RED, bckColor);   
  sprite.drawString(" REF ", 235, 20, 2);
  sprite.setTextColor(txtColor, bckColor);  
  sprite.drawString("PWR : ", 15, 37, 2);     
  sprite.drawString("PEAK: ", 15, 52, 2); 
  sprite.drawString("PWR : ", 230, 37, 2); 
  sprite.drawString("SWR : ", 230, 52, 2);  
}