/*
  WiFi Web Server LED Blink - styled dashboard

  Connect to the WiFi network, open the Serial monitor to see the IP address,
  then browse to that address to control the LED on pin 2.

  http://yourAddress/H        turn the LED on
  http://yourAddress/L        turn the LED off
  http://yourAddress/toggle   toggle the LED
*/

#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "";
const char *password = "";

const int LED_PIN = 2;

NetworkServer server(80);
bool ledOn = false;

void sendPage(NetworkClient &client, bool on) {
  client.print(F(R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>LED Control</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{--accent:#7c5cff;--accent2:#22d3ee;--text:#eef1ff;--muted:#9aa3c7;--card:rgba(255,255,255,.055);--line:rgba(255,255,255,.1)}
body.on{--accent:#34d399;--accent2:#a3e635}
html,body{height:100%}
body{font-family:ui-sans-serif,system-ui,-apple-system,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;color:var(--text);background:radial-gradient(900px 600px at 15% 10%,rgba(124,92,255,.22),transparent 60%),radial-gradient(700px 500px at 85% 90%,rgba(34,211,238,.16),transparent 60%),#080b16;display:grid;place-items:center;padding:24px;min-height:100vh;transition:background .4s}
.card{width:min(100%,420px);background:var(--card);border:1px solid var(--line);border-radius:28px;padding:34px 30px 28px;backdrop-filter:blur(18px);-webkit-backdrop-filter:blur(18px);box-shadow:0 30px 80px -30px rgba(0,0,0,.8),inset 0 1px 0 rgba(255,255,255,.08);text-align:center}
.eyebrow{display:inline-flex;align-items:center;gap:8px;font-size:11px;letter-spacing:.16em;text-transform:uppercase;color:var(--muted);margin-bottom:14px}
.eyebrow i{width:7px;height:7px;border-radius:50%;background:var(--accent);box-shadow:0 0 12px var(--accent);animation:pulse 2s infinite}
@keyframes pulse{50%{opacity:.35;transform:scale(.8)}}
h1{font-size:30px;font-weight:700;letter-spacing:-.03em;background:linear-gradient(100deg,var(--accent2),var(--accent));-webkit-background-clip:text;background-clip:text;color:transparent;margin-bottom:8px}
.sub{font-size:14px;color:var(--muted);margin-bottom:26px}
.pill{display:inline-flex;align-items:center;gap:9px;padding:7px 15px;border-radius:999px;border:1px solid var(--line);background:rgba(255,255,255,.04);font-size:12px;font-weight:600;letter-spacing:.12em;margin-bottom:24px}
.dot{width:8px;height:8px;border-radius:50%;background:var(--accent);box-shadow:0 0 10px var(--accent)}
.btn{width:132px;height:132px;border-radius:50%;border:1px solid rgba(255,255,255,.18);cursor:pointer;color:#fff;background:linear-gradient(145deg,var(--accent),var(--accent2));box-shadow:0 18px 40px -12px var(--accent),inset 0 1px 0 rgba(255,255,255,.45);display:grid;place-items:center;margin:0 auto 18px;transition:transform .18s,box-shadow .3s,filter .3s}
.btn:hover{transform:translateY(-3px) scale(1.03);filter:brightness(1.08)}
.btn:active{transform:scale(.96)}
.btn svg{width:46px;height:46px}
.hint{font-size:13px;color:var(--muted);min-height:18px}
.divider{height:1px;background:var(--line);margin:26px 0 20px}
.meta{display:flex;justify-content:space-between;gap:12px;font-size:12px;color:var(--muted);text-align:left}
.meta b{display:block;color:var(--text);font-weight:600;font-size:13px;margin-top:3px;word-break:break-all}
.meta div{flex:1}
.meta div+div{text-align:right}
footer{margin-top:22px;font-size:11px;color:rgba(154,163,199,.6)}
@media(max-width:380px){.card{padding:28px 20px 24px}h1{font-size:26px}.btn{width:116px;height:116px}}
</style>
</head>
<body class=")HTML"));

  client.print(on ? F("on") : F("off"));

  client.print(F(R"HTML(">
<main class="card">
<div class="eyebrow"><i></i>ESP32 · Web Server</div>
<h1>LED Control</h1>
<p class="sub">A tiny dashboard for GPIO 2</p>
<div class="pill"><span class="dot"></span><span id="stateTxt">
)HTML"));

  client.print(on ? F("ON") : F("OFF"));

  client.print(F(R"HTML(</span></div>
<button class="btn" id="btn" aria-label="Toggle LED" aria-pressed="
)HTML"));

  client.print(on ? F("true") : F("false"));

  client.print(F(R"HTML(" onclick="toggleLed()">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M12 2.8v8.4"/><path d="M6.4 6.4a8 8 0 1 0 11.2 0"/></svg>
</button>
<p class="hint" id="hint">
)HTML"));

  client.print(on ? F("Light is on - tap to switch off") : F("Light is off - tap to switch on"));

  client.print(F(R"HTML(</p>
<div class="divider"></div>
<div class="meta">
<div>GPIO<b>Pin 2 · <span id="level">
)HTML"));

  client.print(on ? F("HIGH") : F("LOW"));

  client.print(F(R"HTML(</span></b></div>
<div>IP ADDRESS<b>
)HTML"));

  client.print(WiFi.localIP().toString());

  client.print(F(R"HTML(</b></div>
</div>
<footer>Served fresh from your microcontroller</footer>
</main>
<script>
function toggleLed(){
var on=!document.body.classList.contains('on');
fetch(on?'/H':'/L').catch(function(){});
document.body.classList.toggle('on',on);
document.getElementById('stateTxt').textContent=on?'ON':'OFF';
document.getElementById('level').textContent=on?'HIGH':'LOW';
document.getElementById('hint').textContent=on?'Light is on - tap to switch off':'Light is off - tap to switch on';
document.getElementById('btn').setAttribute('aria-pressed',on);
}
</script>
</body>
</html>
)HTML"));
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  delay(10);

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
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (!client) return;

  // Read the request line and headers (with a short safety timeout)
  String request = "";
  String line = "";
  bool firstLine = true;
  unsigned long start = millis();

  while (client.connected() && millis() - start < 2000) {
    if (!client.available()) continue;
    char c = client.read();
    if (c == '\n') {
      if (firstLine) {
        request = line;
        firstLine = false;
      }
      if (line.length() == 0) break;  // blank line = end of headers
      line = "";
    } else if (c != '\r') {
      line += c;
    }
  }

  // Pull the path out of e.g. "GET /H HTTP/1.1"
  String path = "/";
  int sp1 = request.indexOf(' ');
  int sp2 = (sp1 >= 0) ? request.indexOf(' ', sp1 + 1) : -1;
  if (sp1 >= 0 && sp2 > sp1) path = request.substring(sp1 + 1, sp2);
  int q = path.indexOf('?');
  if (q >= 0) path = path.substring(0, q);

  if (path == "/H") {
    ledOn = true;
  } else if (path == "/L") {
    ledOn = false;
  } else if (path == "/toggle") {
    ledOn = !ledOn;
  }
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println("Cache-Control: no-store");
  client.println();

  sendPage(client, ledOn);

  client.stop();
}
