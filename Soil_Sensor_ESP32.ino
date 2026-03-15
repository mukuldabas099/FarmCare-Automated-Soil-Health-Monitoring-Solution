#include <WiFi.h>
#include <WebServer.h>

#define RE_PIN 4
#define DE_PIN 5
#define TIMEOUT_MS 300

HardwareSerial mod(2);

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

const byte moisture[]     = {0x01,0x03,0x00,0x12,0x00,0x01,0x24,0x0F};
const byte temperature[]  = {0x01,0x03,0x00,0x13,0x00,0x01,0x75,0xCF};
const byte conductivity[] = {0x01,0x03,0x00,0x15,0x00,0x01,0x95,0xCE};
const byte phFrame[]      = {0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};
const byte nitrogen[]     = {0x01,0x03,0x00,0x1E,0x00,0x01,0xE4,0x0C};
const byte phosphorus[]   = {0x01,0x03,0x00,0x1F,0x00,0x01,0xB5,0xCC};
const byte potassium[]    = {0x01,0x03,0x00,0x20,0x00,0x01,0x85,0xC0};

byte responseValues[11];

float tempVal, moistVal, ecVal, phVal, nitroVal, phosVal, potaVal;

String webpage = R"=====(

<!DOCTYPE html>
<html>
<head>
<title>Smart Soil Monitoring Dashboard</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
font-family:Arial;
background:linear-gradient(135deg,#1e3c72,#2a5298);
margin:0;
color:white;
text-align:center;
}

h1{
margin-top:20px;
font-size:36px;
}

.container{
display:grid;
grid-template-columns:repeat(auto-fit,minmax(250px,1fr));
gap:25px;
padding:30px;
}

.card{
background:rgba(255,255,255,0.15);
border-radius:15px;
padding:25px;
box-shadow:0 8px 25px rgba(0,0,0,0.4);
}

.sensor{
font-size:20px;
margin-bottom:10px;
font-weight:bold;
}

.value{
font-size:34px;
margin:10px 0;
}

.range{
font-size:13px;
opacity:0.8;
}

.bar{
width:100%;
height:12px;
background:#ccc;
border-radius:10px;
overflow:hidden;
margin-top:10px;
}

.fill{
height:100%;
background:#00ff9c;
width:0%;
}

.status{
margin-top:8px;
font-weight:bold;
}

.good{color:#00ff9c;}
.low{color:#ffd54f;}
.high{color:#ff5252;}

</style>
</head>

<body>

<h1>Smart Soil Monitoring Dashboard</h1>

<div class="container">

<div class="card">
<div class="sensor">Temperature</div>
<div class="value" id="temp">--</div>
<div class="bar"><div id="tempBar" class="fill"></div></div>
<div class="status" id="tempStatus"></div>
<div class="range">Ideal Range: 20 - 30 C</div>
</div>

<div class="card">
<div class="sensor">Moisture</div>
<div class="value" id="moist">--</div>
<div class="bar"><div id="moistBar" class="fill"></div></div>
<div class="status" id="moistStatus"></div>
<div class="range">Ideal Range: 30 - 60 %</div>
</div>

<div class="card">
<div class="sensor">Electrical Conductivity</div>
<div class="value" id="ec">--</div>
<div class="bar"><div id="ecBar" class="fill"></div></div>
<div class="status" id="ecStatus"></div>
<div class="range">Ideal Range: 200 - 1200 uS/cm</div>
</div>

<div class="card">
<div class="sensor">pH Level</div>
<div class="value" id="ph">--</div>
<div class="bar"><div id="phBar" class="fill"></div></div>
<div class="status" id="phStatus"></div>
<div class="range">Ideal Range: 6.0 - 7.5</div>
</div>

<div class="card">
<div class="sensor">Nitrogen</div>
<div class="value" id="n">--</div>
<div class="bar"><div id="nBar" class="fill"></div></div>
<div class="status" id="nStatus"></div>
<div class="range">Ideal Range: 50 - 150 mg/kg</div>
</div>

<div class="card">
<div class="sensor">Phosphorus</div>
<div class="value" id="p">--</div>
<div class="bar"><div id="pBar" class="fill"></div></div>
<div class="status" id="pStatus"></div>
<div class="range">Ideal Range: 15 - 40 mg/kg</div>
</div>

<div class="card">
<div class="sensor">Potassium</div>
<div class="value" id="k">--</div>
<div class="bar"><div id="kBar" class="fill"></div></div>
<div class="status" id="kStatus"></div>
<div class="range">Ideal Range: 120 - 250 mg/kg</div>
</div>

</div>

<script>

function updateBar(id,value,max){
let percent=(value/max)*100
if(percent>100) percent=100
document.getElementById(id).style.width=percent+"%"
}

function checkStatus(value,min,max,id){
let status=document.getElementById(id)

if(value<min){
status.innerHTML="LOW"
status.className="status low"
}
else if(value>max){
status.innerHTML="HIGH"
status.className="status high"
}
else{
status.innerHTML="GOOD"
status.className="status good"
}
}

setInterval(function(){

fetch("/data")
.then(response=>response.json())
.then(data=>{

document.getElementById("temp").innerHTML=data.temp+" C"
document.getElementById("moist").innerHTML=data.moist+" %"
document.getElementById("ec").innerHTML=data.ec+" uS/cm"
document.getElementById("ph").innerHTML=data.ph
document.getElementById("n").innerHTML=data.n+" mg/kg"
document.getElementById("p").innerHTML=data.p+" mg/kg"
document.getElementById("k").innerHTML=data.k+" mg/kg"

updateBar("tempBar",data.temp,50)
updateBar("moistBar",data.moist,100)
updateBar("ecBar",data.ec,2000)
updateBar("phBar",data.ph,14)
updateBar("nBar",data.n,200)
updateBar("pBar",data.p,100)
updateBar("kBar",data.k,300)

checkStatus(data.temp,20,30,"tempStatus")
checkStatus(data.moist,30,60,"moistStatus")
checkStatus(data.ec,200,1200,"ecStatus")
checkStatus(data.ph,6,7.5,"phStatus")
checkStatus(data.n,50,150,"nStatus")
checkStatus(data.p,15,40,"pStatus")
checkStatus(data.k,120,250,"kStatus")

})

},2000)

</script>

</body>
</html>

)=====";

int16_t querySensor(const byte *frame, byte frameSize){

digitalWrite(DE_PIN,HIGH);
digitalWrite(RE_PIN,HIGH);
delayMicroseconds(200);

while(mod.available()) mod.read();

mod.write(frame,frameSize);
mod.flush();

digitalWrite(DE_PIN,LOW);
digitalWrite(RE_PIN,LOW);

unsigned long start=millis();
int len=0;

while((millis()-start)<TIMEOUT_MS){
if(mod.available()){
if(len<sizeof(responseValues)){
responseValues[len++]=mod.read();
}
}
}

if(len>=7 && responseValues[0]==0x01 && responseValues[1]==0x03){
int16_t val=(responseValues[3]<<8)|responseValues[4];
return val;
}

return -1;
}

void readSensors(){

int16_t tempRaw   = querySensor(temperature,sizeof(temperature));
int16_t moistRaw  = querySensor(moisture,sizeof(moisture));
int16_t ecRaw     = querySensor(conductivity,sizeof(conductivity));
int16_t phRaw     = querySensor(phFrame,sizeof(phFrame));
int16_t nitroRaw  = querySensor(nitrogen,sizeof(nitrogen));
int16_t phosRaw   = querySensor(phosphorus,sizeof(phosphorus));
int16_t potaRaw   = querySensor(potassium,sizeof(potassium));

tempVal  = (tempRaw!=-1)?tempRaw/10.0:-1;
moistVal = (moistRaw!=-1)?moistRaw/10.0:-1;
ecVal    = (ecRaw!=-1)?ecRaw:-1;
phVal    = (phRaw!=-1)?phRaw/100.0:-1;
nitroVal = (nitroRaw!=-1)?nitroRaw:-1;
phosVal  = (phosRaw!=-1)?phosRaw:-1;
potaVal  = (potaRaw!=-1)?potaRaw:-1;

}

void setup(){

Serial.begin(115200);
mod.begin(9600,SERIAL_8N1,16,17);

pinMode(RE_PIN,OUTPUT);
pinMode(DE_PIN,OUTPUT);

digitalWrite(RE_PIN,LOW);
digitalWrite(DE_PIN,LOW);

WiFi.begin(ssid,password);

Serial.print("Connecting WiFi");

while(WiFi.status()!=WL_CONNECTED){
delay(500);
Serial.print(".");
}

Serial.println();
Serial.println("WiFi Connected");

Serial.print("Open Browser: http://");
Serial.println(WiFi.localIP());

server.on("/",[](){
server.send(200,"text/html",webpage);
});

server.on("/data",[](){

readSensors();

String json="{";
json+="\"temp\":"+String(tempVal)+",";
json+="\"moist\":"+String(moistVal)+",";
json+="\"ec\":"+String(ecVal)+",";
json+="\"ph\":"+String(phVal)+",";
json+="\"n\":"+String(nitroVal)+",";
json+="\"p\":"+String(phosVal)+",";
json+="\"k\":"+String(potaVal);
json+="}";

server.send(200,"application/json",json);

});

server.begin();

}

void loop(){
server.handleClient();
}