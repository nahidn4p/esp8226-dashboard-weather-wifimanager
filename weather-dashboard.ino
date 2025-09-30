#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define LED_PIN 2
ESP8266WebServer server(80);

bool ledState = false;
float temp = 0, humidity = 0, feels_like = 0;
String weatherDesc = "Unknown";
String weatherIcon = "";
float windSpeed = 0;
int windDeg = 0, clouds = 0;
String placeName = "Unknown";

const char* serverName = "http://api.openweathermap.org/data/2.5/weather";
const char* apiKey = "OPENWEATHER-API-KEY";
const float latitude = 00.00; //YOUR LOCATION LATITUDE
const float longitude = 00.00; //YOUR LOCATION LONGITUDE
unsigned long lastWeatherUpdate = 0;

// ----- Fetch weather data -----
void fetchWeather() {
  if(WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  WiFiClient client;
  String url = String(serverName) + "?lat=" + String(latitude,4) + "&lon=" + String(longitude,4) +
               "&appid=" + apiKey + "&units=metric";
  http.begin(client, url);
  int httpCode = http.GET();
  if(httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    if(deserializeJson(doc, payload) == DeserializationError::Ok){
      temp = doc["main"]["temp"];
      feels_like = doc["main"]["feels_like"];
      humidity = doc["main"]["humidity"];
      weatherDesc = doc["weather"][0]["description"].as<String>();
      weatherIcon = doc["weather"][0]["icon"].as<String>();
      windSpeed = doc["wind"]["speed"];
      windDeg = doc["wind"]["deg"];
      clouds = doc["clouds"]["all"];
      placeName = doc["name"].as<String>();
    }
  }
  http.end();
}

// Map temperature & humidity to color intensity
int tempToRed(float t){ return constrain(map((int)t,0,40,50,200),50,200); }
int humToBlue(float h){ return constrain(map((int)h,0,100,50,200),50,200); }

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,HIGH);

  WiFiManager wm;
  wm.autoConnect("ESP8266_AP");

  Serial.println("✅ Connected! IP: " + WiFi.localIP().toString());

  server.on("/", []() {
    fetchWeather(); // fetch first to get accurate initial colors

    int tempRed = tempToRed(temp);
    int humBlue = humToBlue(humidity);

    String mac = WiFi.macAddress();
    String macPartial = mac.substring(0,5) + ":**:**:**:**";

    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<title>ESP8266 Weather Dashboard</title>";
    html += "<style>";
    html += "body{font-family:Arial;text-align:center;}";
    html += ".box{display:inline-block;width:200px;height:auto;margin:10px;padding:15px;border-radius:10px;color:#fff;font-size:18px;font-weight:bold;transition:background 1s;word-wrap:break-word;line-height:1.3;}";
    html += ".led-btn{padding:10px 20px;margin:5px;}";
    html += ".mac-btn{padding:5px 10px;margin-left:10px;}";
    html += "</style></head><body>";

    html += "<h1>ESP8266 Weather Dashboard</h1>";
    html += "<p><b>Place:</b> " + placeName + "</p>";
    html += "<p><b>Wi-Fi SSID:</b> " + WiFi.SSID() + "</p>";
    html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";

    // MAC toggle
    html += "<p><b>MAC:</b> <span id='macAddr'>" + macPartial + "</span>";
    html += "<button class='mac-btn' id='macBtn' onclick=\"toggleMac()\">Show</button></p>";

    // Uptime placeholder
    html += "<p><b>Uptime:</b> <span id='uptime'>0 s</span></p><hr>";

    // Temperature card with Feels like
    html += "<div class='box' id='tempBox' style='background:rgb("+String(tempRed)+",70,70)'>";
    html += "Temp: <span id='tempValue'>"+String(temp)+" °C</span><br>";
    html += "Feels like: <span id='feelsValue'>"+String(feels_like)+" °C</span></div>";

    // Humidity card with Wind
    html += "<div class='box' id='humBox' style='background:rgb(70,70,"+String(humBlue)+")'>";
    html += "Humidity: <span id='humValue'>"+String(humidity)+" %</span><br>";
    html += "Wind: <span id='windValue'>"+String(windSpeed)+" m/s, "+String(windDeg)+"°</span></div>";

    if(weatherIcon.length()>0) html += "<p><img src='http://openweathermap.org/img/wn/"+weatherIcon+"@2x.png'></p>";
    html += "<p>Weather: <span id='descValue'>"+weatherDesc+"</span></p>";
    html += "<p>Cloudiness: <span id='cloudValue'>"+String(clouds)+" %</span></p>";

    html += "<hr><p>LED: <span id='ledState'>"+String(ledState?"ON":"OFF")+"</span></p>";
    html += "<button class='led-btn' onclick=\"toggleLED(true)\">ON</button>";
    html += "<button class='led-btn' onclick=\"toggleLED(false)\">OFF</button>";

    // ----- JavaScript for live updates -----
    html += "<script>";
    html += "let macShown=false;";
    html += "function toggleMac(){";
    html += "if(!macShown){document.getElementById('macAddr').innerText='" + mac + "'; document.getElementById('macBtn').innerText='Hide'; macShown=true;}";
    html += "else{document.getElementById('macAddr').innerText='" + macPartial + "'; document.getElementById('macBtn').innerText='Show'; macShown=false;}";
    html += "}";

    // Live update function
    html += "async function updateData(){";
    html += "const resp=await fetch('/data'); const d=await resp.json();";
    html += "document.getElementById('tempValue').innerText=d.temp.toFixed(1)+' °C';";
    html += "document.getElementById('feelsValue').innerText=d.feels.toFixed(1)+' °C';";
    html += "document.getElementById('humValue').innerText=d.humidity.toFixed(1)+' %';";
    html += "document.getElementById('windValue').innerText=d.windspd+' m/s, '+d.winddeg+'°';";
    html += "document.getElementById('descValue').innerText=d.desc;";
    html += "document.getElementById('cloudValue').innerText=d.clouds+' %';";
    html += "document.getElementById('ledState').innerText=d.led?'ON':'OFF';";
    html += "document.getElementById('tempBox').style.background='rgb('+Math.floor("+String(tempRed)+"+(d.temp-0)*3)+',70,70)';";
    html += "document.getElementById('humBox').style.background='rgb(70,70,'+Math.floor("+String(humBlue)+"+(d.humidity-0)*1.5)+')';";
    html += "}setInterval(updateData,10000);";

    // LED control
    html += "function toggleLED(state){fetch(state?'/led/on':'/led/off');}";

    // Uptime clock animation
    html += "let uptimeSeconds = " + String(millis()/1000) + ";"; // initialize from ESP millis
    html += "setInterval(()=>{";
    html += "  uptimeSeconds++;"; // increment every second
    html += "  let h = Math.floor(uptimeSeconds/3600);";
    html += "  let m = Math.floor((uptimeSeconds%3600)/60);";
    html += "  let s = uptimeSeconds%60;";
    html += "  document.getElementById('uptime').innerText = h+'h '+m+'m '+s+'s';";
    html += "},1000);";


    html += "</script>";

    html += "</body></html>";
    server.send(200,"text/html",html);
  });

  // JSON API
  server.on("/data", [](){
    String json = "{\"temp\":"+String(temp)+",\"humidity\":"+String(humidity)+",\"feels\":"+String(feels_like)+
                  ",\"desc\":\""+weatherDesc+"\",\"icon\":\""+weatherIcon+"\",\"windspd\":"+String(windSpeed)+
                  ",\"winddeg\":"+String(windDeg)+",\"clouds\":"+String(clouds)+",\"led\":"+(ledState?"true":"false")+"}";
    server.send(200,"application/json",json);
  });

  // LED control
  server.on("/led/on", [](){ ledState=true; digitalWrite(LED_PIN,LOW); server.send(200,"text/plain","LED ON"); });
  server.on("/led/off", [](){ ledState=false; digitalWrite(LED_PIN,HIGH); server.send(200,"text/plain","LED OFF"); });

  server.begin();
  fetchWeather(); // fetch first for initial display
}

void loop() {
  server.handleClient();
  unsigned long now = millis();
  if(now - lastWeatherUpdate > 600000){ // 10 min
    lastWeatherUpdate = now;
    fetchWeather();
  }

  // Blinking LED
  static unsigned long lastBlink=0;
  static bool blinkState=false;
  if(ledState && now - lastBlink > 500){
    lastBlink=now;
    blinkState=!blinkState;
    digitalWrite(LED_PIN, blinkState?LOW:HIGH);
  }
}
