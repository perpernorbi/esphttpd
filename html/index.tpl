<html><head><title>%devicename%</title>
<meta charset="UTF-8">
<meta name="viewport" content="initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
<link rel="stylesheet" type="text/css" href="skeleton.css">
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<ul class="topnav" id="myTopnav">
 <li><a href="/wifi">Wifi</a></li>
</ul>
<h1>%devicename%</h1>
<div id="bulb">
 <img id="on"  src="bulb_on.png" onclick="toggle()"></img>
 <img id="off" src="bulb_off.png" onclick="toggle()"></img>
</div>
<p>

</div>
<script language="javascript" type="text/javascript">
var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');
//var wsUri = "ws://"+window.location.host+"/led-ws.cgi";
var wsUri = "ws://192.168.10.163/led-ws.cgi";


function toggle() {
    var ajax = new XMLHttpRequest();
    ajax.open('POST', 'led.cgi');
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

window.onload = function() { init(); };
</script>
</body></html>
