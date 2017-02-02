<html><head><title>%devicename%</title>
<meta charset="UTF-8">
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<h1>%devicename%</h1>
<div id="bulb">
 <img id="on"  src="bulb_on.png" onclick="toggle()"></img>
 <img id="off" src="bulb_off.png" onclick="toggle()"></img>
</div>
<p>
<!--
<a href="/wifi">do so.</a>
-->
</div>
<script language="javascript" type="text/javascript">
var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');
var wsUri = "ws://"+window.location.host+"/led-ws.cgi";

function updateLightBulb(state) {
if (state == "on") {
    onBulb.style.opacity = 1;
    offBulb.style.opacity = 0;
} else {
    onBulb.style.opacity = 0;
    offBulb.style.opacity = 1;
}

}
function toggle() {
    var ajax = new XMLHttpRequest();
    ajax.open('POST', 'led.cgi');
    ajax.send('led=t');
}

function init()
{
    testWebSocket();
}

  function testWebSocket()
  {
    websocket = new WebSocket(wsUri);
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
  }

  function onOpen(evt)
  {
  }

  function onMessage(evt)
  {
    state = JSON.parse(evt.data).led;
    updateLightBulb(state);
  }

  function doSend(message)
  {
    writeToScreen("SENT: " + message);
    websocket.send(message);
  }

  window.addEventListener("load", init, false);
</script>
</body></html>
