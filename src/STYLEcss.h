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