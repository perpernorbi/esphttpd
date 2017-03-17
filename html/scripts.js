var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');
var wsUri = "ws://"+window.location.host+"/led-ws.cgi";
var currAp = ""

function toggle() {
    var ajax = new XMLHttpRequest();
    ajax.open("POST", "led.cgi");
    ajax.send("led=t");
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
    updateWifiStatus();
}

/* Close when someone clicks on the "x" symbol inside the overlay */
function closeWifiNav() {
    document.getElementById("wifiNav").style.width = "0%";
}

function updateWifiStatus()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        renderWifiStatus(this.responseText);
      }
    };
    xhttp.open("GET", "wifi/wifistatus.cgi", true);
    xhttp.send();
    scanAPs();
}

function renderWifiStatus(plaintextdata)
{
    data = JSON.parse(plaintextdata);
    currAp = data.currSsid;

    document.getElementById("wifimode").innerHTML = data.WiFiMode;
    if (data.WiFiMode == "SoftAP") {
        document.getElementById("wifinote").innerHTML = "<b>Can't scan in this mode.</b> Click <a href=\"wifi/setmode.cgi?mode=3\">here</a> to go to STA+AP mode."
    } else {
        document.getElementById("wifinote").innerHTML = "Click <a href=\"wifi/setmode.cgi?mode=2\">here</a> to go to standalone AP mode."
    }
}


function scanAPs() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "wifi/wifiscan.cgi");
    xhr.onreadystatechange=function() {
        if (xhr.readyState==4 && xhr.status>=200 && xhr.status<300) {
            var data=JSON.parse(xhr.responseText);
            currAp=getSelectedEssid();
            if (data.result.inProgress=="0" && data.result.APs.length>1) {
                $("#aps").innerHTML="";
                for (var i=0; i<data.result.APs.length; i++) {
                    if (data.result.APs[i].essid=="" && data.result.APs[i].rssi==0) continue;
                    $("#aps").appendChild(createInputForAp(data.result.APs[i]));
                }
                window.setTimeout(scanAPs, 20000);
            } else {
                window.setTimeout(scanAPs, 1000);
            }
        }
    }
    xhr.send();
}

function getSelectedEssid() {
    var e=document.forms.wifiform.elements;
    for (var i=0; i<e.length; i++) {
        if (e[i].type=="radio" && e[i].checked) return e[i].value;
    }
    return currAp;
}

function createInputForAp(ap) {
    if (ap.essid=="" && ap.rssi==0) return;
    var div=document.createElement("div");
    div.id="apdiv";
    var rssi=document.createElement("div");
    var rssiVal=-Math.floor(ap.rssi/51)*32;
    rssi.className="icon";
    rssi.style.backgroundPosition="0px "+rssiVal+"px";
    var encrypt=document.createElement("div");
    var encVal="-64"; //assume wpa/wpa2
    if (ap.enc=="0") encVal="0"; //open
    if (ap.enc=="1") encVal="-32"; //wep
    encrypt.className="icon";
    encrypt.style.backgroundPosition="-32px "+encVal+"px";
    var input=document.createElement("input");
    input.type="radio";
    input.name="essid";
    input.value=ap.essid;
    if (currAp==ap.essid) input.checked="1";
    input.id="opt-"+ap.essid;
    var label=document.createElement("label");
    label.htmlFor="opt-"+ap.essid;
    label.textContent=ap.essid;
    div.appendChild(input);
    div.appendChild(rssi);
    div.appendChild(encrypt);
    div.appendChild(label);
    return div;
}

window.onload = function() { init(); };
