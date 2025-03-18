
#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include<Wire.h>
#include <Servo.h>
#include "Adafruit_MPU6050.h"
#include "Adafruit_Sensor.h"
#include "arduino_secrets.h" 
#include<Wire.h>
/* 
const int MPU_addr = 0x68; // I2C address of the MPU-6050
float xa, ya, za, roll, pitch; */
//Adafruit_MPU6050 mpu;

Servo servox;
Servo servoy;
int servoxpos = 90;
int servoxpin = 9;
int servoypos = 90;
int servoypin = 10;
const int avgbufsize = 10;

int MSGLEN = 200;

int y_sh;
double sp;

// for pid controller
unsigned long last_time; 
double total_e, last_e;
double control_signal;
int T = 10; 
/* double kp = 1.5;    T =5 ok 
double ki = 0.125; 
double kd = 50; */    
int max_control = 90;
int min_control = -90;

double _integral = 0;
double derivative = 0;

double kp = 0.2;   
double ki = 0.012; 
double kd = 2; 

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

String rcvbuffer = String(80); 
String sndbuffer = String(80); 
#define BUFSIZE = 

WiFiUDP udpsocket;
IPAddress server(10,160,0,182);

float x, y, z;
int angleX = 0;
int angleY = 0;
int degreesX = 0;
int degreesY = 0;
unsigned long previousMillis = 0;

void setup() {
  //Initialize serial and wait for port to open:
  /* Wire.begin();

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 */
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
/* 
  if (!mpu.begin()) {
  Serial.println("Failed to find MPU6050 chip");
  while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); */


  // init gyro
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  // init servo
  servox.attach(10);
  servoy.attach(9);
  servoy.write(90);
  if(servox.attached())
  {
    Serial.println("servox active");
  }else{
    Serial.println("failed");
  }


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.print("Please upgrade the firmware to");
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
  }


  // scan for existing networks:
/* Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000); */
  

  // attempt to connect to Wifi network:
while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    //status = WiFi.beginEnterprise(ssid,"arduino",pass);

    // wait 10 seconds for connection:
    delay(1000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();  
}


void loop() { 
  if (!udpsocket.begin(7000)) { 
    while(1)Serial.println("udp socket failed");
  } 

    unsigned long last_time = millis();
    unsigned long last_recv_time = millis();
    while (1) {
        unsigned long current_time = millis();
        IMU.readAcceleration(x, y, z);


        char imumsg[MSGLEN] = {'\0'};
        String imustring = String(""); 
        imustring += String(x) + ":" + String(y) + ":" + String(x); 
        imustring.toCharArray(imumsg,100);
        udpsocket.beginPacket(server,8000);
        udpsocket.write(imumsg,100);
        udpsocket.endPacket();  
      

        if(current_time - last_recv_time > 100){
          int bytes = udpsocket.available();
          bytes = udpsocket.parsePacket();
          if(bytes){
            char rcvbuff[MSGLEN] = {'\0'};  
            udpsocket.read(rcvbuff,bytes);
            //Serial.println(rcvbuffer);
            String message = String(rcvbuff);
            kp = String(strtok(rcvbuff,":")).toDouble();
            ki = String(strtok(NULL,":")).toDouble();
            kd = String(strtok(NULL,":")).toDouble();
            T  = String(strtok(NULL,":")).toInt();
            Serial.print("T:");Serial.println(T);
          } 
          last_recv_time = millis();
        }



        //servoxpos = map(x*100, -100,100, 0, 180);
        //servoypos = map(y*100, -100,100, 0, 180);
        //Serial.println(servoxpos);

        //servoxpos = shift_calc_avg(xavgbuf,servoxpos);
        //servoypos = shift_calc_avg(yavgbuf,servoypos);

        //Serial.println(servoxpos);
        // -1.0 : 1.0 
        //double mappedy = map(y*100, -100,100, 0, 180);
        //int y_sh; // = y *100;

        //if(y > 0){y_sh = map(y*10,0,100,0,90);}
        //else{y_sh = map(y*10,-100,0,-90,0);};
        //Serial.print(y); Serial.print(" : "); Serial.println(y_sh); 

        y_sh = map(y*100,-100,100,-90,90);


        current_time = millis();
        int delta_time = current_time - last_time;
        if(delta_time >= T ){
          sp = 0; 
          //Serial.println(y);
          double e  = sp - y_sh;  
          total_e += e; // accumulates total error -> integral term
          if(total_e >= max_control){total_e = max_control; } // bounds checking
          else if(total_e <= min_control){total_e = min_control;}

          _integral += e * T;
          
          derivative = (e - last_e)/T; // difference of error -> derivative term;

          control_signal = kp*e + (ki)*_integral + (kd)*derivative; // + (ki*T)*total_e ; //PID control compute

          if (control_signal >= max_control) control_signal = max_control; // bounds checking
          else if (control_signal <= min_control) control_signal = min_control;

          last_e = e;

          if(true){//< 10000){
          Serial.print("y:");Serial.print(y_sh);
          Serial.print(" err: ");Serial.print(e);
          Serial.print("  control:");Serial.print(control_signal);
          Serial.print("  i:"); Serial.print((ki)*_integral);
          Serial.print("  d:");Serial.println((kd)*derivative);  
          }


          servoypos = map(control_signal, -90,90, 0, 180);
          //servoypos = control_signal;

          //servox.write(servoxpos);
          servoy.write(servoypos);

          last_time = current_time;
    
        }

    }

  
}

//--------------------------------------------------------------

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
    case ENC_TYPE_UNKNOWN:
    default:
      Serial.println("Unknown");
      break;
  }
}

int shift_calc_avg(int buf[avgbufsize],int n){
  float avg = n;
  avg += buf[0];
  for (int i = 1; i < avgbufsize; i++){
    avg += buf[i];
    buf[i] = buf[i-1];
  }
  buf[0] = n;
  return (int)avg/avgbufsize;
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
