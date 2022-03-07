
#include "Arduino.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>n64 Controller Debug</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>n64 Controller Debug</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#059e8a;"></i> BUTTON</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-gamepad" style="color:#00add6;"></i> x</p><p><span class="reading"><span id="hum">%XAXIS%</span></span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-gamepad" style="color:#e1e437;"></i> y</p><p><span class="reading"><span id="pres">%YAXIS%</span></span></p>
      </div>
            <div class="card">
        <p><i class="fas fa-gamepad" style="color:#e1e437;"></i> y</p><p><span class="reading"><span id="TX_MODE">%YAXIS%</span></span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('XAXIS', function(e) {
  console.log("XAXIS", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('YAXIS', function(e) {
  console.log("YAXIS", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);

  source.addEventListener('TX_MODE', function(e) {
  console.log("TX_MODE", e.data);
  document.getElementById("ex_mode").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";