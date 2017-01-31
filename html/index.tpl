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
<script>
var onBulb = document.getElementById('on');
var offBulb = document.getElementById('off');

function toggle() {
    var ajax = new XMLHttpRequest();
    ajax.onreadystatechange = function() {
        if (ajax.readyState == 4) {
            if (ajax.responseText == "on") {
                onBulb.style.opacity = 0;
                offBulb.style.opacity = 1;
            } else {
                onBulb.style.opacity = 1;
                offBulb.style.opacity = 0;
            }
        }
    }
    ajax.open('POST', 'led.cgi');
    ajax.send('led=t');
}
</script>
</body></html>
