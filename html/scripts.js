var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');
//var wsUri = "ws://"+window.location.host+"/led-ws.cgi";
var wsUri = "ws://192.168.10.218/led-ws.cgi"
var currAp = ""
var hueSlider = document.getElementById("hueSlider");
var saturationSlider = document.getElementById("saturationSlider");
var lightnessSlider = document.getElementById("lightnessSlider");
var websocket;

function getCssValuePrefix()
{
    var rtrnVal = '';//default to standard syntax
    var prefixes = ['-o-', '-ms-', '-moz-', '-webkit-'];

    // Create a temporary DOM object for testing
    var dom = document.createElement('div');

    for (var i = 0; i < prefixes.length; i++)
    {
        // Attempt to set the style
        dom.style.background = prefixes[i] + 'linear-gradient(#000000, #ffffff)';

        // Detect if the style was successfully set
        if (dom.style.background)
        {
            rtrnVal = prefixes[i];
        }
    }

    dom = null;
    delete dom;

    return rtrnVal;
}

var cssValuePrefix = getCssValuePrefix();

/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   {number}  h       The hue
 * @param   {number}  s       The saturation
 * @param   {number}  l       The lightness
 * @return  {Array}           The RGB representation
 */
function hslToRgb(h, s, l){
    var r, g, b;

    if(s == 0){
        r = g = b = l; // achromatic
    }else{
        var hue2rgb = function hue2rgb(p, q, t){
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1/6) return p + (q - p) * 6 * t;
            if(t < 1/2) return q;
            if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
        }

        var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        var p = 2 * l - q;
        r = hue2rgb(p, q, h + 1/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1/3);
    }

    return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
}

function toggle() {
    var ajax = new XMLHttpRequest();
    ajax.open("POST", "led.cgi");
    ajax.send("led=t");
}

function updateSliderBackgrounds()
{
    if ( typeof updateSliderBackgrounds.saturationSlider == 'undefined' ) {
        updateSliderBackgrounds.saturationSlider = document.getElementById("saturationSlider")
        updateSliderBackgrounds.lightnessSlider = document.getElementById("lightnessSlider")
    }
    var h = hueSlider.value / 1000;
    saturatedRgb = hslToRgb(h, 0.5, 0.5);
    saturatedRgb = "rgb(" + saturatedRgb[0] + ", " + saturatedRgb[1]+ ", " + saturatedRgb[2]+ ")"
    updateSliderBackgrounds.saturationSlider.style.background =
            cssValuePrefix + "linear-gradient(left,  #7F7F7F, " + saturatedRgb + ")"
    updateSliderBackgrounds.lightnessSlider.style.background =
            cssValuePrefix + "linear-gradient(left,  #000000, " + saturatedRgb + ", #FFFFFF)"
}

function updateHSL()
{
    var h = hueSlider.value / 1000;
    var s = saturationSlider.value / 1000;
    var l = lightnessSlider.value / 1000;
    rgb = hslToRgb(h, s, l);
    var led = { led: { r: rgb[0], g: rgb[1], b: rgb[2]}};
    websocket.send(JSON.stringify(led));
    updateSliderBackgrounds();
}

function init() {
    websocket = new WebSocket(wsUri);
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    hueSlider.oninput = function() { updateHSL() };
    saturationSlider.oninput = function() { updateHSL() };
    lightnessSlider.oninput = function() { updateHSL() };
    updateSliderBackgrounds();
}

function onOpen(evt) {
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
        document.getElementById("main").style.marginRight = "350px";
    } else {
        document.getElementById("wifiNav").style.width = "100%";
    }
    updateWifiStatus();
}

/* Close when someone clicks on the "x" symbol inside the overlay */
function closeWifiNav() {
    document.getElementById("wifiNav").style.width = "0%";
    document.getElementById("main").style.marginRight = "0px";
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
