<html>
	<head>
		<title>%devicename%</title>
		<meta charset="UTF-8">
		<meta name="viewport" content="initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
		<link rel="stylesheet" type="text/css" href="skeleton.css">
        <script type="text/javascript" src="wifi/140medley.min.js"></script>
		<link rel="stylesheet" type="text/css" href="style.css">
	</head>
	<body>
        <div id="wifiNav" class="overlay">
            <!-- Button to close the overlay navigation -->
            <a href="javascript:void(0)" class="closebtn" onclick="closeWifiNav()">&times;</a>

            <!-- Overlay content -->
            <div class="overlay-content">
            <p>Current WiFi mode: <span id="wifimode"></span></p>
            <p><span id="wifinote"></span></p>
            <form name="wifiform" action="wifi/connect.cgi" method="post">
                <p>To connect to a WiFi network, please select one of the detected networks...<br>
                    <ul id="aps"><p>Scanning...</p></ul>
                    <p>WiFi password, if applicable:</p>
                    <input type="text" name="passwd" val="%WiFiPasswd%"> <br />
                    <input class="button-primary" type="submit" name="connect" value="Connect!">
                </p>
            </form>
            </div>
        </div>
        <div id="main">
            <ul class="topnav" id="myTopnav">
                <li><a href="javascript:void(0)" onclick="openWifiNav()"><img src="wifi.png"/><span> Wifi</span></a></li>
            </ul>
            <h1>%devicename%</h1>
            <div id="bulb">
                <img id="on"  src="bulb_on.png" onclick="toggle()"></img>
                <img id="off" src="bulb_off.png" onclick="toggle()"></img>
            </div>
        </div>
        <script src="scripts.js"></script>
	</body>
</html>
