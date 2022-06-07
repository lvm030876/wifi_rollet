const char* helpIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
	</head>
	<body>
		<h1 style="color: #5e9ca0;"><span style="color: #000000;">Команди пристроя</span></h1>
		<div>
			<ul>
				<li><span style="color: #ff0000;">switch <span style="color: #000000;">- керування релейним блоком</span></span></li>
				<ul>
					<li><div>rollet = up/stop/down - відкрити, зупинити, закрити</div></li>
				</ul>
				<li><span style="color: #ff0000;">resalarm <span style="color: #000000;">- перезапуск спрацьованих датчиків</span></span></li>
				<li><span style="color: #ff0000;">switch.xml, switch.json</span></li>
				<li><span style="color: #ff0000;">rf.htm <span style="color: #000000;">- сторінка налаштунку раді-модуля</span></span></li>
				<li><span style="color: #ff0000;">rf.json</span></li>
				<li><span style="color: #ff0000;">ip.htm <span style="color: #000000;">- сторінка налаштунку статичного адресу</span></span></li>
				<li><span style="color: #ff0000;">scanwifi.json</span></li>
				<li><span style="color: #ff0000;">mem <span style="color: #000000;">- збереження налаштувань</span></span></li>
				<ul>
					<li><div>ipStat - статична адреса</div></li>
					<li><div>protTime - час повного закриття/відкриття</div></li>
					<li><div>rfUp - радіо-код відкривання</div></li>
					<li><div>rfStop - радіо-код зупинки</div></li>
					<li><div>rfDown - радіо-код закривання</div></li>
					<li><div>ssid - назва wifi точки доступу</div></li>
					<li><div>pass - пароль доступу до wifi</div></li>
				</ul>
				<li><span style="color: #ff0000;">default <span style="color: #000000;">- онулення налаштувань пристрою</span></span></li>
				<li><span style="color: #ff0000;">reboot <span style="color: #000000;">- перезавантаження пристрою</span></span></li>
			</ul>
		</div>
	</body>
</html>
)=====";

const char* homeIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>ролети</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
			function displaySwitch() {
				function xml_to_css(x, y){
					let a = document.getElementById(x).classList;
					(y == x)?a.remove("cheng"):a.add("cheng");
				}
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
					document.getElementById('mac').innerText = data.switch.mac;
					xml_to_css("up", data.switch.move);
					xml_to_css("stop", data.switch.move);
					xml_to_css("down", data.switch.move);
					setTimeout(displaySwitch, 500);
					})
			};
			function switch_do(todo){
				fetch("switch?rollet="+todo);
			}		
		</script>
	</head>
	<body onload='displaySwitch()'>
		<div class='block header'>ролети<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
			<fieldset>
			<legend>Блок керування</legend>
				<div class="keyboard">
					<button class="btn" id='up' onclick='switch_do("up")'>вiдкрити</button>
					<button class="btn" id='stop' onclick='switch_do("stop")'>зупинити</button>
					<button class="btn" id='down' onclick='switch_do("down")'>закрити</button>
				</div>
			</fieldset>
		</div>
		<div class='block menu'>
			<button onclick="location.href = '/ip.htm'">wifi нал.</button>
			<button onclick="location.href = '/rf.htm'">rf нал.</button>
			<button onclick="location.href = '/help'">help</button>
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
						setTimeout(tick ,500);
					})
			}
            function start(){
				tick();
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

const char* rfIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>ролети</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
        <script>
            var cfg = ['rfUp', 'rfDown', 'rfStop'];
            var rfcode;

            function tick(){
                fetch("rf.json")
                .then(response => response.json())
                .then(data => {
		            if (data.rf.rfNew != rfcode){
                        document.getElementById("rfCode").value = data.rf.rfNew;
                        rfcode = data.rf.rfNew;
                    }
		            setTimeout(tick, 500);
                })
            }

            function start(){
		        tick();
                var allButtons = document.querySelectorAll('.rfcode>button');
                for (var i = 0; i < allButtons.length; i++) {
                    allButtons[i].addEventListener('click', function() {
                        let a = document.getElementById('rfCode').value;
                        fetch("mem?"+this.id+"="+a)
                        .then(response => response.json())
                        .then(data => this.innerText = a);
                    });
                }

				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
					document.getElementById('mac').innerText = data.switch.mac;
					})

                fetch("rf.json")
                .then(response => response.json())
                .then(data => {
                    cfg.forEach(element => {
                        document.getElementById(element).innerText = data.rf[element]
                    });
                })
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
            <legend>Налаштування радіо пульту</legend>
                <div class="rfcode">
                    <div>Зафіксований код:</div>
                    <input id='rfCode' type='text'/>
                    <div>Код вiдкривання:</div>
                    <button id='rfUp'>0</button>
                    <div>Код зупинки:</div>
                    <button id='rfStop'>0</button>
                    <div>Код опускання:</div>
                    <button id='rfDown'>0</button>
                </div>
			</fieldset>
		</div>
		<div class='block menu'>
            <div></div>
            <div></div>
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

const char* style = R"=====(
hr{
font-size:8px;
border:0;
height:2px;
background:#333;
background-image:-webkit-linear-gradient(left, #ccc, #333, #ccc);
}
body{
margin:0;
background:#fff;
font-family:Arial, cursive;
font-style:italic;
}
.block{
margin:1px auto;
font-size:24px;
color:#E8FF66;
border:double 5px #2d2d2d;
width: 380px;
background:#0059B3;
padding:10px 10px 10px 10px;
border-radius: 5px;
text-align: center;
}
.header{
font-size: 32px;
text-shadow: 1px 1px 2px black, 0 0 1em red;
}
.ctrl{
text-align: left;
}
.ipStat{
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 1fr;
}
.rfcode {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 100px;
}
.wificode {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 100px 1fr;
}
.wifiscan {
margin-bottom: 10px;
border: inset 3px #ffff00;
padding: 5px;
font-size: 12px;
display: grid;
grid-gap: 3px;
grid-template-columns: 1fr 1fr 40px;
}
.setauto {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 50px;
}
.menu{
display: grid;
grid-gap: 10px;
grid-template-columns: repeat(3, 1fr);
}
.footer{
display: grid;
grid-gap: 10px;
font-size:10px;
color:#003378;
}
a{
font-size:14px;
color:#E8FF66;
}
input{
background-color: yellow;
border-radius: 5px;
text-align: center;
}
button:visited, button{
width: 100%;
font-size:14px;
cursor:pointer;
display:inline-block;
font-weight:bold;
text-decoration:none;
white-space:nowrap;
border-radius:5px;
background-image: linear-gradient(rgba(255,255,255,.1), rgba(255,255,255,.05) 49%, rgba(0,0,0,.05) 51%, rgba(0,0,0,.1));
background-color:#1F2485;
color:#E8FF66;
border:1px solid rgba(0,0,0,0.3);
border-bottom-color: #4D8Eff;
box-shadow: inset 0 0.08em 0 rgba(255,255,255,0.7), inset 0 0 0.08em rgba(255,255,255,0.5);
height: 30px;
}
button:focus, button:hover{
background-color:#1F2485;
background-image: linear-gradient(rgba(255,255,255,0.5), rgba(255,255,255,0.2) 49%, rgba(0,0,0,0.05) 51%, rgba(0,0,0,0.15));
}
button:disabled{
color:#0059B3;
background-color:#0059B3;
background-image: linear-gradient(rgba(255,255,255,0.5), rgba(255,255,255,0.2) 49%, rgba(0,0,0,0.05) 51%, rgba(0,0,0,0.15));
}
.keyboard {
display: grid;
grid-gap: 10px;
}
.btn{
height: 80px;
font-size: 28px;
outline: none;
border-radius: 40px;
border: 7px solid rgb(255, 0, 0);
}
.cheng{
border-color:rgb(0, 255, 0);
}
)=====";