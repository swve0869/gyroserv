<<<<<<< HEAD:arduinofiles/gyro/gyroTCP/gyroTCP.ino
=======
/*
  HEY these are those changes!
  WiFi Web Server

 A simple web server that shows the value of the analog input pins.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * Analog inputs attached to pins A0 through A5 (optional)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 */
>>>>>>> 8118e70aa781bc9ca509c355e269f403eb37e52d:arduinofiles/gyro/gyro.ino

#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include <Servo.h>
#include "arduino_secrets.h" 

Servo servox;
Servo servoy;
int servoxpos = 0;
int servoxpin = 10;
int servoypos = 0;
int servoypin = 9;
const int avgbufsize = 10;
int xavgbuf[avgbufsize] = {90};
int yavgbuf[avgbufsize] = {90};


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
String rcvbuffer = String(80); 
String sndbuffer = String(80); 

WiFiServer server(80);

float x, y, z;
int angleX = 0;
int angleY = 0;
int degreesX = 0;
int degreesY = 0;
unsigned long previousMillis = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // init gyro
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // init servo
  servox.attach(10);
  servoy.attach(9);



  Serial.print("Accelerometer sample rate = ");

  Serial.print(IMU.accelerationSampleRate());

  Serial.println(" Hz");
  

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }


  // scan for existing networks:
/*   Serial.println("Scanning available networks...");
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
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      //Serial.print("?");
      if (client.available()) {
        
        IMU.readAcceleration(x, y, z);

        char c = client.read();
        Serial.write(c);
        if(c == '#'){
          rcvbuffer = "\0";
          while(1){
            c = client.read();
            if(c == '$'){
              break;
            }
            rcvbuffer += String(c);
          }
          //Serial.println(rcvbuffer);
        }
        
        if(rcvbuffer.equals("gyrorequest")){

          client.print('#');
          client.print(x);
          client.print(':');
          client.print(y);
          client.print(':');
          client.print(z);
          client.print(':');    
          client.print('$');
          
          Serial.print(x);
          Serial.print(" ");
          Serial.print(y);
          Serial.print(" ");
          Serial.println(z);
        }

        int servoxpos = map(x*100, -100,100, 0, 180);
        int servoypos = map(y*100, -100,100, 0, 180);
        //Serial.println(servoxpos);

        servoxpos = shift_calc_avg(xavgbuf,servoxpos);
        servoypos = shift_calc_avg(yavgbuf,servoypos);

        Serial.println(servoxpos);
        servox.write(servoxpos);
        servoy.write(servoypos);

        rcvbuffer = "\0";       
        delay(1);
      }
    }
    // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
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
