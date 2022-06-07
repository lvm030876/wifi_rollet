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