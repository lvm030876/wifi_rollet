const char* ipIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>ролети</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
			function selssid(ssid){
				document.getElementById('ssid').value = ssid;
			}
			function tick(){
				fetch("scanwifi.json")
					.then(response => response.json())
					.then(data => {
						var outstr = "";
						var [block] = document.getElementsByClassName("wifiscan");
						outstr += "<div>WiFi</div><div>MAC</div><div>dBm</div>";
						data.scan.forEach(i => {
							outstr += "<a href='javascript:selssid(\"" + i.ssid + "\")'>" + i.ssid + "</a>";
							outstr += "<div>" + i.mac + "</div>";
							outstr += "<div>" + i.rssi + "</div>";
						})
						block.innerHTML = outstr;
					})
			}
            function start(){
				setInterval(tick, 3000);
                document.getElementById("ipStat").value = window.location.host.toString();
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
					document.getElementById('mac').innerText = data.switch.mac;
					})
            }
            function ipSet(){
                let ipStat = document.getElementById('ipStat').value;
                let ssid = document.getElementById('ssid').value;
                let pass = document.getElementById('pass').value;
				let s = "mem?";
				if (ipStat != "") s += "ipStat=" + ipStat + "&";
				if (ssid != "") s += "ssid=" + ssid + "&";
				if (pass != "") s += "pass=" + pass + "&";
                fetch(s)
	            .then(response => {
                    console.log(response.json());
                    if (ipStat != window.location.host.toString()) location.href = 'http://' + ipStat + '/';
					else location.href = '/'
                    });
            }
        </script>
	</head>
	<body onload='start()'>
		<div class='block header'>ролети<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
            <fieldset>
            <legend>WiFi налаштування</legend>
                <div class="wifiscan">
					<div>WiFi</div><div>MAC</div><div>dBm</div>
				</div>
                <div class="wificode">
                    <div>назва wifi:</div>
                    <input type='text' title='назва wifi' id='ssid'/>
                    <div>пароль wifi:</div>
                    <input type='password' title='пароль wifi' id='pass'/>
                    <div>IP адреса:</div>
                    <input type='text' title='статична IP адреса' name='ipStat' id='ipStat' pattern='\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}'/>
                </div>
            </fieldset>
		</div>
		<div class='block menu'>
			<button onclick="ipSet()">примінити</button>
			<button onclick="location.href = '/reboot'">reboot</button>
			<button onclick="location.href = '/'">на головну</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";