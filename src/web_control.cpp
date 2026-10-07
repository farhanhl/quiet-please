#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "web_control.h"

static const char INDEX_HTML[] PROGMEM = R"RAW(
<!DOCTYPE html>
<html><head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta charset="utf-8">
<title>quiet-please</title>
<style>
*{box-sizing:border-box}
body{background:#111;color:#eee;font-family:monospace;margin:0;padding:12px;max-width:480px;margin:0 auto}
h1{font-size:1.3em;text-align:center;margin:8px 0}
.lbl{color:#888;font-size:.8em;margin:10px 0 4px;display:block}
.dot{width:12px;height:12px;border-radius:50%;display:inline-block;vertical-align:middle}
.green{background:#0f0}.red{background:#f33}
.btn{display:block;width:100%;padding:13px;margin:3px 0;font-size:1em;border:none;border-radius:6px;background:#2a2a2a;color:#eee;cursor:pointer;transition:background .15s}
.btn:active{background:#444}
.btn.sel{background:#0a5;border-color:#0f5}
.btn.start{background:#063;font-size:1.2em;font-weight:bold;margin-top:8px}
.btn.start:active{background:#085}
.btn.stop{background:#600;font-size:1.2em;font-weight:bold}
.btn.scan{background:#234}
.btn.scan:active{background:#346}
.g2{display:grid;grid-template-columns:1fr 1fr;gap:3px}
.g4{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px}
input[type=range]{width:100%;accent-color:#0a5;margin:4px 0}
input[type=number]{width:80px;background:#222;color:#eee;border:1px solid #444;border-radius:4px;padding:4px}
.stats{background:#1a1a1a;padding:10px;border-radius:6px;margin-top:10px;font-size:.85em;line-height:1.6}
#freq{color:#0f0}
.scanbar{height:6px;border-radius:2px;margin:1px 0}
.scanrow{display:flex;align-items:center;gap:4px;font-size:.7em}
.scanlbl{width:50px;color:#888}
.scanbarwrap{flex:1;background:#1a1a1a;border-radius:2px}
</style>
</head><body>
<h1><span id="dot" class="dot red"></span> quiet-please</h1>
<span class="lbl">MODE</span>
<div class="g2">
<button class="btn" id="m1" onclick="api('mode?m=1')">BLE ADV</button>
<button class="btn" id="m2" onclick="api('mode?m=2')">BLE FULL</button>
<button class="btn" id="m3" onclick="api('mode?m=3')">BT CLASSIC</button>
<button class="btn" id="m4" onclick="api('mode?m=4')">CW SINGLE</button>
</div>
<span class="lbl">CW CHANNEL — <span id="freq">2440 MHz</span></span>
<input type="range" min="0" max="125" value="40" id="ch" onchange="api('channel?c='+this.value)" oninput="document.getElementById('freq').innerText=(2400+ +this.value)+' MHz'">
<span class="lbl">POWER</span>
<div class="g4">
<button class="btn" id="p0" onclick="api('power?p=0')"><small>-18</small></button>
<button class="btn" id="p1" onclick="api('power?p=1')"><small>-12</small></button>
<button class="btn" id="p2" onclick="api('power?p=2')"><small>-6</small></button>
<button class="btn" id="p3" onclick="api('power?p=3')"><small>0dBm</small></button>
</div>
<span class="lbl">AUTO-STOP TIMER (seconds, 0=off)</span>
<input type="number" id="timer" min="0" max="3600" value="300" onchange="api('timer?t='+this.value*1000)">
<button class="btn start" onclick="api('start')">START</button>
<button class="btn stop" onclick="api('stop')">STOP</button>
<button class="btn scan" onclick="api('scan')">SCAN CHANNELS</button>
<div class="stats" id="st">connecting...</div>
<div id="scanout"></div>
<script>
function api(q){fetch('/api/'+q).then(r=>r.json()).then(upd,errer)}
function errer(e){document.getElementById('st').innerText='error'}
var MN=['IDLE','BLE_ADV','BLE_FULL','BT_CLASSIC','CW','SCAN'];
function upd(s){
 var d=document.getElementById('dot');
 d.className='dot '+(s.active?'green':'red');
 for(var i=1;i<=4;i++){document.getElementById('m'+i).className='btn'+(s.mode==i?' sel':'')}
 for(var i=0;i<=3;i++){document.getElementById('p'+i).className='btn'+(s.pa_level==i?' sel':'')}
 document.getElementById('ch').value=s.cw_channel;
 document.getElementById('freq').innerText=(2400+s.cw_channel)+' MHz';
 document.getElementById('timer').value=Math.round(s.auto_stop_ms/1000);
 var rem=s.active&&s.remaining_ms>0?' ('+Math.ceil(s.remaining_ms/1000)+'s left)':'';
 document.getElementById('st').innerHTML=
  '<b>Mode</b>: '+MN[s.mode]+'<br>'+
  '<b>Active</b>: '+(s.active?'<span style="color:#0f0">YES</span>':'no')+rem+'<br>'+
  '<b>CW Ch</b>: '+s.cw_channel+' ('+(2400+s.cw_channel)+' MHz)<br>'+
  '<b>Power</b>: '+s.pa_level+'<br>'+
  '<b>M1</b>: '+(s.m1_ok?'OK':'MISSING')+' | <b>M2</b>: '+(s.m2_ok?'OK':'MISSING')+'<br>'+
  '<b>Sweeps</b>: '+s.sweeps;
}
function loadScan(){
 fetch('/api/scanresults').then(r=>r.json()).then(function(d){
  if(!d||!d.length){document.getElementById('scanout').innerHTML='<span style="color:#888">no scan data</span>';return}
  var h='<span class="lbl">SCAN RESULTS (signal > -64dBm)</span>';
  for(var i=0;i<d.length;i++){
   var det=d[i].m1||d[i].m2;
   var bg=det?'#0a5':'#333';
   h+='<div class="scanrow"><span class="scanlbl">'+(2400+d[i].ch)+'</span><div class="scanbarwrap"><div class="scanbar" style="background:'+bg+';width:'+(det?100:5)+'%"></div></div>'+(det?'*':'')+'</div>';
  }
  document.getElementById('scanout').innerHTML=h;
 },function(){document.getElementById('scanout').innerHTML='<span style="color:#f33">scan failed</span>'});
}
setInterval(function(){fetch('/api/status').then(r=>r.json()).then(upd,errer)},1000);
fetch('/api/status').then(r=>r.json()).then(upd,errer);
</script>
</body></html>
)RAW";

static WebServer server(WEB_PORT);

WebControl::WebControl(Jammer& j) : jammer_(j), ap_ok_(false) {}

bool WebControl::check_auth() {
    if (!server.authenticate(WEB_USER, WEB_PASS)) {
        server.requestAuthentication();
        return false;
    }
    return true;
}

static void send_status_json(Jammer& jammer_) {
    JammerStatus s = jammer_.status();
    char json[320];
    snprintf(json, sizeof(json),
        "{\"mode\":%d,\"active\":%s,\"cw_channel\":%d,\"pa_level\":%d,\"m1_ok\":%s,\"m2_ok\":%s,\"sweeps\":%lu,\"auto_stop_ms\":%lu,\"elapsed_ms\":%lu,\"remaining_ms\":%lu}",
        (int)s.mode, s.active ? "true" : "false",
        s.cw_channel, s.pa_level,
        s.m1_ok ? "true" : "false", s.m2_ok ? "true" : "false",
        (unsigned long)s.sweeps,
        (unsigned long)s.auto_stop_ms,
        (unsigned long)s.elapsed_ms,
        (unsigned long)s.remaining_ms);
    server.send(200, "application/json", json);
}

void WebControl::handle_root()          { if (check_auth()) server.send_P(200, "text/html", INDEX_HTML); }
void WebControl::handle_status()        { if (check_auth()) send_status_json(jammer_); }
void WebControl::handle_mode()          { if (check_auth()) { if (server.hasArg("m")) jammer_.set_mode((JammerMode)server.arg("m").toInt()); send_status_json(jammer_); } }
void WebControl::handle_channel()       { if (check_auth()) { if (server.hasArg("c")) jammer_.set_channel((uint8_t)server.arg("c").toInt()); send_status_json(jammer_); } }
void WebControl::handle_power()         { if (check_auth()) { if (server.hasArg("p")) jammer_.set_power((uint8_t)server.arg("p").toInt()); send_status_json(jammer_); } }
void WebControl::handle_start()         { if (check_auth()) { jammer_.start(); send_status_json(jammer_); } }
void WebControl::handle_stop()          { if (check_auth()) { jammer_.stop();  send_status_json(jammer_); } }
void WebControl::handle_timer()         { if (check_auth()) { if (server.hasArg("t")) jammer_.set_auto_stop((uint32_t)server.arg("t").toInt()); send_status_json(jammer_); } }
void WebControl::handle_save()          { if (check_auth()) { jammer_.save_settings(); server.send(200, "text/plain", "saved"); } }
void WebControl::handle_not_found()     { server.send(404, "text/plain", "404"); }

void WebControl::handle_scan() {
    if (!check_auth()) return;
    jammer_.set_mode(JammerMode::SCAN);
    jammer_.run_scan();
    send_status_json(jammer_);
}

void WebControl::handle_scan_results() {
    if (!check_auth()) return;
    if (!jammer_.scan_available()) {
        server.send(200, "application/json", "[]");
        return;
    }
    const ScanResult* r = jammer_.scan_results();
    uint8_t n = jammer_.scan_count();
    String json = "[";
    for (uint8_t i = 0; i < n; i++) {
        if (i) json += ",";
        json += "{\"ch\":";
        json += r[i].channel;
        json += ",\"m1\":";
        json += r[i].m1_detected ? "true" : "false";
        json += ",\"m2\":";
        json += r[i].m2_detected ? "true" : "false";
        json += "}";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void WebControl::init_ap() {
    WiFi.mode(WIFI_AP);
    ap_ok_ = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS, WIFI_CHANNEL);
}

void WebControl::init_routes() {
    server.on("/",                HTTP_GET, [this]{ handle_root(); });
    server.on("/api/status",      HTTP_GET, [this]{ handle_status(); });
    server.on("/api/mode",        HTTP_GET, [this]{ handle_mode(); });
    server.on("/api/channel",     HTTP_GET, [this]{ handle_channel(); });
    server.on("/api/power",       HTTP_GET, [this]{ handle_power(); });
    server.on("/api/start",       HTTP_GET, [this]{ handle_start(); });
    server.on("/api/stop",        HTTP_GET, [this]{ handle_stop(); });
    server.on("/api/scan",        HTTP_GET, [this]{ handle_scan(); });
    server.on("/api/scanresults", HTTP_GET, [this]{ handle_scan_results(); });
    server.on("/api/timer",       HTTP_GET, [this]{ handle_timer(); });
    server.on("/api/save",        HTTP_GET, [this]{ handle_save(); });
    server.onNotFound([this]{ handle_not_found(); });
    server.begin();
}

void WebControl::begin() {
    init_ap();
    init_routes();
    Serial.print("AP SSID : "); Serial.println(WIFI_AP_SSID);
    Serial.print("AP IP   : "); Serial.println(WiFi.softAPIP());
    Serial.print("Web UI  : http://"); Serial.println(WiFi.softAPIP());
    Serial.printf("Auth    : %s / %s\n", WEB_USER, WEB_PASS);
}

void WebControl::loop() {
    server.handleClient();
}
