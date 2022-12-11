#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "ca_cert.h"
#include "time.h"

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

String get_ISO8601_time(time_t timestamp)
{
  tm timeinfo;
  gmtime_r(&timestamp, &timeinfo);

  String str;

  char buffer[32];
  strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S", &timeinfo);
  str = buffer;

  return str;
}

void init_WiFi()
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

JsonArray get_github_contribution_weeks(String from, String to)
{
  HTTPClient http;
  http.useHTTP10(true);
  http.begin("https://api.github.com/graphql", ca_cert);
  http.addHeader("Authorization", "bearer " GITHUB_TOKEN);
  Serial.println("Sending request... ");

  String login = GITHUB_USERNAME;
  String query = "{\"query\" : \"query($login:String!,$from:DateTime!,$to:DateTime!){user(login:$login){contributionsCollection(from:$from,to:$to){contributionCalendar{totalContributions weeks{contributionDays{contributionLevel date}}}}}}\", \"variables\": { \"login\": \"" + login + "\", \"from\": \"" + from + "\", \"to\": \"" + to + "\"}}";

  Serial.println(query);

  int httpCode = http.POST(query);

  if (httpCode != 200)
  {
    Serial.println("Error on HTTP request");
    http.end();
    return JsonArray();
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

  http.end();

  JsonObject contributionCalendar = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"];
  JsonArray weeks = contributionCalendar["weeks"];

  return weeks;
}

void setup()
{
  Serial.begin(115200);
  matrix.begin();
  matrix.setBrightness(255);
  init_WiFi();

  const char *ntpServer = "pool.ntp.org";
  configTime(0, 0, ntpServer);
  delay(5000);

  time_t timestamp_now = time(nullptr);
  time_t timestamp_eight_weeks_ago = timestamp_now - 8 * 7 * 24 * 60 * 60;

  String today = get_ISO8601_time(timestamp_now);
  String eight_days_ago = get_ISO8601_time(timestamp_eight_weeks_ago);

  JsonArray weeks = get_github_contribution_weeks(eight_days_ago, today);

  for (JsonObject week : weeks)
  {
    JsonArray contributionDays = week["contributionDays"];

    for (JsonObject contributionDay : contributionDays)
    {
      String contributionLevel = contributionDay["contributionLevel"];
      String date = contributionDay["date"];
      Serial.println(contributionLevel);
    }
  }
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
