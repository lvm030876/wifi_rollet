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
                    cfg.forEach(element => {
                        document.getElementById(element).innerText = data.rf[element]
                    });
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