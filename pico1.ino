#include <WiFi.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
const int buttonPin = 16;
int dadJokeCount = 0;
bool buttonState = false;
bool lastButtonState = false;

const char* ssid = "Wokwi-GUEST";
const char* password = "";

WiFiServer server(80);

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Dad Joke Count:");
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    dadJokeCount++;
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(dadJokeCount);
    Serial.print("Dad Joke Count: ");
    Serial.println(dadJokeCount);
  }
  lastButtonState = buttonState;
  delay(50);

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        currentLine += c;
        if (c == '\n') {
          if (currentLine.length() == 1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<html><body>");
            client.print("<h1>Dad Joke Counter</h1>");
            client.print("<p>Dad Joke Count: ");
            client.print(dadJokeCount);
            client.print("</p>");
            client.print("<form action=\"/reset\" method=\"POST\"><button type=\"submit\">Reset Counter</button></form>");
            client.print("</body></html>");
            client.println();
            break;
          }
          currentLine = "";
        } else if (currentLine.endsWith("POST /reset")) {
          dadJokeCount = 0;
          lcd.setCursor(0, 1);
          lcd.print("Count: ");
          lcd.print(dadJokeCount);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
