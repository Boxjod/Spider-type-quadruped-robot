#include <Servo.h>
#include "action.h"
#include <Arduino.h>
#include "html.h"
#include <pins_arduino.h>
#include <flash_hal.h>
#include <ESPAsyncWebServer.h>
#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

int nextAnimation = -1;
int newAction = -1;
IPAddress local_IP(192, 168, 0, 86);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(113,214,230,150);
IPAddress secondaryDNS(113,214,230,160); 

IPAddress local_IP1(192,168,4,100);
IPAddress gateway1(192,168,4,1);  
IPAddress subnet1(255,255,255,0); 

const char* ssid = "Boxjod";//"TP-LINK_0EAEC0"; //连接的路由SSID8
const char* password = "password";//"98765432"; //连接路由密码
#define AP_SSID "Robot_WiFi"    //wif SSID 如手机热点
#define AP_PASS "1594315579"   //密码
AsyncWebServer server(80);
const int numberOfServos = 8; // Number of servos 舵机数

int servoCal[] = {   0,   0,   0,  0,   0,   0,   0,   0 }; // Servo calibration data//修正调整角度
int servoPos[] = {   135,  45, 135,  45,  45, 135,  45, 135 }; // Servo current position///存放当前角度
int startPos[] = {     0,   0,   0,  0,   0,   0,   0,   0 };//起始角度
const int servoPrgPeriod = 20; // 20 ms

Servo servo[numberOfServos];


void runServoPrg(int servoPrg[][numberOfACE], int step)//
{

  for (int i = 0; i < step; i++) 
  { 
    for (int s = 0; s < numberOfServos; s++) //取当前保存的角度
        startPos[s] = servoPos[s];      
    int totalTime = servoPrg[i][numberOfACE - 1]; //取每步的延时时间，加起来
    for (int j = 0; j < totalTime / servoPrgPeriod; j++) { //使用for延时，总时间除以舵机运行时间20ms
      for (int k = 0; k < numberOfServos; k++) {    //    
                    //                                      源角度           目标角度  修正角度
			  int angle = (map(j, 0, totalTime / servoPrgPeriod,startPos[k],servoPrg[i][k]+servoCal[k])) ;//map为一个映射函数，相当于做了一个延时以20ms为单位延时
			   servo[k].write(angle);
			  //Serial.println(angle);
			  servoPos[k]=angle;//保存当前角度;

      }
      delay(servoPrgPeriod);//
    }
  }
}

void ServoSet(int servoNum, int angle1)//
{
  servo[servoNum].write(angle1);
}
void setup() {

	Serial.begin(115200);
	servo[0].attach(D1);//前右脚
	servo[1].attach(D2);//前右腿关节
	servo[2].attach(D3);//后右腿关节
	servo[3].attach(D4);//后右脚
	servo[4].attach(D5);//前左脚
	servo[5].attach(D6);//前左腿关节
	servo[6].attach(D7);//后左腿关节
	servo[7].attach(D8);//后左脚
	WiFi.mode(WIFI_AP_STA);
 
  //WiFi.softAPConfig(local_IP1, gateway1, subnet1);
  WiFi.softAP(AP_SSID, AP_PASS); 
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  ////配置静态IP
 
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))//WIFI分配的IP这使与arduino IDE电脑同一网，方便OTA更新固件 网页http://ip
	{
		Serial.println("...");
	}

	WiFi.begin(ssid, password);//连接WIFI
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.printf("WiFi Failed!\n");
		//delay(5000);
	  //	ESP.restart();
		//return;
	}
//Serial.println(WiFi.localIP());
	/////get请求
	server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
		int params = request->params();
		for (int i = 0; i < params; i++) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->isFile()) { //p->isPost() is also true
				//Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
			}
			else if (p->isPost()) {


			}
			else {
				//取GET参数 如192.168.0.88?aa=11
				Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
				//move(p->value().toInt());
				request->send_P(200, "text/plain", p->value().c_str(), NULL);
				return;
			}
		}
		request->send_P(200, "text/html", htmltxt, NULL);//网页

	});

	////post请求
	server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
		int params = request->params();
		for (int i = 0; i < params; i++) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->isFile()) { //p->isPost() is also true
				//Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
			}
			else if (p->isPost()) {
				newAction= p->value().toInt(); ///取客户端POST数据
       if(newAction >= 0 && newAction <= 9000)
            nextAnimation = newAction;

				Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
				request->send_P(200, "text/plain", p->value().c_str(), NULL);//返回给客户端

			}
			else {
				
				Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
				request->send_P(200, "text/html", p->value().c_str(), NULL);

			}
		}

	});
	server.onNotFound([](AsyncWebServerRequest *request) {
		//无效的请求
		request->send(404);
	});
  server.begin(); // 启用 web服务器
runServoPrg(servoPrg01, servoPrg01step);//

}

void loop() 
{
	//Serial.println("Restart");
  //delay(1000);
  if(nextAnimation != -1)
  {
    Serial.println(nextAnimation);
    if (nextAnimation/1000)
    {
      ServoSet((nextAnimation/1000)-1,nextAnimation%1000);
    }
    else if((nextAnimation>25) && (nextAnimation<1000))
    {
      
      ServoSet((nextAnimation/100)-1,nextAnimation%100);
    }
    else if(nextAnimation<25)
    {
      switch(nextAnimation){
          case 0:
              runServoPrg(servoPrg00, servoPrg00step);
              break;
          case 1:
              runServoPrg(servoPrg01, servoPrg01step);
              break;
          case 2:
              runServoPrg(servoPrg02, servoPrg02step);
              runServoPrg(servoPrg02, servoPrg02step);
              break;
          case 3:
              runServoPrg(servoPrg03, servoPrg03step);
              runServoPrg(servoPrg03, servoPrg03step);
              break;
          case 4:
              runServoPrg(servoPrg04, servoPrg04step);
              break;
          case 5:
              runServoPrg(servoPrg05, servoPrg05step);
              break;
          case 6:
              runServoPrg(servoPrg06, servoPrg06step);
              break;
          case 7:
              runServoPrg(servoPrg07, servoPrg07step);
              break;
          case 8:
              runServoPrg(servoPrg08, servoPrg08step);
              break;
          case 9:
              runServoPrg(servoPrg09, servoPrg09step);
              break;
          case 10:
              runServoPrg(servoPrg10, servoPrg10step);
              break;
          case 11:
              runServoPrg(servoPrg11, servoPrg11step);
              break;
          case 12:
              runServoPrg(servoPrg12, servoPrg12step);
              break;
          case 13:
              runServoPrg(servoPrg13, servoPrg13step);
              break;
          case 14:
              runServoPrg(servoPrg14, servoPrg14step);
              break;
          case 15:
              runServoPrg(servoPrg15, servoPrg15step);
              break;
        case 24:
          //ESPhttpUpdate.update("http://yourchosenname.000webhostapp.com/esp8266.bin");//
          break;
        case 25:
          break;

          default:
              break;
      }
    }
    nextAnimation = -1;
  }
  
    // nextAnimation = -1;



 
}