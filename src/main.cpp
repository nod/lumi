#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define FASTLED_ESP8266_D1_PIN_ORDER
#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];


const char* ssid = "YOUR NETWORK NAME HERE";
const char* password = "SOME SECRET STUFF HERE";


const int led = 13;

bool lights_should_pulse = false;
bool lights_should_christmas = false;
bool lights_should_cylon = false;

ESP8266WebServer server(80);


void fadeall() {
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); }
}


void loop_lights_to_rgb(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
    FastLED.show();
    delay(10);
  }
}


void pulse_cylon() {
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}

	// Now go in the other direction.  
	for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}


void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "light controller");
  digitalWrite(led, 0);
}


void handleZane(void) {
  lights_should_pulse = false;
  lights_should_cylon = false;
  lights_should_christmas = false;

  // yellow
  loop_lights_to_rgb(250, 189, 47);

  // red
  loop_lights_to_rgb(250, 10, 10);

  // blue
  loop_lights_to_rgb(50, 80, 153);

  server.send(200, "text/plain", "ok");
}


void handleRGB(void) {
  lights_should_pulse = false;
  lights_should_cylon = false;
  lights_should_christmas = false;

  if (server.arg("r")=="" || server.arg("g")=="" || server.arg("b")=="") {
    String message = "missing args r,g,b";
    server.send( 501, "text/plain", message);
    return;
  }
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();
  loop_lights_to_rgb(r,g,b);
  server.send(200, "text/plain", "ok");
}


void handleRainbow(void) {
  lights_should_pulse = false;
  lights_should_christmas = false;
  lights_should_cylon = false;

  int TEMPERATURE_1 = Tungsten100W;
  int TEMPERATURE_2 = OvercastSky;
  int DISPLAYTIME = 20;

  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);


  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
    leds[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
    leds[0] = TEMPERATURE_2; // show indicator pixel
  }
  FastLED.show();
  server.send(200,"text/plain", "ok");
}


void handleBright(void) {
  if (server.arg("b")=="") {
    String message = "missing args b";
    server.send( 501, "text/plain", message);
    return;
  }
  int b = server.arg("b").toInt();
  FastLED.setBrightness(b);
  FastLED.show();
  server.send(200,"text/plain", "ok");
}

void handleCylon(void) {
  lights_should_pulse = false;
  lights_should_christmas = false;
  lights_should_cylon = ! lights_should_cylon;
  server.send( 200, "text/plain", "ok");
}

void handlePulse(void) {
  lights_should_pulse = ! lights_should_pulse;
  lights_should_cylon = false;
  lights_should_christmas = false;
  server.send( 200, "text/plain", "ok");
}

void handleChristmas(void) {
  lights_should_pulse = false;
  lights_should_cylon = false;
  lights_should_christmas = ! lights_should_christmas;
  server.send( 200, "text/plain", "ok");
}


void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


void setup_httpd(void) {
  server.on("/", handleRoot);
  server.on("/rgb", handleRGB);
  server.on("/pulse", handlePulse);
  server.on("/cylon", handleCylon);
  server.on("/christmas", handleChristmas);
  server.on("/rainbow", handleRainbow);
  server.on("/zane", handleZane);
  server.on("/bright", handleBright);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}


void setup_wifi(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
}


void setup_lights(void) {
  Serial.println("starting lights");
  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(32); //32);
}


void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  setup_wifi();
  setup_httpd();
  setup_lights();

  delay(40);
  loop_lights_to_rgb(20,20,20);
}


void pulse_christmas(void) {
  loop_lights_to_rgb(220,0,0);
  loop_lights_to_rgb(30,220,30);
}


void pulse_lights(void) {
  loop_lights_to_rgb(20,60,160);
  loop_lights_to_rgb(24,100,50);
}


void loop(void){
  server.handleClient();
  if (lights_should_pulse) pulse_lights();
  if (lights_should_christmas) pulse_christmas();
  if (lights_should_cylon) pulse_cylon();
}

