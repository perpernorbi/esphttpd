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
    if (document.body.clientWidth > 800) {
        document.getElementById("wifiNav").style.width = "350px";
        document.getElementById("main").style.marginLeft = "350px";
    } else {
        document.getElementById("wifiNav").style.width = "100%";
    }
    updateWifiStatus();
}

/* Close when someone clicks on the "x" symbol inside the overlay */
function closeWifiNav() {
    document.getElementById("wifiNav").style.width = "0%";
    document.getElementById("main").style.marginLeft = "0px";
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
    var li = document.createElement("li");
    var input = document.createElement("input");
    input.type = "radio";
    input.name = "essid";
    input.value = ap.essid;
    input.id = "opt-"+ap.essid;
    if (currAp == ap.essid) input.checked = "1";
    var essid = document.createElement("label");
    essid.textContent = ap.essid;
    essid.htmlFor = "opt-"+ap.essid;
    li.appendChild(input);
    li.appendChild(essid);
    return li;
}

window.onload = function() { init(); };
