var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');
//var wsUri = "ws://"+window.location.host+"/led-ws.cgi";
var wsUri = "ws://192.168.10.163/led-ws.cgi";


function toggle() {
    var ajax = new XMLHttpRequest();
//    ajax.open('POST', 'led.cgi');
    ajax.open('POST', 'http://192.168.10.163/led.cgi');
    ajax.send('led=t');
}

function init() {
    websocket = new WebSocket(wsUri);
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
}

function onOpen(evt)
{
  websocket.send("getLedStatus");
}

function onMessage(evt) {
  state = JSON.parse(evt.data).led;
  updateLightBulb(state);
}

function updateLightBulb(state) {
  if (state == "on") {
    onBulb.style.opacity = 1;
    offBulb.style.opacity = 0;
  } else {
    onBulb.style.opacity = 0;
    offBulb.style.opacity = 1;
  }
}

/* Open when someone clicks on the span element */
function openWifiNav() {
    document.getElementById("wifiNav").style.width = "100%";
}

/* Close when someone clicks on the "x" symbol inside the overlay */
function closeWifiNav() {
    document.getElementById("wifiNav").style.width = "0%";
}

window.onload = function() { init(); };
