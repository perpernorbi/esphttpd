<html>
	<head>
		<title>%devicename%</title>
		<meta charset="UTF-8">
		<meta name="viewport" content="initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
		<link rel="stylesheet" type="text/css" href="skeleton.css">
		<link rel="stylesheet" type="text/css" href="style.css">
	</head>
	<body>
        <div id="wifiNav" class="overlay">
            <!-- Button to close the overlay navigation -->
            <a href="javascript:void(0)" class="closebtn" onclick="closeWifiNav()">&times;</a>

            <!-- Overlay content -->
            <div class="overlay-content">
            <p>Current WiFi mode: %WiFiMode%</p>
            <p>Note: %WiFiapwarn%</p>
            <form name="wifiform" action="connect.cgi" method="post">
                <p>To connect to a WiFi network, please select one of the detected networks...<br>
                    <div id="aps">Scanning...</div>
                    <br>
                    WiFi password, if applicable: <br />
                    <input type="text" name="passwd" val="%WiFiPasswd%"> <br />
                    <input type="submit" name="connect" value="Connect!">
                </p>
            </form>
            </div>
        </div>
        <ul class="topnav" id="myTopnav">
            <li><a href="javascript:void(0)" onclick="openWifiNav()">Wifi</a></li>
        </ul>
        <h1>%devicename%</h1>
        <div id="bulb">
            <img id="on"  src="bulb_on.png" onclick="toggle()"></img>
            <img id="off" src="bulb_off.png" onclick="toggle()"></img>
        </div>
        <script src="scripts.js"></script>
	</body>
</html>
