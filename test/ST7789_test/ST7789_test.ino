#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include <math.h> // Include math.h for floor function

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BLK 32

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 172

#define FRAME_RATE 2

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;
unsigned long MIN_FRAME_TIME = 1000 / FRAME_RATE;  // in miliseconds
unsigned long lastFrameTime;
float temperature = 0;

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Starting ..."));

  tft.init(SCREEN_HEIGHT, SCREEN_WIDTH);  // Init ST7789 172x320

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);

  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  lastFrameTime = millis();

  drawTemperatureBox();
  isHigherThanThreshold(10, 5); 
}
 
void loop() {
  // tft.fillScreen(ST77XX_BLACK);
  if (millis() - lastFrameTime > 1000.0/FRAME_RATE) {
    lastFrameTime = millis();
    isHigherThanThreshold(temperature, 50); 
    udpateTemp(temperature);
    temperature += 43.15;
    int timeElapsed = temperature;
    updateProgressBar(timeElapsed, 300);
  }
  if (temperature > 300) {
    temperature = -50;
    resetDisplay();
  }
}

#define TEMP_BOX_WIDTH (SCREEN_WIDTH / 2)
#define TEMP_BOX_HEIGHT (SCREEN_HEIGHT / 2)


void drawTemperatureBox() {
  tft.fillRect(0, 0, TEMP_BOX_WIDTH, TEMP_BOX_HEIGHT, ST77XX_BLUE);  // A blue box for temperature display
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.setCursor(20, 20);
  tft.print("Temp: ");
}

void isHigherThanThreshold(const float number, const float threshold) {
  if (number > threshold) {
    tft.fillRect(10 + TEMP_BOX_WIDTH, 0, TEMP_BOX_WIDTH, TEMP_BOX_HEIGHT, ST77XX_RED);
  } else {
    tft.fillRect(10 + TEMP_BOX_WIDTH, 0, TEMP_BOX_WIDTH, TEMP_BOX_HEIGHT, ST77XX_GREEN);
    }
}

void udpateTemp(float newTemp) {
  tft.setCursor(TEMP_BOX_WIDTH + 10 , 20);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  char buffer[5];
  dtostrf(newTemp, 5, 1, buffer); // Convert float to string with 5 total digits and 2 after the decimal
  tft.print(buffer);
}

const int progressBarX = 10;     // X position of progress bar
const int progressBarY = TEMP_BOX_HEIGHT + 20;    // Y position of progress bar
const int progressBarWidth = SCREEN_WIDTH - 2*progressBarX ; // Width of progress bar
const int progressBarHeight = 50; // Height of progress bar
unsigned long startTime;

void updateProgressBar(float time, float thresholdTime) {
  // Calculate the width of the filled part based on the progress (0.0 to 100)
  float percentProgress = time / thresholdTime;

  // Draw the filled portion of the bar
  tft.fillRect(progressBarX, progressBarY, percentProgress * progressBarWidth, progressBarHeight, ST77XX_GREEN);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(percentProgress*progressBarWidth - 60 , progressBarY + progressBarHeight/2 );
  char buffer[4]; // Make sure the buffer is large enough
  itoa(percentProgress*100, buffer, 10); // Convert integer to string in base 10
  buffer[2] = '%'; // Add the new character
  buffer[3] = '\0'; // Add the null terminator
  tft.print(buffer);
}

void resetDisplay() {  // Optional: Clear remaining part if bar is updated backward
  tft.fillScreen(ST77XX_BLACK);
  // tft.fillRect(progressBarX , progressBarY, progressBarWidth, progressBarHeight, ST77XX_BLACK);
  // lastFrameTime = millis();
  drawTemperatureBox();
  isHigherThanThreshold(10, 5); 
}