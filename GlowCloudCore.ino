#include <Adafruit_NeoPixel.h>

#define pix 32   // how many pixels

int pinputs[3]={16,18,17};  // {button,encA,encB}

boolean hold[3];
long unsigned holdTimer[3];

int speeds[10]={3,5,7,11,16,25,37,56,85,128};


  Adafruit_NeoPixel strip[5]={
  Adafruit_NeoPixel(pix, 8 ,NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(pix, 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(pix, 3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(pix, 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(pix, 5, NEO_GRB + NEO_KHZ800)};

  
void setup() {
  Serial.begin(115200);
  for(int a=0;a<3;a++){
    pinMode(pinputs[a],INPUT_PULLUP);
  }
  for(int a=0;a<5;a++){
    strip[a].begin();
  }
}


const int divi=768/pix;
int color;
int oldcolor;
int spd=5;             // speed setting
int pgm;               // program
int lpixel;            //lightningpixel;
int lstrip=1;
int lcolor;
long unsigned ltimer;  // lighting timer
long unsigned ctimer;  // color change timer
long unsigned  timer;  //timer

boolean setupz=1;
boolean updateC;
boolean flashCore;

void loop() {
  timer=millis();
  for(int a=0;a<3;a++){
    if(!digitalRead(pinputs[a])&&!hold[a]&&timer>holdTimer[a]){
      holdTimer[a]=timer+2;
      hold[a]=1;
      if(a==0){
        pgm=(pgm+1)%6;
        setupz=1;
      }
      if(a==1&&!hold[2]){spd=min(spd+1,9);color=min(color+5,255);}
      if(a==2&&!hold[1]){spd=max(0,spd-1);color=max(0,color-5);}
    }
    if(digitalRead(pinputs[a])&&hold[a]&&timer>holdTimer[a]){
      holdTimer[a]=timer+10;
      hold[a]=0;
      Serial.print("P:");
      Serial.print(pgm);
      Serial.print(" C:");
      Serial.print(color);
      Serial.print(" S:");
      Serial.println(speeds[spd]);
    }
  }

  
  if(pgm==0){
    if(setupz){
      setupz=0;
      for(int a=0;a<6;a++){
        dimPixel(0,a,255);
      }
      strip[0].show();
    }
    if(timer>ctimer){
      ctimer=timer+speeds[spd];
      color+=3;
      color%=768;
      for(int a=1;a<5;a++){
        for(int b=0;b<pix;b++){
          setPixel(a,b,(b*divi)+color,255);          
        }
        strip[a].show();
      }
    }
  }
  
  if(pgm==1){
    if(timer>ctimer){
      ctimer=timer+speeds[spd];
      color+=3;
      color%=768;
      for(int a=0;a<5;a++){
        for(int b=0;b<pix;b++){
          setPixel(a,b,color,255);          
        }
        strip[a].show();
      }
    }
  }


  if(pgm==2){
    if(setupz){
      setupz=0;
      dimAll();
      showAll();
    }
    if(color!=oldcolor){
      oldcolor=color;
      for(int a=0;a<5;a++){
        for(int b=0;b<pix;b++){
          setPixel(a,b,color*3,255);
        }
        strip[a].show();
      }
    }
  }

  if(pgm==3){
    if(setupz){
      setupz=0;
      dimAll();
      showAll();
    }
    if(timer>ltimer){
      ltimer=timer+3;
      if(flashCore){
        flashCore=0;
        dimAll();
        strip[0].show();
      }
      if(!lpixel){
        ltimer+=8;
        for(int a=0;a<pix;a++){
          dimPixel(0,a,255);
        }
        strip[0].show();
        flashCore=1;
      }
      dimPixel(lstrip,lpixel,255);
      strip[lstrip].show();
      lpixel++;
      if(lpixel==pix){
        dimAll();
        ltimer=timer+random(speeds[spd]*40);
        lpixel=0;
        lstrip=random(4)+1;
        lcolor=random(768);
      }
    }
  }
  if(pgm==4){
    if(setupz){
      setupz=0;
      dimAll();
      showAll();
    }
    if(timer>ltimer){
      ltimer=timer+3;
      if(flashCore){
        flashCore=0;
        dimAll();
        strip[0].show();
      }
      if(!lpixel){
        ltimer+=8;
        for(int a=0;a<pix;a++){
          dimPixel(0,a,255);
        }
        strip[0].show();
        flashCore=1;
      }
      setPixel(lstrip,lpixel,lcolor,255);
      strip[lstrip].show();
      lpixel++;
      if(lpixel==pix){
        dimAll();
        ltimer=timer+random(speeds[spd]*40);
        lpixel=0;
        lstrip=random(4)+1;
        lcolor=random(768);
      }
    }
  }

  if(pgm==5){
    if(timer>ctimer){
      ctimer=timer+speeds[spd];
      for(int a=0;a<5;a++){
        setPixel(a,random(pix),random(768),255);
        strip[a].show();
      }
      dimAll();
    }
  }




  
}

void setPixel(int stripz, int pixelz, int colorz, long unsigned fadez){
  colorz%=768;
  pixelz%=pix;
  fadez*=fadez;
  fadez/=255;
  
  if(colorz<256){
    colorz=(colorz*fadez)/255;
    strip[stripz].setPixelColor(pixelz,colorz,0,fadez-colorz);
  }
  if(colorz>=256&&colorz<512){
    colorz-=256;
    colorz=(colorz*fadez)/255;
    strip[stripz].setPixelColor(pixelz,fadez-colorz,colorz,0);
  }
  if(colorz>=512){
    colorz-=512;
    colorz=(colorz*fadez)/255;
    strip[stripz].setPixelColor(pixelz,0,fadez-colorz,colorz);
  }
}

void dimPixel(int stripz, int pixelz, long unsigned fadez){
  fadez*=fadez;
  fadez/=255;
  pixelz%=pix;
  strip[stripz].setPixelColor(pixelz,fadez,fadez,fadez);
}

void dimAll(){
  for(int a=0;a<5;a++){
    for(int b=0;b<pix;b++){
      dimPixel(a,b,0);
    }
    strip[a].show();
  }
}

void showAll(){
  for(int a=0;a<5;a++){
    strip[a].show();
  }
}
