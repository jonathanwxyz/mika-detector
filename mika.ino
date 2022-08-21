#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SHOCK_SENSOR_PIN 22

#define MINUTES_TO_DAYS (60 * 24)

#include <WiFi.h>
#include <WebServer.h>

//need to define this yourself
#include <WiFiAuth.h>
/*
const char* ssid = "[SSID]";  // Enter SSID here
const char* password = "[PASSWORD]";  //Enter Password here
*/

WebServer server(80);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool shock_sensor_state = 0;
int count = 0;
 
void setup() {
  Serial.begin(115200);
  
  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);

  setDisplay(0);

  pinMode(SHOCK_SENSOR_PIN, INPUT);
  shock_sensor_state = digitalRead(SHOCK_SENSOR_PIN);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  
  server.handleClient();

  if (minutes() % MINUTES_TO_DAYS == 0) {
    count = 0;
    setDisplay(0);
  }

  if (shock_sensor_state == 1) {
    ++count;
    setDisplay(count);
    shock_sensor_state = 0;
    delay(3000);
  } else {
    shock_sensor_state = digitalRead(SHOCK_SENSOR_PIN);    
  }
}

void setDisplay(unsigned long num) {
    display.clearDisplay();
    display.setTextSize(8);
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.print(num);
    display.display();
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>MikaDetector5000</h1>\n";
  ptr +="<h3>Mika has come or gone ";
  ptr += String(count) + " times today</h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

unsigned long minutes() {
  return millis() / (1000 * 60);
}
