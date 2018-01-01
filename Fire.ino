/**
 * Arduino Uno - NeoPixel Fire
 * v. 1.0
 * Copyright (C) 2015 Robert Ulbricht
 */
#include <Adafruit_NeoPixel.h>

#define PIN 2 // data pin
#define CNT 7 // led count

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, PIN, NEO_GRBW + NEO_KHZ800);

uint32_t fire_color   = strip.Color ( 80/4,  35/4,  00);
uint32_t off_color    = strip.Color (  0,  0,  0);
uint32_t white_color  = strip.Color ( 255, 255, 255 );

///
/// Fire simulator
///
class NeoFire
{
  Adafruit_NeoPixel &strip;
 public:

  NeoFire(Adafruit_NeoPixel&);
  void Draw();
  void Clear();
  void AddColor(uint8_t position, uint32_t color);
  void SubstractColor(uint8_t position, uint32_t color);
  uint32_t Blend(uint32_t color1, uint32_t color2);
  uint32_t Substract(uint32_t color1, uint32_t color2);
};

///
/// Constructor
///
NeoFire::NeoFire(Adafruit_NeoPixel& n_strip)
: strip (n_strip)
{
}

///
/// Set all colors
///
void NeoFire::Draw()
{
Clear();

for(int i=0;i<CNT;i++)
  {
  AddColor(i, fire_color);
  int r = random(80);
  uint32_t diff_color = strip.Color ( r, r/2, r/2);
  SubstractColor(i, diff_color);
  }
  
strip.show();
}

///
/// Set color of LED
///
void NeoFire::AddColor(uint8_t position, uint32_t color)
{
uint32_t blended_color = Blend(strip.getPixelColor(position), color);
strip.setPixelColor(position, blended_color);
}

///
/// Set color of LED
///
void NeoFire::SubstractColor(uint8_t position, uint32_t color)
{
uint32_t blended_color = Substract(strip.getPixelColor(position), color);
strip.setPixelColor(position, blended_color);
}

///
/// Color blending
///
uint32_t NeoFire::Blend(uint32_t color1, uint32_t color2)
{
uint8_t r1,g1,b1;
uint8_t r2,g2,b2;

r1 = (uint8_t)(color1 >> 16),
g1 = (uint8_t)(color1 >>  8),
b1 = (uint8_t)(color1 >>  0);

r2 = (uint8_t)(color2 >> 16),
g2 = (uint8_t)(color2 >>  8),
b2 = (uint8_t)(color2 >>  0);

return strip.Color(constrain(r1+r2, 0, 255), constrain(g1+g2, 0, 255), constrain(b1+b2, 0, 255));
}

///
/// Color blending
///
uint32_t NeoFire::Substract(uint32_t color1, uint32_t color2)
{
uint8_t r1,g1,b1;
uint8_t r2,g2,b2;
int16_t r,g,b;

r1 = (uint8_t)(color1 >> 16),
g1 = (uint8_t)(color1 >>  8),
b1 = (uint8_t)(color1 >>  0);

r2 = (uint8_t)(color2 >> 16),
g2 = (uint8_t)(color2 >>  8),
b2 = (uint8_t)(color2 >>  0);

r=(int16_t)r1-(int16_t)r2;
g=(int16_t)g1-(int16_t)g2;
b=(int16_t)b1-(int16_t)b2;
if(r<0) r=0;
if(g<0) g=0;
if(b<0) b=0;

return strip.Color(r, g, b);
}

///
/// Every LED to black
///
void NeoFire::Clear()
{
for(uint16_t i=0; i<strip.numPixels (); i++)
  strip.setPixelColor(i, off_color);
}

NeoFire fire(strip);

void burn() {
  fire.Draw();
  delay(random(50,150)/1.75);
}

void intensity(int i){
  strip.setBrightness(i);
}

void maxIntensity() {
  for (int x = 60; x < 255; x = x + 10 ) {
    strip.setBrightness(x);
    burn();
  }
}

void rampDownFire() {
   for (int x = 255; x > 0; x-- ) {
    strip.setBrightness(x);
    delay(random(50,150)/1.75);
    burn();
  }
  strip.setBrightness(0); 
}

void cleanLEDs(){
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(0);
}
