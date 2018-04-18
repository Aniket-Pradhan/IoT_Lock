#include <ESP8266WebServer.h>

const String loginPage = "<!DOCTYPE html><html><head><title>Login</title></head><body> <div id=\"login\"> <form action='/login' method='POST'> <center> <h1>Login </h1><p><input type='text' name='user' placeholder='User name'></p><p><input type='password' name='pass' placeholder='Password'></p><br><button type='submit' name='submit'>login</button></center> </form></body></html>";
//const String loginok = "<!DOCTYPE html><html><head><title>Login</title></head><body> <div> <form action='/' method='POST'> <center> <a href=\"/refresh\"><h4>Refresh</h4></a><a href=\"/logoff\"><h4>Logoff</h4></a><a href=\"/on\"><h4>Unlock</h4></a><a href=\"/off\"><h4>Lock</h4></a><a href=\"/Log\"><h4>Show Log</h4></a></center> </form></body></html>";
char loginok[10000];

const char* ssid     = "MI5";
const char* password = "qwertyui";

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
  * {\
    box-sizing: border-box;\
  }\
  body {\
    background: linear-gradient(-135deg, #c850c0, #4158d0);\
    font-family: calibri;\
    width: 100%;\
    min-height: 100vh;\
    display: -webkit-box;\
    display: -webkit-flex;\
    display: -moz-box;\
    display: -ms-flexbox;\
    display: flex;\
    flex-wrap: wrap;\
    justify-content: center;\
    align-items: center;\
    padding: 15px;\
    background: #9053c7;\
    background: -webkit-linear-gradient(-135deg, #c850c0, #4158d0);\
    background: -o-linear-gradient(-135deg, #c850c0, #4158d0);\
    background: -moz-linear-gradient(-135deg, #c850c0, #4158d0);\
    background: linear-gradient(-135deg, #c850c0, #4158d0);\
  }\
  a:link, a:visited {\
    border: 2px solid #4158d0;\
    background-color: #4158d0;\
    -webkit-transition-duration: 0.4s;\
    color: #FFF;\
    cursor: pointer;\
    padding: 14px 25px;\
    text-align: center;\
    text-decoration: none;\
    transition-duration: 0.4s;\
    display: inline-block;\
    width: 200px;\
  }\
  a:hover, a:active {\
    border: 2px solid #c850c0;\
    background-color: #c850c0;\
    color: #FFF;\
  }\
  .login-form {\
    width: 400px;\
    margin: 50px auto;\
  }\
  .login-form h2 {\
    margin: 0 0 15px;\
  }\
  .form-control, .btn {\
    min-height: 38px;\
    border-radius: 2px;\
  }\
  .btn {\
    font-size: 15px;\
    font-weight: bold;\
  }\
  .boxxing {\
    margin-bottom: 15px;\
    background: #f7f7f7;\
    box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.3);\
    padding: 30px;\
  }\
  @media screen and (max-width: 600px) {\
    .col-25, .col-75, input {\
      width: 100%;\
      margin-top: 0;\
    }\
  }\
</style>\
</head>\
<body>\
  <center>\
    <div class=\"login-form\">\
      <div class=\"boxxing\">\
        <h2>SLOCK</h2>\
        <form action='/' method='POST'>\
           <a href= \"/refresh\">Refresh</a>\
           <br>\
           <a href= \"/logoff\">Logoff</a>\
           <br>\
           <a href= \"/on\"> UNLOCK </a>\
           <br>\
           <a href= \"/off\"> LOCK </a>\
           <br>\
           <a href= \"/Log\"> Show Log </a>\
           <br>\
           <a href= \"/resetLog\">Reset Log</a>\
           <br>\
       </form>\
     </div>\
   </div>\
  </center>\
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
  server.on("/resetLog", resetLog);

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
    switchOffe();
    tempign = millis();
  }

  if (abs(millis() - tempign) > 120000) {
    switchOffe();
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
  content += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
  content += "</center>";
  String theWebPage = loginok;
  theWebPage += content;
  server.send(200, "text/html", theWebPage);
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void resetLog() {
  Log = "";
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void switchOffe() {
  digitalWrite(LED, LOW);
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
