FASTLED_USING_NAMESPACE

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { confetti, sinelon, rainbowWithGlitter };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
//uint8_t gHue = 0;



#define COOLING  55
 
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120
/*void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];
 
  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 3; k > 0; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
 
    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = HeatColor( heat[j]);
    }
}*/



void loop() {

#ifdef ATMEGA32U4

  // it will read MIDI incoming messages if using ATMEGA32U4
  MIDIread();
#endif

#ifdef ATMEGA328
  MIDI.read();
#endif

#ifdef USING_BUTTONS
  buttons();
#endif

#ifdef USING_ENCODER
  encoders();
  isEncoderMoving();

#endif

//Fire2012(); // run simulation frame
//  FastLED.show(); 


  if (lightShowOn) {
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(100/FRAMES_PER_SECOND); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 40 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } 
  }


  cpu.run(); // for threads

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

  for (int i = 0; i < N_M_FADERS; i++) {

    touchLine[i] = capSense[i].capacitiveSensor(capSenseSpeed); // cs_7_8.capacitiveSensor(20); // checks the capacitance in each fader
    checkTouch(i, touchLine[i]); // checks if the fader is beign touched

    updateFader(i, faderPos[i]); // moves the fader if it receives a MIDI in message
    faderPPos[i] = faderPos[i];

  }

#endif

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  //rainbow();
  addGlitter(80);
}

void addGlitter(uint8_t chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CHSV( random8(), 255, 20); //CRGB::White;
  } else {
    fadeToBlackBy( leds, NUM_LEDS, 20);
  
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 30);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 20);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), 20);
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
