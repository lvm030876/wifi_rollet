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
				<li><span style="color: #ff0000;">куищще <span style="color: #000000;">- перезавантаження пристрою</span></span></li>
			</ul>
		</div>
	</body>
</html>
)=====";