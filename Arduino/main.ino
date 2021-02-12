
#include <M5Core2.h>
#include <driver/i2s.h>
#include <Fonts/EVA_20px.h>
#include <stdio.h>

#include "Free_Fonts.h" // Include the header file attached to this sketch

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <EEPROM.h>

#include "Adafruit_SGP30.h"
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "SHT3X.h"

#include <WiFi.h>
#include <Separador.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>



#define NET_NAME_LENGTH       64 
#define EEPROM_SSID_ADR       0*NET_NAME_LENGTH
#define EEPROM_PASS_ADR       1*NET_NAME_LENGTH
#define EEPROM_ADC_OFF        2*NET_NAME_LENGTH

#define EEPROM_CAL_LIM        EEPROM_ADC_OFF + 10
#define EEPROM_HUM_LIM        EEPROM_ADC_OFF + 20
#define EEPROM_TEMP_LIM       EEPROM_ADC_OFF + 30
#define EEPROM_CO2_LIM        EEPROM_ADC_OFF + 40


#define EEPROM_HOST_ADR       3*NET_NAME_LENGTH

char ssid[NET_NAME_LENGTH] = "DIAVERUM";            // your network SSID (name)
char pass[NET_NAME_LENGTH] = "barracas448";        // your network password
char host[NET_NAME_LENGTH] = "funginet.000webhostapp.com";        // your network password


//*********************************
//********* BLUETOOTH *************
//*********************************

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID                "1a47419f-febc-4c54-9c3d-066c85158df1" // UART service UUID
#define CHARACTERISTIC_UUID_RX      "d1ce0b77-ca94-4073-9934-dec9058d33f7"
#define CHARACTERISTIC_UUID_TX      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define CHARACTERISTIC_UUID_TX_TEMP "671f67f2-30d4-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_UUID_TX_HUM  "4a32de02-30d5-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_UUID_TX_CO2  "5aaca66e-30d5-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_SWX_TX_CO2   "6a2a4ae2-30d5-11eb-adc1-0242ac120002"




//*********************************
//*********** CONFIG **************
//*********************************


#define ADCPIN G35     // what pin we're connected to
#define MECANICO G13     // what pin we're connected to
#define OPTICO   G14     // what pin we're connected to

#define RELAY_CALEFACTOR    0     // Relay del Calefactor
#define RELAY_HUMIDIFICADOR 1     // Relay del Humidificador
#define RELAY_VENTILADOR    2     // Relay del Ventilador
#define RELAY_ENFRIADOR     3     // Relay del Enfriador

#define CYCLE_CALEFACTOR    2*60*1000 //2min
#define CYCLE_HUMIDIFICADOR 2*60*1000 //2min
#define CYCLE_VANTILADOR    2*60*1000 //2min
#define CYCLE_ENFRIADOR     2*60*1000 //2min



//*********************************
//*********** BUTTON **************
//*********************************
TouchButton BtnSW1 = TouchButton(10, 50, 80, 80, "SW1");
TouchButton BtnSW2 = TouchButton(90, 50, 80, 80, "SW2");
TouchButton BtnSW3 = TouchButton(170,50, 80, 80, "SW3");
TouchButton BtnSW4 = TouchButton(250,50, 80, 80, "SW4");



unsigned int LCD_BG=TFT_BLACK;
unsigned int TBI_BG=0x5c5c;
byte LCDTIMER;
//estos datos deben estar configurador tambiÃ©n en las constantes de tu panel
// NO USES ESTOS DATOS PON LOS TUYOS!!!!
String serial_number = "111";
const String insert_password = "121212";
const String get_data_password = "232323";
const char*  server = "funginet.000webhostapp.com";

//MQTT
const char *mqtt_server = "ioticos.org";
const int mqtt_port = 8883;

//no completar, el dispositivo se encargarÃ¡ de averiguar quÃ© usuario y quÃ© contraseÃ±a mqtt debe usar.
char mqtt_user[20] = "";
char mqtt_pass[20] = "";

const int expected_topic_length = 26;

//WiFiManager wifiManager;
WiFiClientSecure client;
PubSubClient mqttclient(client);
WiFiClientSecure client2;

Separador s;


struct {
  
  unsigned int WIFICON:1;
  unsigned int TOPIC:1;
  unsigned int VCIN:1;
  unsigned int LCDON:1;
  unsigned int PAGE:2;
  unsigned int GOTMYIP:1;
  unsigned int SSIDSET:1;
  unsigned int PASSSET:1;
  unsigned int NEWSSIDSET:1;
  unsigned int NEWPASSSET:1;
  unsigned int NETSAVED:1;
  unsigned int BTSEND:1;
  unsigned int PT100Cal:1;
  unsigned int RMEC:1;
  unsigned int ROP:1;
  unsigned int NEWHOST:1;
  unsigned int SSIDBTSEND:1;
  unsigned int PASSBTSEND:1;
  unsigned int HOSTBTSEND:1;
  unsigned int NEWLIMIT:1;
  
  
  
}ProgFlags;

//************************************
//***** DECLARACION FUNCIONES ********
//************************************
bool get_topic(int length);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void send_mqtt_data();
void send_to_database();

uint32_t chipId = 0;

//*************************************
//********      GLOBALS         *******
//*************************************
char device_topic_subscribe [40]="";
char device_topic_publish_data [40]="";
char device_topic_publish_limits [40]="";

char msg[45];
float temp = 0;
float hum = 0;
float co2 = 0;
float ReadValue=0;
long TimeToPublish = 0;
long TimeToPublishBT = 0;

long TimeToDataBase = 0;
long TimeToReConnect = 0;
long TimeToRead = 0;
long TimeToConnectMQTT=0;

long TimerCalefactor;
long TimerEnfriador;
long TimerHumidificador;
long Timerventilador;


byte DTCY_Calefactor=50;
byte DTCY_Enfriador=50;
byte DTCY_Humidificador=50;
byte DTCY_Ventilador=50;



byte sw1 = 0;
byte sw2 = 0;
byte sw3 = 0;
byte sw4 = 0;
byte swx = 0;

int maxtemp = 36;
int mintemp = 20;
int minhum = 70;
int maxco2 = 1000;

long TimeOpticOn = 0;
long TimeOpticOff = 1000;
long DTCYON=600;
long POW=0;

#define DTCYTOT 2000
#define PT100BUFF 10
byte Batery;

Adafruit_SGP30 CO2;
SHT3X sht30;
Adafruit_BMP280 bme;
  
String MyIp="";
int PT100Read[PT100BUFF];
int PT100Offset=0;
byte PT100Ptr=0;
float PT100Temp=0;
float PT100FinalTemp=0;

unsigned int EfectiveTimeSec=0;
unsigned int caltimer=0;

//*********************************
//********* BLUETOOTH *************
//********* CallBacks *************
//*********************************

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      String inputString="";
      if (rxValue.length() > 0) {
        Serial.println("****_________________________________*****");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){
          Serial.print(rxValue[i]);
          inputString+=char(rxValue[i]);
        }
      }
      
      Serial.print("INPUTSTRING     ");
      Serial.println(inputString);
      Serial.println("****_________________________________*****");
            
      if (inputString.startsWith("ssid[")) {      //Set SSID
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        inputString=(inputString.substring(index+1));
        if(inputString.length()>0)
        {
          Serial.println("SSIDSET OK");
          Serial.println(inputString);
          inputString.toCharArray(ssid,NET_NAME_LENGTH);
          ProgFlags.SSIDSET=1;
          ProgFlags.NEWSSIDSET=1;
        }
        else
        {
          Serial.println("SSIDSET FAL");
          ProgFlags.SSIDSET=0;
          
        }
      }
      if (inputString.startsWith("pass[")) {      //Set pass
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        inputString=(inputString.substring(index+1));
        if(inputString.length()>0)
        {
          Serial.println("PASSSET OK");
          Serial.println(inputString);
          inputString.toCharArray(pass,NET_NAME_LENGTH);
          ProgFlags.PASSSET=1;
          ProgFlags.NEWPASSSET=1;
       
        }
        else
        {
          Serial.println("PASSSET FAL");
          ProgFlags.PASSSET=0;
         
        }
      }
      if (inputString.startsWith("host[")) {      //Set pass
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        inputString=(inputString.substring(index+1));
        if(inputString.length()>0)
        {
          Serial.println("PASSSET OK");
          Serial.println(inputString);
          inputString.toCharArray(host,NET_NAME_LENGTH);
          ProgFlags.NEWHOST=1;
          
        }
        else
        {
          Serial.println("PASSSET FAL");
          ProgFlags.PASSSET=0;
         
        }
      }
      if (inputString.startsWith("sw1[")) {      //Set SW1
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        sw1=(inputString.substring(index+1)).toInt();
        Serial.print("SW1_DSET TO");
        Serial.println(sw1);
        ProgFlags.BTSEND=1;
      }
      if (inputString.startsWith("sw2[")) {      //Set SW2
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        sw2=(inputString.substring(index+1)).toInt();
        Serial.print("SW2_DSET TO");
        Serial.println(sw2);
        ProgFlags.BTSEND=1;
      }
      if (inputString.startsWith("sw3[")) {      //Set SW3
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        sw3=(inputString.substring(index+1)).toInt();
        Serial.print("SW3_DSET TO");
        Serial.println(sw3);
        ProgFlags.BTSEND=1;
      }
      if (inputString.startsWith("sw4[")) {      //Set SW4
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        sw4=(inputString.substring(index+1)).toInt();
        Serial.print("SW4_DSET TO");
        Serial.println(sw4);
        ProgFlags.BTSEND=1;
      }
      if (inputString.startsWith("call[")) {      //Set SW2
        int index=inputString.lastIndexOf("]OK");
        inputString.remove(index);
        index=inputString.lastIndexOf("[");
        int resp=(inputString.substring(index+1)).toInt();
        if(resp==1)
          ProgFlags.SSIDBTSEND=1;
        if(resp==2)
          ProgFlags.PASSBTSEND=1;
        if(resp==3)
          ProgFlags.HOSTBTSEND=1;
        
        Serial.print("CALL REC ");
        Serial.println(resp);
      }
      
      Serial.println("*********");
      Serial.println(inputString);
      Serial.println("*********");
    
    }
      
    
};



void setup() {

  // Initialize the M5Stack object

  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
    
  // LCD display
  ProgFlags.WIFICON=0;
  ProgFlags.TOPIC=0;
  ProgFlags.NETSAVED=0;
  ProgFlags.BTSEND=0;
  ProgFlags.PT100Cal=1;
  ProgFlags.NEWLIMIT=1;
  EEPROM.begin(512);
  M5.begin(true,true,true,true);
  M5.Lcd.fillScreen(LCD_BG);
  M5.Lcd.setTextColor(TFT_WHITE, LCD_BG);
  M5.Lcd.drawString("INICIANDO", 10, 10, 4);
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  serial_number=String(chipId);  
 
  delay(1000);
 
  
  M5.Lcd.drawString("Sensor CO2", 10, 30, 4);
  if (! CO2.begin()){
    M5.Lcd.drawString("FAIL", 220, 30, 4);
  }
  else
  {
    M5.Lcd.drawString("OK", 220, 30, 4);
  }
  M5.Lcd.drawString("Wifi", 10, 50, 4);
  delay(1000);
  
  M5.Lcd.drawString("Sensor T-H", 10, 50, 4);
  if(!bme.begin(0x76)){  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    M5.Lcd.drawString("FAIL", 220, 50, 4);
  }
  else
  {
    M5.Lcd.drawString("OK", 220, 50, 4);
  }
  M5.Lcd.drawString("Wifi", 10, 70, 4);
  delay(1000);
  
  Read_String(EEPROM_SSID_ADR, ssid);
  Read_String(EEPROM_PASS_ADR, pass);
  Read_String(EEPROM_HOST_ADR, host);
  Serial.println("Connecting...");
  WiFi.begin(ssid, pass);

  TimeToReConnect=millis();
  while(WiFi.status() != WL_CONNECTED){
    if (millis() - TimeToReConnect > 10000){
      ProgFlags.WIFICON=0;
      break;
    }
    if(WiFi.status() == WL_CONNECTED){
      ProgFlags.WIFICON=1;
      break;
    }
      
  }
  if(ProgFlags.WIFICON)
    M5.Lcd.drawString("OK", 220, 70, 4);
  else
    M5.Lcd.drawString("FAIL", 220, 70, 4);
  
  ProgFlags.VCIN=M5.Axp.isACIN();
  ProgFlags.LCDON=0;
  ProgFlags.PAGE=1;
  
  //client2.setCACert(web_cert);
  if(ProgFlags.WIFICON){
    M5.Lcd.drawString("Topico", 10, 90, 4);
    while(!ProgFlags.TOPIC){
      ProgFlags.TOPIC = get_topic(expected_topic_length);
    }
    M5.Lcd.drawString("OK", 220, 90, 4);
    
    
    M5.Lcd.drawString("MQTT", 10, 110, 4);
    //set mqtt cert
    //client.setCACert(mqtt_cert);
    mqttclient.setServer(mqtt_server, mqtt_port);
    mqttclient.setCallback(callback);
    M5.Lcd.drawString("OK", 220, 110, 4);
    M5.Lcd.drawString("SN: ", 10, 130, 4);
    M5.Lcd.drawString(String(chipId), 100, 130, 4);
    Serial.print("Chip ID: ");
    Serial.println(chipId);
    M5.Lcd.drawString("ONLINE", 10, 150, 4);
  
  }
 
//*********************************
//********** BLUETOOTH ************
//*********** SETUP ***************
  
  String DEV="Device_"+serial_number;
  char buff[30];
  DEV.toCharArray(buff,30);
  
 
  BLEDevice::setMTU(128); // debug with larger MTU size
  BLEDevice::init(buff);                              // Create the BLE Device
 
  BLEDevice::setMTU(128); // debug with larger MTU size
  pServer = BLEDevice::createServer();                          // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);  // Create the BLE Service
  pTxCharacteristic = pService->createCharacteristic(           // Create a BLE Characteristic
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_READ
                      );
  pTxCharacteristic->addDescriptor(new BLE2902());
  
  
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                      );
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();                                            // Start the service
  pServer->getAdvertising()->start();                           // Start advertising  
  Serial.println("Waiting a client connection to notify...");

  
  
  
  M5.Axp.ScreenBreath(15);
  M5.Lcd.fillScreen(LCD_BG);
  M5.Lcd.fillRect(1,1,320,20,TBI_BG);
  pinMode(MECANICO, OUTPUT);
  pinMode(OPTICO, OUTPUT);
  
  RelayBoard(MECANICO, false);
  RelayBoard(OPTICO, false);
  Read_Int(EEPROM_ADC_OFF,PT100Offset);

  readRelayReg(0x10);
  readRelayReg(0x11);
  WriteRelayReg(0x10,1);
  WriteRelayReg(0x11,0);
  
  Read_Int(EEPROM_CAL_LIM,maxtemp);
  Read_Int(EEPROM_HUM_LIM,mintemp);
  Read_Int(EEPROM_TEMP_LIM,minhum);
  Read_Int(EEPROM_CO2_LIM,maxco2);
  TimerCalefactor=millis();
  TimerEnfriador=millis();
  TimerHumidificador=millis();
  Timerventilador=millis();


  
  delay(1000);
  
  
}
void loop() {
  if (WiFi.status() != WL_CONNECTED)
  {
    ProgFlags.TOPIC=0;
    ProgFlags.GOTMYIP=0;
    if((ProgFlags.PASSSET)&&(ProgFlags.SSIDSET)){
      if (millis() - TimeToReConnect > 60000)
      { 
        TimeToReConnect=millis();
        ProgFlags.WIFICON=0;
        
        if((ProgFlags.NEWPASSSET)&&(ProgFlags.NEWSSIDSET)){
          Serial.println("Connecting NewSSID...");
        
        }
        else
        {
          Read_String(EEPROM_SSID_ADR, ssid);
          Read_String(EEPROM_PASS_ADR, pass);
          Read_String(EEPROM_HOST_ADR, host);
  
          Serial.println("Connecting OLD...");
        }
        WiFi.begin(ssid, pass);
      }
    }
  }
  else
  {
    ProgFlags.WIFICON=1;
    if(!ProgFlags.NETSAVED){
      Save_String(EEPROM_SSID_ADR, ssid);
      Save_String(EEPROM_PASS_ADR, pass);
      ProgFlags.NETSAVED=1;
      
    }
    if(ProgFlags.NEWHOST){

      Save_String(EEPROM_HOST_ADR, host);
      ProgFlags.NEWHOST=0;
    }
  }
  if(!ProgFlags.WIFICON){
  }
  else
  {
    
    if(!ProgFlags.TOPIC){
      ProgFlags.TOPIC = get_topic(expected_topic_length);
   
    }
    else
    {
      mqttclient.setServer(mqtt_server, mqtt_port);
      mqttclient.setCallback(callback);
      if(!ProgFlags.GOTMYIP)
      {
        if(GetExternalIP())
          ProgFlags.GOTMYIP=1;
      }
      if (millis() - TimeToConnectMQTT > 5000){
        TimeToConnectMQTT = millis();
        if (!client.connected()) {
          reconnect();
        }
      }
      if (millis() - TimeToDataBase > 60000){
        TimeToDataBase = millis();
        send_to_database();
          
      }
      if (millis() - TimeToPublish > 3000){
        TimeToPublish = millis();
    
    
        if(mqttclient.connected()){
          String to_send =String((int)temp) + "," + String((int)hum) + ","+ String((int)co2) + ","+ String((int)Batery) + "," + String(sw1)+","+ String(sw2)+","+ String(sw3)+","+ String(sw4);
        //  Serial.print("SENDING  Data MQTT");
        //  Serial.println(to_send);
          to_send.toCharArray(msg,30);
          mqttclient.publish(device_topic_publish_data,msg);
          if(ProgFlags.NEWLIMIT)
          {
            to_send = String((int)maxtemp) + "," + String((int)mintemp) + ","+ String((int)minhum) + ","+ String((int)maxco2);
            Serial.print("SENDING  Limits MQTT");
            Serial.println(to_send);
            to_send.toCharArray(msg,30);
            mqttclient.publish(device_topic_publish_limits,msg);
            ProgFlags.NEWLIMIT=0;
          }
          
          
        }
      }
      mqttclient.loop();
             
    }
  }

  if(ProgFlags.BTSEND){
    SendSwxBT();
    ProgFlags.BTSEND=0;
  }
  if(ProgFlags.SSIDBTSEND){
    SendSSIDBT();
    ProgFlags.SSIDBTSEND=0;
  }
 
  if (millis() - TimeToPublishBT > 2000){
    TimeToPublishBT = millis();
    ProgFlags.BTSEND=1;
  }
  if(ProgFlags.LCDON){                     //Si el bit LCDON esta activo imprime en pantalla
    ShowTopBar();
    if(ProgFlags.PAGE==1)
    {
        ShowSensorValues(10,10);
    }
    if(ProgFlags.PAGE==2)
    {
        ShowActuatorsValues(10,10);
    }
    if(ProgFlags.PAGE==3)
    {
        ShowActuatorsPasteur();
    
    }
  } 
  if (millis() - TimeToRead > 1000){
    TimeToRead = millis();
    if(ProgFlags.VCIN){                    //Si esta conectado a VCC Usa la pantalla
      LCDTIMER=60;
 
      if(!ProgFlags.LCDON){
        M5.Lcd.wakeup();
        ProgFlags.LCDON=1;
       M5.Axp.ScreenBreath(15); 
      Serial.println("BRILLO 255");
      }
    }
    else{                               //Si no esta conectado a VCC a los 60 segundos se apaga
      M5.Axp.ScreenBreath(11);
      Serial.println("BRILLO 20");
 
      if(LCDTIMER)
      {
        LCDTIMER--;
      }
      else
      {
        M5.Lcd.sleep();
        M5.Axp.ScreenBreath(7);
        Serial.println("BRILLO 0");
 
        ProgFlags.LCDON=0;
      }    
    }
    if(sht30.get()==0){
   
    ReadValue =sht30.humidity;
    if(SensorTrueValue(ReadValue,100.0,0.0))
      hum=ReadValue;
    
    ReadValue = sht30.cTemp;
    if(SensorTrueValue(ReadValue,150.0,-20.0))
      temp=ReadValue;
    }
    else
    {
  //    Serial.println("TEMP HUM Measurement failed");
      hum =random(0,100);
      temp =30;
      //temp =random(20,40);
      
    }
    if (! CO2.IAQmeasure()) {
      co2=random(1000,3000);
  //    Serial.println("CO2 Measurement failed");
    }
    else
    {
      co2=CO2.eCO2;
    }
    
    PT100Temp=PT100Temp-PT100Read[PT100Ptr];
    PT100Read[PT100Ptr] = analogRead(ADCPIN)+PT100Offset;
    PT100Temp=PT100Temp+PT100Read[PT100Ptr];
    PT100Ptr++;
    if( PT100FinalTemp>80)
    {
      EfectiveTimeSec++;
    }
    if(PT100Ptr>=PT100BUFF)
    {
      PT100Ptr=0;
      if(!ProgFlags.PT100Cal)
      {
        ProgFlags.PT100Cal=1;
        Serial.println("CALIBRANDO______________________________________________________");
        float PT100_Volt=PT100Temp/(float)40950.0*(float)3.3;
        Serial.print("PT100 Volt:  ");
        Serial.println(PT100_Volt,5);
        PT100_Volt=((float)0.27315+(float)0.022)-PT100_Volt;
        Serial.print("PT100 DIF :  ");
        Serial.println(PT100_Volt,5);
        PT100_Volt=PT100_Volt*(float)4095.0/(float)3.3;
        Serial.print("PT100 AD_D :  ");
        Serial.println(PT100_Volt,5);
        PT100Offset=PT100_Volt;
        Save_Int(EEPROM_ADC_OFF,PT100Offset);
      }
     
    }
    //Serial.print("PT100Read:  ");
    //Serial.println(PT100Temp/10);
    PT100FinalTemp=PT100Temp/(float)40950.0*(float)3.3;
    //Serial.print("PT100 Volt:  ");
    //Serial.println(PT100FinalTemp,5);
    
    PT100FinalTemp=PT100FinalTemp-(float)0.27315;
    //Serial.print("PT100 CELS:  ");
    //Serial.println(PT100FinalTemp,5);
        
    
    PT100FinalTemp=(PT100FinalTemp*1000);

    //Serial.print("PT100Temp:  ");
    //Serial.println(PT100FinalTemp);
    if(ProgFlags.PAGE==3)
    {
      LCDTIMER=60;
      if(PT100Ptr==0)
      {
        
        if( PT100FinalTemp<=80)
        {
           RelayBoard(MECANICO, true);
           if(DTCYON<DTCYTOT)
           {
            DTCYON=DTCYON+20;
           }
        }
        if( PT100FinalTemp>=83)
        {
          RelayBoard(MECANICO, false);
          if(DTCYON>20)
          {
            DTCYON=DTCYON-20;
          }
        }
      }  
      if (millis() - TimeOpticOn > DTCYON){
        TimeOpticOn=millis();
        if(ProgFlags.RMEC)
          POW=POW+DTCYON;
        RelayBoard(OPTICO, false);
      }
      if (millis() - TimeOpticOff > DTCYTOT){
        TimeOpticOff=millis();
        RelayBoard(OPTICO, true);
        
      }
    }
  } 
  
  M5.update();
  if ( M5.BtnA.wasPressed() ) {
  Btna_fcn();
  }
  if ( M5.BtnB.wasPressed() ) {
    LCDTIMER=60;
    caltimer=0;
    DTCYON=600;
    POW=0;
    M5.Lcd.wakeup();
    M5.Axp.ScreenBreath(15); 
    Serial.println("BRILLO 255");
    RelayBoard(MECANICO, false);
    EfectiveTimeSec=0;
    ProgFlags.LCDON=1;
    ProgFlags.PAGE=2;   
    M5.Lcd.fillRect(1,17,320,223,LCD_BG);
  }
  
  if ( M5.BtnC.wasPressed() ) {
    LCDTIMER=60;
    M5.Lcd.wakeup();
    M5.Axp.ScreenBreath(15); 
    Serial.println("BRILLO 255");
    if(ProgFlags.PAGE==3)
    {
      caltimer++;
      Serial.print("CALTIME   ______________   ");
      Serial.println(caltimer);
      if(caltimer>10)
      {
        caltimer=0;
        ProgFlags.PT100Cal=0;
        PT100Ptr=0;
        PT100Temp=0;
        PT100Offset=0;
        for(byte a=0;a<PT100BUFF;a++)
        {
          PT100Read[a]=0;
        }
      }
    }
    
    EfectiveTimeSec=0;
    ProgFlags.LCDON=1;
    ProgFlags.PAGE=3;   
    M5.Lcd.fillRect(1,17,320,223,LCD_BG);
  }
  
  if(ProgFlags.PAGE==2)  {
    if(BtnSW1.wasPressed())
    {
      Serial.println("--- BtnSW1_fn BUTTON WAS PRESSED ---");
      sw1=!sw1;
      WriteRelayNumber(RELAY_CALEFACTOR, sw1); 
      ProgFlags.BTSEND=1;
    }
    if(BtnSW2.wasPressed())
    {
      Serial.println("--- BtnSW2_fn BUTTON WAS PRESSED ---");
      sw2=!sw2;
      WriteRelayNumber(RELAY_HUMIDIFICADOR, sw2); 
      ProgFlags.BTSEND=1;
    }
    if(BtnSW3.wasPressed())
    {
      Serial.println("--- BtnSW3_fn BUTTON WAS PRESSED ---");
      sw3=!sw3;
      WriteRelayNumber(RELAY_VENTILADOR, sw3); 
      ProgFlags.BTSEND=1;
    }
    if(BtnSW4.wasPressed())
    {
      Serial.println("--- BtnSW4_fn BUTTON WAS PRESSED ---");
      sw4=!sw4;
      WriteRelayNumber(RELAY_ENFRIADOR, sw4); 
      ProgFlags.BTSEND=1;
    }
  }
 
  if (!deviceConnected && oldDeviceConnected) {             // disconnecting
      delay(500);                                           // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising();                          // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {             // connecting
  
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
//--------------------------------------------------controls-----------------------------
 
  if(temp<mintemp){
    if(millis()<TimerCalefactor+((CYCLE_CALEFACTOR * DTCY_Calefactor)/100)){
      swx=1;
    } else {
      swx=0;
    }
    if(millis()>=TimerCalefactor+CYCLE_CALEFACTOR){
      if(temp<mintemp){
        if(DTCY_Calefactor<=95) {
          DTCY_Calefactor+=5;
        }
      } else {
        if(DTCY_Calefactor>=5){
          DTCY_Calefactor-=5;
        }
      }
      Serial.println("Fin Calefactor ");
      TimerCalefactor=millis();
    }
  } else {
    TimerCalefactor=millis();
    swx=0;
  }
  if(swx!=sw1)
  {
    sw1=swx;
    WriteRelayNumber(RELAY_CALEFACTOR, sw1); 
    Serial.println("CALEFACTOR " + String(sw1));
  }
 
  if(temp>maxtemp){
    if(millis()<TimerEnfriador+((CYCLE_ENFRIADOR * DTCY_Enfriador)/100)){
      swx=1;
    } else {
      swx=0;
    }
    if(millis()>=TimerEnfriador+CYCLE_ENFRIADOR){
      if(temp>maxtemp){
        if(DTCY_Enfriador<=95) {
          DTCY_Enfriador+=5;
        }
      } else {
        if(DTCY_Enfriador>=5){
          DTCY_Enfriador-=5;
        }
      }
      Serial.println("Fin Enfriador ");
      TimerEnfriador=millis();
    }
  } else {
    TimerEnfriador=millis();
    swx=0;
  }
  if(swx!=sw4)
  {
    sw4=swx;
    WriteRelayNumber(RELAY_ENFRIADOR, sw4); 
    Serial.println("ENFRIADOR " + String(sw4));
  }
  
  ProgFlags.VCIN=M5.Axp.isACIN();
 
}

//************************************
//*********** FUNCIONES **************
//************************************

void Btna_fcn(void)
{
  LCDTIMER=60;
  caltimer=0;
  DTCYON=600;
  POW=0;
  M5.Lcd.wakeup();
  M5.Axp.ScreenBreath(15); 
  Serial.println("BRILLO 255");
  RelayBoard(MECANICO, false);
  EfectiveTimeSec=0;
  ProgFlags.LCDON=1;
  ProgFlags.PAGE=1;   
  M5.Lcd.fillRect(1,17,320,223,LCD_BG);
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
	Serial.print("Mensaje recibido desde topico -> ");
	Serial.print(topic);
	Serial.println("");
	for (int i = 0; i < length; i++) {
		incoming += (char)payload[i];
	}
	incoming.trim();
	Serial.println("Mensaje -> " + incoming);

  String str_topic = String(topic);
  String command = s.separa(str_topic,'/',3);
  Serial.println(command);

  if(command=="sw1"){
    Serial.println("Sw1 pasa a estado " + incoming);
    sw1 = incoming.toInt();
    WriteRelayNumber(RELAY_CALEFACTOR, sw1); 
  }

  if(command=="sw2"){
    Serial.println("Sw2 pasa a estado " + incoming);
    sw2 = incoming.toInt();
    WriteRelayNumber(RELAY_HUMIDIFICADOR, sw2); 
  }

 if(command=="sw3"){
    Serial.println("Sw3 pasa a estado " + incoming);
    sw3 = incoming.toInt();
    WriteRelayNumber(RELAY_VENTILADOR, sw3); 
  }
 
 if(command=="sw4"){
    Serial.println("Sw4 pasa a estado " + incoming);
    sw4 = incoming.toInt();
    WriteRelayNumber(RELAY_ENFRIADOR, sw4); 
  }

  if(command=="slider"){
    Serial.println("Slider pasa a estado " + incoming);
    maxtemp = incoming.toInt();
    Save_Int(EEPROM_CAL_LIM,maxtemp);
    ProgFlags.NEWLIMIT=1;
  }
  if(command=="slider1"){
    Serial.println("Slider1 pasa a estado " + incoming);
    mintemp = incoming.toInt();
    Save_Int(EEPROM_HUM_LIM,mintemp);
    ProgFlags.NEWLIMIT=1;
  }
  if(command=="slider2"){
    Serial.println("Slider2 pasa a estado " + incoming);
    minhum = incoming.toInt();
    Save_Int(EEPROM_TEMP_LIM,minhum);
    ProgFlags.NEWLIMIT=1;
  }
  if(command=="slider3"){
    Serial.println("Slider3 pasa a estado " + incoming);
    maxco2 = incoming.toInt();
    Save_Int(EEPROM_CO2_LIM,maxco2);
    ProgFlags.NEWLIMIT=1;
  }
  
  ProgFlags.BTSEND=1;
      
}

void reconnect() {

//	while (!mqttclient.connected()) {
		Serial.print("Intentando conexion MQTT SSL");
		// we create client id
		String clientId = "esp32_ia_";
		clientId += String(random(0xffff), HEX);
		// Trying SSL MQTT connection
		if (mqttclient.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
			Serial.println("Connected!");
			// We subscribe to topic

			mqttclient.subscribe(device_topic_subscribe);

		} else {
			Serial.print("fallo :( con error -> ");
			Serial.print(mqttclient.state());
			Serial.println(" Intentamos de nuevo en 5 segundos");

		}
//	}
}

//funciÃ³n para obtener el tÃ³pico perteneciente a este dispositivo
bool get_topic(int length){

  Serial.println("\nIniciando conexion segura para obtener topico raiz...");

  if (!client2.connect(server, 443)) {
    Serial.println("Fallo conexion!");
  }else {
    Serial.println("Conectados a servidor para obtener topico - ok");
    // Make a HTTP request:
    String data = "gdp="+String(get_data_password)+"&sn="+String(serial_number)+"\r\n";
    String HTML=(String("POST ") + "/app/getdata/gettopics" + " HTTP/1.1\r\n" +\
                 "Host: " + server + "\r\n" +\
                 "Content-Type: application/x-www-form-urlencoded"+ "\r\n" +\
                 "Content-Length: " + String (data.length()) + "\r\n\r\n" +\
                 data );
    client2.print(HTML);

    Serial.println("Solicitud enviada - ok");

    while (client2.connected()) {
      String line = client2.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers recibidos - ok");
        break;
      }
    }

    String line;
    while(client2.available()){
      line += client2.readStringUntil('\n');
    }
    String temporal_topic = s.separa(line,'#',1);
    String temporal_user = s.separa(line,'#',2);
    String temporal_password = s.separa(line,'#',3);



    Serial.println("El topico es: " + temporal_topic);
    Serial.println("El user MQTT es: " + temporal_user);
    Serial.println("La pass MQTT es: " + temporal_password);
    Serial.println("La cuenta del topico obtenido es: " + String(temporal_topic.length()));

    if (temporal_topic.length()==length){
      Serial.println("El largo del topico es el esperado: " + String(temporal_topic.length()));

      String temporal_topic_subscribe = temporal_topic + "/actions/#";
      temporal_topic_subscribe.toCharArray(device_topic_subscribe,40);
      Serial.println(device_topic_subscribe);
      String temporal_topic_publish = temporal_topic + "/data";
      temporal_topic_publish.toCharArray(device_topic_publish_data,40);
      
      temporal_topic_publish = temporal_topic + "/limits";
      temporal_topic_publish.toCharArray(device_topic_publish_limits,40);
      
      temporal_user.toCharArray(mqtt_user,20);
      temporal_password.toCharArray(mqtt_pass,20);

      client2.stop();
      return true;
    }else{
      client2.stop();
      return false;
    }

  }
}


void send_to_database(){

  Serial.println("Iniciando conexion segura para enviar a base de datos...");

  if (!client2.connect(server, 443)) {
    Serial.println("Fallo conexion!");
  }else {
    Serial.println("Conectados a servidor para insertar en db - ok");
    // Make a HTTP request:
    
    String data = "idp="+insert_password+"&sn="+serial_number+"&temp="+String(temp)+"&hum="+String((int)hum)+"&co2="+String((int)co2)+"&ip="+MyIp;
    Serial.print("POST DATA ");
    Serial.println(data);
    String HTML=(String("POST ") + "/app/insertdata/insert" + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +\
                 "Accept: *" + "/" + "*\r\n" +\
                 "Content-Type: application/x-www-form-urlencoded"+ "\r\n" +\
                 "Content-Length: " + String (data.length()) + "\r\n\r\n" +\
                 data );

    client2.print(HTML);
    Serial.println("Solicitud enviada - ok");

    while (client2.connected()) {
      String line = client2.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers recibidos - ok");
        Serial.println(line);
        break;
      }
    }
    
    
    String line;
    while(client2.available()){
      line += client2.readStringUntil('\n');
    }
    Serial.println("Response  ");
    Serial.println(line);
    client2.stop();

    }

  }
void ShowSensorValues(short int X, short int Y)
{
  M5.Lcd.setTextColor(TFT_WHITE, LCD_BG);

  M5.Lcd.drawString(String ((int)temp), 10, 30, 8);
  M5.Lcd.drawString("C", 120, 80, 4);
  M5.Lcd.drawString("Temperatura", 40, 110, 2);
  
  M5.Lcd.drawString(String ((int)hum), 160, 30, 8);
  M5.Lcd.drawString("%", 270, 80, 4);
  M5.Lcd.drawString("Humedad", 180, 110, 2);

  M5.Lcd.drawString(String ((int)co2), 10, 130, 8);
  M5.Lcd.drawString("ppm", 250, 160, 4);
  M5.Lcd.drawString("CO2", 150, 220, 2);

}
void ShowTopBar(void){
  
  M5.Lcd.setTextColor(WHITE, TBI_BG);
  Batery=CheckBat();
  String bat=String(Batery)+" %";
  M5.Lcd.drawString(bat, 280,3, 2);
  M5.Lcd.fillRect(265,5,9,12,WHITE);
  M5.Lcd.fillRect(267,3,5,2,WHITE);
  bat="SN: "+String(chipId);
  M5.Lcd.drawString(bat, 3,3, 2);
  
}
bool SensorTrueValue(float ReadValue,float HL,float LL)
{
  if((ReadValue<=HL)&&(ReadValue>=LL))
    return true;
  else
    return false;
  
}

int CheckBat(void){
  float batVoltage = M5.Axp.GetBatVoltage();
  float batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;
  return (int)batPercentage;
}

void ShowActuatorsValues(short int X, short int Y)
{
  M5.Lcd.setTextColor(TFT_WHITE, LCD_BG);
  M5.Lcd.drawString("OUT1", 10, 50, 4);
  SwitchOnOff(20,80,sw1);
  M5.Lcd.drawString("OUT2", 90, 50, 4);
  SwitchOnOff(100,80,sw2);
  M5.Lcd.drawString("OUT3", 170, 50, 4);
  SwitchOnOff(180,80,sw3);
  M5.Lcd.drawString("OUT4", 250, 50, 4);
  SwitchOnOff(260,80,sw4);
  M5.Lcd.progressBar(10,120,300,20,(maxtemp*100)/255);  
 }
void SwitchOnOff(unsigned int X,unsigned int Y,bool ON)
{
  if(ON)
  {
    M5.Lcd.fillRect(X,Y,50,20,TBI_BG);
    M5.Lcd.fillRect(X+33,Y+3,14,14,WHITE);
  }
  else
  {
    M5.Lcd.fillRect(X,Y,50,20,0xDEDB);
    M5.Lcd.fillRect(X+3,Y+3,14,14,WHITE);
  }
    
}


bool GetExternalIP()
{
  WiFiClient client;
  if (!client.connect("api.ipify.org", 80)) {
    Serial.println("Failed to connect with 'api.ipify.org' !");
  }
  else {
    int timeout = millis() + 5000;
    client.print("GET /?format=json HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");
    while (client.available() == 0) {
      if (timeout - millis() < 0) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return false;
      }
    }
    
    String line;
    while(client.available()){
      line += client.readStringUntil('\n');
    }
    if (line.startsWith("HTTP/1.1 200 OK")) {      //GET nets on field
        int index=line.lastIndexOf("\"");
        line.remove(index);
        index=line.lastIndexOf("\"");
        MyIp=line.substring(index+1);
        Serial.print("MYIP:  ");
        Serial.println(MyIp);
        Serial.println(line);
         
      }
    else
    {
      return false;
    }
  }
  return true;
}

bool Save_String(unsigned int ADR, char DATA[])
{
  for(int a=0;a<NET_NAME_LENGTH;a++){
    EEPROM.write(ADR+a, DATA[a]);
  }
  EEPROM.commit();
  Serial.print("EEPROM WRITE: ");
  Serial.println(DATA);
  return true;
}
bool Read_String(unsigned int ADR, char DATA[])
{
  for(int a=0;a<NET_NAME_LENGTH;a++){
    DATA[a]=EEPROM.read(ADR+a);
  }
  Serial.print("EEPROM READ:  ");
  Serial.println(DATA);
  return true;
}
bool Save_Int(unsigned int ADR, int &DATA)
{
  char buf[10] = {0};
  itoa((int)DATA, buf, 10);
   
  for(int a=0;a<10;a++){
    EEPROM.write(ADR+a, buf[a]);
  }
  EEPROM.commit();
  Serial.print("EEPROM WRITE INT: ");
  Serial.println(DATA);
  return true;
}
bool Read_Int(unsigned int ADR, int &DATA)
{
  char buf[10] = {0};
  Serial.print("EEPROM READ INT:  ");
  for(int a=0;a<10;a++){
    buf[a]=EEPROM.read(ADR+a);
 //   Serial.print(buf[a]);
  }
  delay(2000);
  DATA= atoi( buf);
  
  Serial.print("EEPROM READ RESULT:  ");
  Serial.println(DATA);
  return true;
}

void SendSwxBT(void)
{
  if (deviceConnected) {
    String OUT;
    char txString[60];
    OUT=String (co2);
    OUT+=","+String (hum);
    OUT+=","+String (temp);
    OUT+=","+String (sw4);
    OUT+=","+String (sw3);
    OUT+=","+String (sw2);
    OUT+=","+String (sw1);
    
    
    OUT.toCharArray(txString,60);
    
    Serial.print("SENDING  BT  ");
    pTxCharacteristic->setValue(txString);
    pTxCharacteristic->notify();
    Serial.println(txString);
  }
  
} 
void SendSSIDBT(void)
{
  if (deviceConnected) {
    
    Serial.print("SENDING  BT  ");
    pTxCharacteristic->setValue(ssid);
    pTxCharacteristic->notify();
    Serial.println(ssid);
  }
  else
  {
      Serial.println("SENDING  BT  NOT CONNECTED");
  
  }
} 
void ShowActuatorsPasteur()
{
  unsigned int horas,minutos,segundos;
  horas=EfectiveTimeSec/3600;
  minutos=(EfectiveTimeSec%3600)/60;
  segundos=(EfectiveTimeSec%3600)%60;
  
 
  M5.Lcd.setTextColor(TFT_WHITE, LCD_BG);
  String tiempo="";
  if(ProgFlags.PT100Cal)
  {
    if((PT100FinalTemp<150)&&(PT100FinalTemp>-9))
    {
      // temp=PT100FinalTemp;
      if(PT100FinalTemp<100)
        tiempo=tiempo+"  ";
      if(PT100FinalTemp<10)
        tiempo=tiempo+"  ";
      tiempo=tiempo+String ((int)PT100FinalTemp);
    }
    else
    {
          tiempo="---";
    }
  }
  else
  {
    tiempo="Cal";
  }
  M5.Lcd.drawString(tiempo, 40, 30, 8);
  M5.Lcd.drawString("C", 230, 80, 4);
  M5.Lcd.drawString("Temperatura", 110, 110, 2);

  tiempo=String(horas)+":";
  if(minutos<10)
    tiempo=tiempo+"0";
  tiempo=tiempo+String(minutos)+":";
  if(segundos<10)
    tiempo=tiempo+"0";
  tiempo=tiempo+String(segundos);
  
  M5.Lcd.drawString("TIEMPO", 10, 140, 4);
  M5.Lcd.drawString(tiempo, 120, 140, 4);

  tiempo=String((int)(DTCYON*100/DTCYTOT))+"  %    ";
  M5.Lcd.drawString("DTCY  ", 10, 170, 4);
  M5.Lcd.drawString(tiempo, 120, 170, 4);
  
  tiempo=String((int)(((POW/1000)*1750)/3600))+"  Watt";
  M5.Lcd.drawString("POW  ", 10, 200, 4);
  M5.Lcd.drawString(tiempo, 120, 200, 4);
  
  if(horas>=2)
    Btna_fcn();
  
      
}
void RelayBoard(unsigned int RELE,bool ESTADO)
{
  if(ESTADO==true)
    digitalWrite(RELE, HIGH);
  else
    digitalWrite(RELE, LOW);
  
  if(RELE==OPTICO)
    ProgFlags.ROP=ESTADO;
  if(RELE==MECANICO)
    ProgFlags.RMEC=ESTADO;
  
}

void WriteRelayReg( int regAddr, int data )
{
    Wire.beginTransmission(0x26);
    Wire.write(regAddr);
    Wire.write(data);
    Wire.endTransmission();
    Serial.printf("[ W ] %02X : %02X. \r\n", regAddr, data);
}

int readRelayReg(int regAddr)
{
    Wire.beginTransmission(0x26);
    Wire.write(regAddr);
    Wire.endTransmission();
    Wire.requestFrom(0x26, 1);
    int data = Wire.read() & 0x00ff;
    Serial.printf("[ R ] %02X : %02X. \r\n", regAddr, data);
    return data;
}

void WriteRelayNumber( int number, int state )
{
    int StateFromDevice = readRelayReg(0x11);
    if( state == 0 )
    {
        StateFromDevice &= ~( 0x01 << number );
    }
    else
    {
        StateFromDevice |= ( 0x01 << number );
    }
    WriteRelayReg(0x11,StateFromDevice);
}
