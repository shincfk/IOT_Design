
/* 植物浇灌照明demo */
/*Luoo*/

#include <SPI.h>

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

#include <Wire.h>

#include <math.h>



int analogPin = 0; 
int jidianqi = 10;
int count = 0;
int val = 500;

int a =30;  

/* 接入Yeelink平台的初始化配置，包括API key、设备号、
 
 传感器号、服务器名称、本机MAC、平台IP、默认发送间隔等 */

// Yeelink API key

#define APIKEY       "e38e138ea018c9a0a42bf1d405f8b0bf" 

// Yeelink网站上的您设备号，请参见Yeelink平台手册获取

#define DEVICEID     14887

// 您的传感器号码

#define SENSORID     25551



byte mac[] = {
  0x00, 0x1D, 0x72, 0x82, 0x35, 0x9D};

byte ip[] = { 
  192,168,0,120 };

EthernetClient client;

// Yeelink平台的入口服务器IP

char server[] = "api.yeelink.net"; // yeelink API的域名

// 上次连接到服务器的时间，毫秒

unsigned long lastConnectionTime = 0;          

// 上次连接状态

boolean lastConnected = false;               


const unsigned long postingInterval = 30*1000; 



/* setup函数为全局函数，在整个程序中只执行一次，此函数的作用
 
 是对Arduino硬件进行初始化设置，包括启动Wire库函数、设定波特率
 
 以及以DHCP模式启动W5100上网设置 */

void setup() {


  Wire.begin();

  Serial.begin(57600);
  pinMode(7,OUTPUT);
  pinMode(jidianqi,OUTPUT);
  pinMode(10,OUTPUT);

  if (Ethernet.begin(mac) == 0) {

    Serial.println("Failed to configure Ethernet using DHCP.");

    for( ; ; )

      ;

  }

  else {

    Serial.println("Ethernet configuration OK.");

  }
  Ethernet.begin(mac,ip);

}



/* loop函数为程序的主函数，所有的程序操作都在loop函数中实现 */

void loop() {

  // 首先判断是否有网络数据的传入，如果有，则在串口将数据打印

  if (client.available()) {

    char c = client.read();

    Serial.print(c);

  }



  // 如果没有网络数据传入，但是是最后一次通过loop函数，则停止客户端

  if (!client.connected() && lastConnected) {

    Serial.println();

    Serial.println("disconnecting.");

    client.stop();

  }



  // 如果没有连接，但是从上次连接到现在经过10秒钟时，则重新连接并且发送数据

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {



    // 在发送数据的函数中，首先要读取传感器的数据，并且把它经过函数处理后赋值给sensorReading

    int sensorReading = (analogRead(analogPin)+2)/7.3;



    // 获得数据后，调用sendData函数将数据发送到Yeelink平台

    sendData(sensorReading);

  }

  // 存储连接状态，为下次通过loop函数

  lastConnected = client.connected();


  int sensorValue = analogRead(analogPin); //灯光补偿
  int n = analogRead(A1); 
  if (n>= a )                   
  {
    digitalWrite(7,HIGH);
  }
  else
  {
    digitalWrite(7,LOW);
  }

  if(analogRead(analogPin) > val)  //浇灌补偿
  {
    count++;
  }
  if(count > 20)
  {
    count = 0;

    digitalWrite(jidianqi,HIGH);
    delay(8000);            //浇灌时间
    digitalWrite(jidianqi,LOW); 
    delay(1000);
  }
  else
  {
    digitalWrite(jidianqi,LOW);
    delay(1000);

  }

  

}

// sendData函数实现了一个到服务器的HTTP连接

void sendData(int thisData) {

  // 如果连接成功

  if (client.connect(server, 80)) {

    Serial.println("connecting...");

    // 发送HTTP POST请求

    client.print("POST /v1.0/device/");

    client.print(DEVICEID);

    client.print("/sensor/");

    client.print(SENSORID);

    client.print("/datapoints");

    client.println(" HTTP/1.1");

    client.println("Host: api.yeelink.net");

    client.print("Accept: *");

    client.print("/");

    client.println("*");

    client.print("U-ApiKey: ");

    client.println(APIKEY);

    client.print("Content-Length: ");



    // 计算传感器数据长度

    int thisLength = 10 + getLength(thisData);

    client.println(thisLength);

    client.println("Content-Type: application/x-www-form-urlencoded");

    client.println("Connection: close");

    client.println();

    // POST请求的实际内容

    client.print("{\"value\":");

    client.print(thisData);

    client.println("}");

  } 
  else {

    // 如果无法建立连接

    Serial.println("connection failed");

    Serial.println();

    Serial.println("disconnecting.");

    client.stop();

  }

  // 标记已经进行连接或尝试连接的时间

  lastConnectionTime = millis();

}


int getLength(int someValue) {

  int digits = 1;

  int dividend = someValue / 10;

  while (dividend > 0) {

    dividend = dividend / 10;

    digits++;

  }

  return digits;

}

