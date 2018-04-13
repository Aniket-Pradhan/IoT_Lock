#include <ESP8266WebServer.h>

const String loginPage = "<!DOCTYPE html><html><head><title>Login</title></head><body> <div id=\"login\"> <form action='/login' method='POST'> <center> <h1>Login </h1><p><input type='text' name='user' placeholder='User name'></p><p><input type='password' name='pass' placeholder='Password'></p><br><button type='submit' name='submit'>login</button></center> </form></body></html>";
//const String loginok = "<!DOCTYPE html><html><head><title>Login</title></head><body> <div> <form action='/' method='POST'> <center> <a href=\"/refresh\"><h4>Refresh</h4></a><a href=\"/logoff\"><h4>Logoff</h4></a><a href=\"/on\"><h4>Unlock</h4></a><a href=\"/off\"><h4>Lock</h4></a><a href=\"/Log\"><h4>Show Log</h4></a></center> </form></body></html>";
char loginok[10000];

const char* ssid     = "Pradhans";
const char* password = "SRkna11812";

String Log = "";

int LED = 13;

bool lock = false;

String anchars = "abcdefghijklmnopqrstuvwxyz0123456789", username = "admin", loginPassword = "admin";

unsigned long logincld = millis(), reqmillis = millis(), tempign = millis();

uint8_t i, trycount = 0;

ESP8266WebServer server(80);

String sessioncookie;

void setup(void) {
  snprintf ( loginok, 10000,
  "<!DOCTYPE html>\
<html>\
<head>\
    <title>Login</title>\
    <style>\
      body {\
        background: #FF4444;\
        font-family: calibri;\
      }\
    a:link, a:visited {\
      background-color: #FFF;\
      border: 2px solid #555555;\
      -webkit-transition-duration: 0.4s;\
      color: red;\
      cursor: pointer;\
      padding: 14px 25px;\
      text-align: center;\
      text-decoration: none;\
      transition-duration: 0.4s;\
      display: inline-block;\
    }\
    a:hover, a:active {\
      border: 2px solid #555555;\
      background-color: #555555;\
      color: #FFF;\
    }\
    </style>\
</head>\
<body>\
    <div>\
        <form action='/' method='POST'>\
            <center>\
             <a href= \"/refresh\">Refresh</a>\
             <a href= \"/logoff\">Logoff</a>\
             <a href= \"/on\"> ON </a>\
             <a href= \"/off\"> OFF </a>\
             <a href= \"/Log\"> Show Log </a>\
            </center>\
        </form>\
</body>\
</html>");  


  Serial.begin(115200);
  delay(10);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  gencookie();
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.on("/login", handleLogin);
  server.on("/refresh", refresh);
  server.on("/logoff", logoff);
  server.on("/on", switchOn);
  server.on("/off", switchOff);
  server.on("/Log", showLog);

  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );

  server.begin();
}


bool is_authentified() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie"), authk = "c=" + sessioncookie;
    if (cookie.indexOf(authk) != -1) return true;
  }
  return false;
}

void handleRoot() {

  String header;
  if (!is_authentified()) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  tempign = millis();
  server.send(200, "text/html", loginok);

}

void handleLogin() {

  String msg;
  msg = "<center><br>";

  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
  }

  if (server.hasArg("user") && server.hasArg("pass")) {
    if (server.arg("user") == username &&  server.arg("pass") == loginPassword && !lock) {
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: c=" + sessioncookie + "\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      trycount = 0;
      return;
    }
    if (trycount != 10 && !lock) {
      trycount++;
    }
    if (trycount < 10 && !lock) {
      msg += "Wrong username/password<p></p>";
      msg += "You have ";
      msg += (10 - trycount);
      msg += " tries before system temporarily locks out.";
      logincld = millis();
    }
    if (trycount == 10) { //If too much bad tries
      if (lock) {
        msg += "Too much invalid login requests, you can use this device in ";
        msg += 5 - ((millis() - logincld) / 60000);
        msg += " minutes.";
      }
      else {
        logincld = millis();
        lock = true;
        msg += "Too much invalid login requests, you can use this device in 5 minutes.";
      }
    }

  }
  String content = loginPage;
  content +=  msg + "</center>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void gencookie() {
  sessioncookie = "";
  for ( i = 0; i < 32; i++) sessioncookie += anchars[random(0, anchars.length())];
}

void loop(void) {
  server.handleClient();

  if (lock && abs(millis() - logincld) > 300000) {
    lock = false;
    trycount = 0;
    logincld = millis();
  }

  if (!lock && abs(millis() - logincld) > 60000) {
    trycount = 0;
    logincld = millis();
  }
  if (abs(millis() - tempign) > 30000) {
    switchOff();
    tempign = millis();
  }

  if (abs(millis() - tempign) > 120000) {
    switchOff();
    gencookie();
    tempign = millis();
  }
}

void logoff() {
  digitalWrite(LED, LOW);
  String header = "HTTP/1.1 301 OK\r\nSet-Cookie: c=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void switchOn() {
  Log = Log + "Unlocked by " + username+ "<br>";
  Serial.println("Unlocked by " + username);
  digitalWrite(LED, HIGH);
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void showLog() {
  String content = "<center><br>";
  content += Log;
  content += "</center>";
  String theWebPage = loginok;
  theWebPage += content;
  server.send(200, "text/html", theWebPage);
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void switchOff() {
  Log = Log + "Locked" + "<br>";
  Serial.println("Locked");
  digitalWrite(LED, LOW);
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void refresh() {
  if (is_authentified()) {
    tempign = millis();
    String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
  }
  else {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
  }
}