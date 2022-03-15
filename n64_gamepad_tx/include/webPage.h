
#include "Arduino.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>n64 Controller Debug</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <link
      rel="stylesheet"
      href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
      integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr"
      crossorigin="anonymous"
    />
    <link rel="icon" href="data:," />
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        text-align: normal;
      }
      p {
        font-size: 1rem;
      }
      body {
        margin: 10;
      }
      .reading {
        font-size: 0.5rem;
      }
      .topnav {
        overflow: hidden;
        background-color: #50b8b4;
        color: white;
        font-size: 1rem;
      }
      .content {
        padding: 20px;
      }
      .card {
        background-color: white;
        box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, 0.5);
      }
      .cards {
        max-width: 800px;
        margin: 0 auto;
        display: grid;
        grid-gap: 2rem;
        grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      }

      .axes {
        padding: 1em;
      }

      .buttons {
        margin-left: 1em;
      }

      /*meter*/
      .axis {
        min-width: 200px;
        margin: 1em;
      }

      .button {
        display: inline-block;
        width: 1em;
        text-align: center;
        padding: 1em;
        border-radius: 20px;
        border: 1px solid black;
        background-image: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAAAXNSR0IArs4c6QAAAAxJREFUCNdjYPjPAAACAgEAqiqeJwAAAABJRU5ErkJggg==);
        background-size: 0% 0%;
        background-position: 50% 50%;
        background-repeat: no-repeat;
      }

      .pressed {
        border: 1px solid red;
      }

      .touched::after {
        content: "touch";
        display: block;
        position: absolute;
        margin-top: -0.2em;
        margin-left: -0.5em;
        font-size: 0.8em;
        opacity: 0.7;
      }
    </style>
    <script>
      /*
       * Gamepad API Test
       * Written in 2013 by Ted Mielczarek <ted@mielczarek.org>
       *
       * To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
       *
       * You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
       */
      var haveEvents = "GamepadEvent" in window;
      var haveWebkitEvents = "WebKitGamepadEvent" in window;
      var controllers = {};
      var rAF =
        window.mozRequestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.requestAnimationFrame;

      function connecthandler(e) {
        addgamepad(e.gamepad);
      }
      function addgamepad(gamepad) {
        controllers[gamepad.index] = gamepad;
        var d = document.createElement("div");
        d.setAttribute("id", "controller" + gamepad.index);
        var t = document.createElement("h1");
        t.appendChild(document.createTextNode("gamepad: " + gamepad.id));
        d.appendChild(t);
        var b = document.createElement("div");
        b.className = "buttons";
        for (var i = 0; i < gamepad.buttons.length; i++) {
          var e = document.createElement("span");
          e.className = "button";
          //e.id = "b" + i;
          e.innerHTML = i;
          b.appendChild(e);
        }
        d.appendChild(b);
        var a = document.createElement("div");
        a.className = "axes";
        for (i = 0; i < gamepad.axes.length; i++) {
          e = document.createElement("meter");
          e.className = "axis";
          //e.id = "a" + i;
          e.setAttribute("min", "-1");
          e.setAttribute("max", "1");
          e.setAttribute("value", "0");
          e.innerHTML = i;
          a.appendChild(e);
        }
        d.appendChild(a);
        document.getElementById("start").style.display = "none";
        document.body.appendChild(d);
        rAF(updateStatus);
      }

      function disconnecthandler(e) {
        removegamepad(e.gamepad);
      }

      function removegamepad(gamepad) {
        var d = document.getElementById("controller" + gamepad.index);
        document.body.removeChild(d);
        delete controllers[gamepad.index];
      }

      function updateStatus() {
        scangamepads();
        for (j in controllers) {
          var controller = controllers[j];
          var d = document.getElementById("controller" + j);
          var buttons = d.getElementsByClassName("button");
          for (var i = 0; i < controller.buttons.length; i++) {
            var b = buttons[i];
            var val = controller.buttons[i];
            var pressed = val == 1.0;
            var touched = false;
            if (typeof val == "object") {
              pressed = val.pressed;
              if ("touched" in val) {
                touched = val.touched;
              }
              val = val.value;
            }
            var pct = Math.round(val * 100) + "%";
            b.style.backgroundSize = pct + " " + pct;
            b.className = "button";
            if (pressed) {
              b.className += " pressed";
            }
            if (touched) {
              b.className += " touched";
            }
          }

          var axes = d.getElementsByClassName("axis");
          for (var i = 0; i < controller.axes.length; i++) {
            var a = axes[i];
            a.innerHTML = i + ": " + controller.axes[i].toFixed(4);
            a.setAttribute("value", controller.axes[i]);
          }
        }
        rAF(updateStatus);
      }

      function scangamepads() {
        var gamepads = navigator.getGamepads
          ? navigator.getGamepads()
          : navigator.webkitGetGamepads
          ? navigator.webkitGetGamepads()
          : [];
        for (var i = 0; i < gamepads.length; i++) {
          if (gamepads[i] && gamepads[i].index in controllers) {
            controllers[gamepads[i].index] = gamepads[i];
          }
        }
      }

      if (haveEvents) {
        window.addEventListener("gamepadconnected", connecthandler);
        window.addEventListener("gamepaddisconnected", disconnecthandler);
        console.log("gamepad");
      } else if (haveWebkitEvents) {
        window.addEventListener("webkitgamepadconnected", connecthandler);
        window.addEventListener("webkitgamepaddisconnected", disconnecthandler);
        console.log("webkitgamepad");
      } else {
        setInterval(scangamepads, 500);
        console.log("scan");
      }
    </script>
  </head>
  <body>
    <div class="topnav">
      <h1>n64 Controller Debug</h1>
    </div>
    <div class="content">
      <p>
        <i class="fas fa-angle-double-down" style="color: #059e8a"></i> BUTTON:
        <span class="reading"><span id="temp">%TEMPERATURE%</span> </span>
        <i class="fas fa-gamepad" style="color: #00add6"></i> x:
        <span class="reading"><span id="xAxisSpan">%XAXIS%</span></span>
        <i class="fas fa-gamepad" style="color: #e1e437"></i> y:
        <span class="reading"><span id="yAxisSpan">%YAXIS%</span></span>
      </p>
      <p>
        <i class="fas fa-gamepad" style="color: #e1e437"></i> mode (1=ESP-NOW,
        2=BLE):
        <span class="reading"><span id="tx_mode1">%TX_MODE%</span></span>
      </p>

      <!--
        <div class="cards">
          <div class="card">
            <p><i class="fas fa-angle-double-down" style="color:#059e8a;"></i> BUTTON</p><p><span class="reading"><span id="buttonSpan">%BUTTONPRESSED%</span> </span></p>
          </div>
          <div class="card">
            <p><i class="fas fa-gamepad" style="color:#00add6;"></i> x</p><p><span class="reading"><span id="hum">%XAXIS%</span></span></p>
          </div>
          <div class="card">
            <p><i class="fas fa-gamepad" style="color:#e1e437;"></i> y</p><p><span class="reading"><span id="pres">%YAXIS%</span></span></p>
          </div>
                <div class="card">
            <p><i class="fas fa-gamepad" style="color:#e1e437;"></i> mode (1=ESP-NOW, 2=BLE)</p><p><span class="reading"><span id="tx_mode1">%TX_MODE%</span></span></p>
          </div>
        </div>
    -->

      <div class="update">
        <p><a href="/update">Update</a></p>
      </div>
      <h2 id="start">Press button to load BLE Controller</h2>
    </div>

      <script>
    
    if (!!window.EventSource) {
     var source = new EventSource('/events1');
     
     source.addEventListener('open', function(e) {
      //console.log("Events Connected");
     }, false);
     source.addEventListener('error', function(e) {
      if (e.target.readyState != EventSource.OPEN) {
        //console.log("Events Disconnected");
      }
     }, false);
     
     source.addEventListener('message', function(e) {
      //console.log("message", e.data);
     }, false);
     
     source.addEventListener('BUTTONPRESSED', function(e) {
      //console.log("BUTTONPRESSED", e.data);
      document.getElementById("buttonSpan").innerHTML = e.data;
     }, false);
     
     source.addEventListener('XAXIS', function(e) {
      //console.log("XAXIS", e.data);
      document.getElementById("xAxisSpan").innerHTML = e.data;
     }, false);
     
     source.addEventListener('YAXIS', function(e) {
      //console.log("YAXIS", e.data);
      document.getElementById("yAxisSpan").innerHTML = e.data;
     }, false);
    
      source.addEventListener('TX_MODE', function(e) {
      //console.log("TX_MODE", e.data);
      document.getElementById("tx_mode1").innerHTML = e.data;
     }, false);
    }
    </script>

  </body>
</html>
)rawliteral";