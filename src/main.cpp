#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "ca_cert.h"

#define PIN 4

#define GITHUB_USERNAME "domnantas"

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                                   NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

const uint16_t color_level_0 = matrix.Color(0, 0, 0);
const uint16_t color_level_1 = matrix.Color(0, 5, 0);
const uint16_t color_level_2 = matrix.Color(0, 20, 0);
const uint16_t color_level_3 = matrix.Color(0, 100, 0);
const uint16_t color_level_4 = matrix.Color(0, 160, 0);

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.print(" Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  matrix.begin();
  matrix.setBrightness(255);
  initWiFi();
  delay(2000);
  HTTPClient http;
  http.useHTTP10(true);
  http.begin("https://api.github.com/graphql", ca_cert);
  http.addHeader("Authorization", "bearer " GITHUB_TOKEN);
  Serial.print("Sending request... ");
  String query = "{\"query\" : \"query($login:String!,$from:DateTime!,$to:DateTime!){user(login:$login){contributionsCollection(from:$from,to:$to){contributionCalendar{totalContributions weeks{contributionDays{contributionLevel date}}}}}}\", \"variables\": { \"login\": \"domnantas\", \"from\": \"2022-10-15T00:00:00\", \"to\": \"2022-12-10T00:00:00\"}}";
  int httpCode = http.POST(query);

  if (httpCode != 200)
  {
    Serial.println("Error on HTTP request");
    http.end();
    return;
  }

  Serial.print(httpCode);
  Serial.println(" OK");

  DynamicJsonDocument doc(8192);

  DeserializationError err = deserializeJson(doc, http.getStream());

  if (err)
  {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.f_str());
  }

  JsonObject contributionCalendar = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"];
  int totalContributions = contributionCalendar["totalContributions"];
  JsonArray weeks = contributionCalendar["weeks"];

  Serial.println(totalContributions);
  http.end();
}

void loop()
{
  matrix.fillScreen(0);
  matrix.drawPixel(0, 0, color_level_1);
  matrix.drawPixel(2, 0, color_level_2);
  matrix.drawPixel(4, 0, color_level_3);
  matrix.drawPixel(6, 0, color_level_4);

  matrix.show();
  delay(100);
}
