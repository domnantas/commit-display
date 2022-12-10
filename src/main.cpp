#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

#define PIN 4

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                                   NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

void initWiFi()
{
  const char *ssid = "ssid";
  const char *password = "password";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.print(" Connected! IP: ");
  Serial.print(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  matrix.begin();
  matrix.setBrightness(255);
  initWiFi();
}

const uint16_t color_level_0 = matrix.Color(0, 0, 0);
const uint16_t color_level_1 = matrix.Color(0, 5, 0);
const uint16_t color_level_2 = matrix.Color(0, 20, 0);
const uint16_t color_level_3 = matrix.Color(0, 100, 0);
const uint16_t color_level_4 = matrix.Color(0, 160, 0);

void loop()
{
  matrix.fillScreen(0);
  matrix.drawPixel(0, 0, color_level_1);
  matrix.drawPixel(2, 0, color_level_2);
  matrix.drawPixel(4, 0, color_level_3);
  matrix.drawPixel(6, 0, color_level_4);

  // matrix.setCursor(x, 0);
  // matrix.print(F("Howdy"));
  // if (--x < -36)
  // {
  //   x = matrix.width();
  //   if (++pass >= 3)
  //     pass = 0;
  //   matrix.setTextColor(colors[pass]);
  // }
  matrix.show();
  delay(100);
}
