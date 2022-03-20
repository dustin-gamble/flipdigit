
 
#include <TimeLib.h>
//#include <CoinMarketCapApi.h>


//#include <Ethernet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
//#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ADC.h>
#include <ADC_util.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
// NTP Servers:
IPAddress timeServer(129, 6, 15, 28); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

elapsedMillis timer1;
elapsedMillis timer2;
elapsedMillis timer3;

const int timeZone = -7;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

int hour0;
int hour1;
int hour2;
int min1;
int min2;
int temp0;
int temp1;
int temp2;
int localtemp;
int bitcoin;
int bitcoin0;
int bitcoin1;
int bitcoin2;
int bitcoin3;
int bitcoin4;
int bitcoin5;
int bitcoin6;
int rawindoortemp;
int indoortemp;
int indoortempF;
int indoortemp0;
int indoortemp1;
int indoortemp2;
int dasher0;
int dasher;
int stockprice;




ADC *adc = new ADC(); // adc object;

  //StaticJsonDocument<200> doc;


EthernetClient client;
//CoinMarketCapApi api(client);

unsigned long api_mtbs = 600000; //mean time between api requests
unsigned long api_due_time = 0;

//weather
String apiKey= "a4dd1b6e82e9277997268b51497c001f"; 
//the city you want the weather for 
String location= "san luis obispo,us"; 
//int status = WL_IDLE_STATUS; 
char server1[] = "api.openweathermap.org"; 
char server2[] = "api.coindesk.com"; 
char server3[] = "api.cryptonator.com"; 
char server4[] = "api.binance.com"; 
char server5[] = "api.coindesk.com"; 
char server6[] = "www.alphavantage.co"; 
char serverstock[] = "api.marketstack.com";
char yahoo[] = "finance.yahoo.com";


#define CMC_API "/v1/ticker/bitcoin/"
#define CMC_URL "api.coinmarketcap.com"

const char* fingerprint = "EF 9D 44 BA 1A 91 4C 42 06 B1 6A 25 71 26 58 61 BA DA FA B9";
static char respBuffer[4096];
static char respBuffer2[4096];

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;

#define ARDUINOJSON_USE_DOUBLE 0

bool printWebData = true;  // set to false for better speed measurement


  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

//uint8_t mac[6];
uint8_t serial[4];
EthernetServer server(80);

void setup() 
{
  Serial.begin(115200);
  //while (!Serial) ; // Needed for Leonardo only

    //start wiznet 
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  delay(1);
  digitalWrite(9, HIGH);

  
pinMode(A1, INPUT);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->adc0->setReference(ADC_REFERENCE::REF_1V2); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->adc0->setAveraging(16); // set number of averages
    adc->adc0->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->adc0->enableCompare(1.0/3.3*adc->adc0->getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc->adc0->enableCompareRange(1.0*adc->adc0->getMaxValue()/3.3, 2.0*adc->adc0->getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->adc0->enableInterrupts(adc0_isr);









  Serial1.begin(57600);



  Serial1.write(0x80);
  Serial1.write(0x83);
  Serial1.write(0xFF);

  for(int i; i <28; i++){
  Serial1.write(0x00);
  //Serial.println(i);
  delay(1);
  }

  Serial1.write(0x8F);
  Serial1.write(0x00);
  Serial1.write(0x00);
  
  delay(250);
  Serial.println("TimeNTP Example");
  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    while (1) {
      Serial.println("Failed to configure Ethernet using DHCP");
      delay(10000);
    }
  }
  
  server.begin();
  
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);

  dasher = 0;
  bitcoin = 200000;
  //random(999999);

  int now1;
  now1 = millis();
  while(millis() - now1 <2000){
     getWeather();
    getCypto();
    readindoor();
    getNtpTime();
    //getstock();
    //getCypto5();
    //https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=TSLA&apikey=QIN3EDBZAGB66ZDP
    //getyahoo();
  }
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{  

      //flipdotdisplay();


if(timer1 >= 10000){
  timer1 = timer1 - 10000;

          if(dasher < 6){
        dasher++;
      }else{
        dasher = 0;
      }

          readindoor();
          flipdotdisplay();
  //Serial.println(second());
}


if(timer2 >= 36000000){
  timer2 = timer2 - 3600000;

  int now1;
  now1 = millis();
  while(millis() - now1 <2000){
    getWeather();
    getCypto();

    getNtpTime();
    //getCypto5();
    //getstock();
  }
  
}


if(timer3 >= 2000){
  timer3 = timer3 - 2000;

  
}

      

  
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
      readindoor();
      
      //bitcoin = random(999999);
      //bitcoin = second();
      //Serial.println(dasher);

//      if(dasher < 6){
//        dasher++;
//      }else{
//        dasher = 0;
//      }
      //flipdotdisplay();
///


  char clientline[BUFSIZ];
  char name[17];
  int index = 0;
  
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    
    // reset the input buffer
    index = 0;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        // If it isn't a new line, add the character to the buffer
        if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          // are we too big for the buffer? start tossing out data
          if (index >= BUFSIZ) 
            index = BUFSIZ -1;
          
          // continue to read more data!
          continue;
        }
        
        // got a \n or \r new line, which means the string is done
        clientline[index] = 0;
        
        // Print it out for debugging
        //Serial.println(clientline);




          
          client.println();



       if (strstr(clientline, "GET /wingon.html")){
          
 
        
          client.println("<!DOCTYPE HTML>");
          
          client.println("<html>");

          client.print("<HEAD>");
          client.print("<meta http-equiv=\"refresh\" content=\"2;URL = /index.html\">");
          client.print("<TITLE />Wing Light Controller</title>");
          client.print("</head>");

                    client.println("<br />");
          client.print("Command sent. Redirecting to ");
          client.print("<a href=/index.html>Home page</a>");   
          client.println("<br />");
    
          client.println("</html>");
       
       break;

 

          
        }else {
        //if(strstr(clientline, "GET /") != 0 ||  strstr(clientline, "GET /index.html")){



          client.println("<!DOCTYPE HTML>");
          
          client.println("<html>");

          client.print("<HEAD>");
          client.print("<meta http-equiv=\"refresh\" content=\"2\">");
          client.print("<TITLE />Clock</title>");
          client.print("</head>");
    
          client.print("<strong>Clock (Code Date: 30March2021) SN: ");
          //client.print(teensyUsbSN());
          client.println("<br />");
          client.println("Home</strong>");
          client.println("<br />");


          client.println("<br />");
          client.print("Time:  ");

          client.println(hour());
          client.println(":");
          client.println(minute());
          client.println(":");
          client.println(second());
          client.println("");

          
                  
          client.println("<br />");
          client.println("<br />"); 
   
          client.println("</html>");

          break;


          }

      break;
      }
//      }else {
//          // everything else is a 404
//          client.println("HTTP/1.1 404 Not Found");
//          client.println("Content-Type: text/html");
//          client.println();
//          client.println("<h2>File Not Found!</h2>");
//        }
        break;
      }

    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();


///


        
    }
  }

    unsigned long timeNow = millis();
  if ((timeNow > api_due_time))  {
    //printTickerData("bitcoin");
    //printTickerData("ethereum");
    //printTickerData("dogecoin");
      //getData();

    api_due_time = timeNow + api_mtbs;
    //httpRequest();
   // getWeather();
    //getCypto();
    //getCypto2();
    //getCypto3();
    //readindoor();
    //getCypto4();
    //getCypto5();
  }



  
}

void digitalClockDisplay(){
  // digital clock display of the time
  //Serial.print(hour());
  //printDigits(minute());
  //printDigits(second());
  //Serial.print(" ");
  //Serial.print(day());
  //Serial.print(" ");
  //Serial.print(month());
  //Serial.print(" ");
  //Serial.print(year()); 
  //Serial.println(); 



  
}




void flipdotdisplay(){



if(hour() > 12){
  hour0 = hour()-12;
}else{
  hour0 = hour();
}


  hour1 = hour0 / 10;
  hour2 = hour0 - hour1 * 10;
  
//  if(hour() > 10){
//    hour2 = hour()-10;
//  }else{
//    hour2 = hour();
//  }

  min1 = minute()/10;
  min2 = minute() - min1*10;

  temp0 = localtemp/100;
  temp1= localtemp/10;
  temp2 = localtemp - temp1*10;

  indoortemp0 = indoortempF/100;
  indoortemp1= indoortempF/10;
  indoortemp2 = indoortempF - indoortemp1*10;

//Serial.println(indoortemp0);
//Serial.println(indoortemp1);
//Serial.println(indoortemp2);


  bitcoin0 =   bitcoin/100000;
  bitcoin1 =   bitcoin/10000-bitcoin0*10;
  bitcoin2 =   bitcoin/1000-bitcoin1*10;
  
  bitcoin3 =   bitcoin/100-bitcoin2*10-bitcoin1*100;
  bitcoin4 =   bitcoin/10-bitcoin3*10-bitcoin2*100-bitcoin1*1000;
  bitcoin5 =   bitcoin-bitcoin4*10-bitcoin3*100-bitcoin2*1000-bitcoin1*10000;
//  Serial.println(bitcoin0);
//    Serial.println(bitcoin1);
//      Serial.println(bitcoin2);
//        Serial.println(bitcoin3);
//          Serial.println(bitcoin4);
//            Serial.println(bitcoin5);
          
  



  Serial1.write(0x80);
  Serial1.write(0x83);
  Serial1.write(0xFF);

  Serial1.write(0x00);

  if(hour1>0){
    hexprint(hour1);
  }else{
    Serial1.write(0x00);
  }
  

  
  hexprint(hour2);
  Serial1.write(0x01);

  hexprint(min1);
  hexprint(min2);

  Serial1.write(0x00); //+1
    if(temp0>0){
        hexprint(temp0);
      }else{
        Serial1.write(0x00);
      }
    if(temp1>0){
        hexprint(temp1);
      }else{
        Serial1.write(0x00);
      }

     if(temp1>2){
        hexprint(temp2);
      }else{
        Serial1.write(0x00);
      }

Serial1.write(0x00);

//indoor temperature

  //Serial1.write(0x00); //+1
    if(indoortemp0>0){
        hexprint(indoortemp0);
      }else{
        Serial1.write(0x00);
      }
    if(indoortemp1>0){
        hexprint(indoortemp1);
      }else{
        Serial1.write(0x00);
      }
    
    hexprint(indoortemp2);

//     if(indoortemp1>2){
//        hexprint(indoortemp2);
//      }else{
//        Serial1.write(0x00);
//      }

//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);
//


Serial1.write(0x00);
hexprint(bitcoin0);
hexprint(bitcoin1);
hexprint(bitcoin2);
hexprint(bitcoin3);
hexprint(bitcoin4);
hexprint(bitcoin5);

Serial1.write(0x00);

//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);
//Serial1.write(0x00);


  if(dasher == 0){
    Serial1.write(0x08);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }else if(dasher == 1){
    Serial1.write(0x00);
    Serial1.write(0x08);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }else if(dasher == 2){
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x08);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }else if(dasher == 3){
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x08);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }else if(dasher == 4){
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x08);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }else if(dasher == 5){
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x08);
    Serial1.write(0x00);
  }else if(dasher == 6){
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x08);
  }else{
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
  }


//Serial1.write(0x00);



   //was 22 - 4 = 18 -5-6=
//    for(int i=0; i <8; i++){
//      Serial1.write(0x00);
//      //hexprint(sendx);
//      //Serial.print("-");
//    }

  Serial1.write(0x8F);
  Serial1.write(0x00);
  Serial1.write(0x00);

//Serial.print("flip dot: ");
//Serial.print(hour1);
//Serial.print("  ");
//Serial.print(hour2);
//Serial.println();


}

void hexprint(int input){

//Serial.print("Recieving: ");
//Serial.println(input);
String inputstring = String(input);
if(inputstring == "-"){Serial1.write(0x01);}
if(inputstring == "0"){Serial1.write(0x7E);}
if(inputstring == "1"){Serial1.write(0x30);}
if(inputstring == "2"){Serial1.write(0x6D);}
if(inputstring == "3"){Serial1.write(0x79);}
if(inputstring == "4"){Serial1.write(0x33);}
if(inputstring == "5"){Serial1.write(0x5B);}
if(inputstring == "6"){Serial1.write(0x5F);}
if(inputstring == "7"){Serial1.write(0x70);}
if(inputstring == "8"){Serial1.write(0x7F);}
if(inputstring == "9"){Serial1.write(0x73);}
if(inputstring == "10"){Serial1.write(0x01);}
  
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Sync");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No Sync");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

//void printTickerData(String ticker) {
//  Serial.println("---------------------------------");
//  Serial.println("Getting ticker data for " + ticker);
//
//
//  // Ticker unfortunately is not the symbol for some reason.
//  // Go to CoinMarketCap.com and select the coin you would like to check
//  // The ticker name makes up the last part of the URL
//  // e.g: http://coinmarketcap.com/currencies/bitcoin/ , "bitcoin" is the ticker value
//
//  // Currency is optional, so you can pass only ticker if you want.
//  // Check out the currency drop down on CoinMarketCap.com to get available values
//  CMCTickerResponse response = api.GetTickerInfo(ticker, "eur");
//  //Serial.println(command);
//  if (response.error == "") {
//    Serial.print("ID: ");
//    Serial.println(response.id);
//    Serial.print("Name: ");
//    Serial.println(response.name);
//    Serial.print("Symbol: ");
//    Serial.println(response.symbol);
//
//    Serial.print("Rank: ");
//    Serial.println(response.rank);
//
//    Serial.print("Price in USD: ");
//    Serial.println(response.price_usd);
//    Serial.print("Price in BTC: ");
//    Serial.println(response.price_btc);
//
//    Serial.print("24h Volume USD: ");
//    Serial.println(response.volume_usd_24h);
//    Serial.print("Market Cap USD: ");
//    Serial.println(response.market_cap_usd);
//
//    Serial.print("Available Supply: ");
//    Serial.println(response.available_supply);
//    Serial.print("Total Supply: ");
//    Serial.println(response.total_supply);
//
//    Serial.print("Percent Change 1h: ");
//    Serial.println(response.percent_change_1h);
//    Serial.print("Percent Change 24h: ");
//    Serial.println(response.percent_change_24h);
//    Serial.print("Percent Change 7d: ");
//    Serial.println(response.percent_change_7d);
//    Serial.print("Last Updated: ");
//    Serial.println(response.last_updated);
//
//    // These fields will not come back if you do not request a currency
//    Serial.print("Price in requested currecy: ");
//    Serial.println(response.price_currency);
//    Serial.print("24h Volume in requested currency: ");
//    Serial.println(response.volume_currency_24h);
//    Serial.print("Market Cap in requested currency: ");
//    Serial.println(response.market_cap_currency);
//
//
//  } else {
//    Serial.print("Error getting data: ");
//    Serial.println(response.error);
//  }
//  Serial.println("---------------------------------");
//}

void getWeather() { 
   Serial.print("connecting to ");
  Serial.print(server1);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server1, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /data/2.5/forecast?q=san%20luis%20obispo,us&appid=a4dd1b6e82e9277997268b51497c001f&cnt=1&units=imperial");
    client.println("Host: http://api.openweathermap.org/");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //beginMicros = micros();





  uint16_t index = 0;
  char c;
  while(client.connected())
  {
    if(client.available())
    {
      c = client.read();
      respBuffer[index++] = c;
      delay(1);
    }
  }
  client.stop();
  
  Serial.print("Total size: ");
  Serial.println(index);
  Serial.println(respBuffer);


//json

// char input[MAX_INPUT_LENGTH];

StaticJsonDocument<768> doc;

DeserializationError error = deserializeJson(doc, respBuffer, index);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* cod = doc["cod"]; // "200"
int message = doc["message"]; // 0
int cnt = doc["cnt"]; // 1

JsonObject list_0 = doc["list"][0];
long list_0_dt = list_0["dt"]; // 1618790400

JsonObject list_0_main = list_0["main"];
float list_0_main_temp = list_0_main["temp"]; // 77.29
float list_0_main_feels_like = list_0_main["feels_like"]; // 76.32
float list_0_main_temp_min = list_0_main["temp_min"]; // 77.13
float list_0_main_temp_max = list_0_main["temp_max"]; // 77.29
int list_0_main_pressure = list_0_main["pressure"]; // 1017
int list_0_main_sea_level = list_0_main["sea_level"]; // 1017
int list_0_main_grnd_level = list_0_main["grnd_level"]; // 971
int list_0_main_humidity = list_0_main["humidity"]; // 34
float list_0_main_temp_kf = list_0_main["temp_kf"]; // 0.09

JsonObject list_0_weather_0 = list_0["weather"][0];
int list_0_weather_0_id = list_0_weather_0["id"]; // 800
const char* list_0_weather_0_main = list_0_weather_0["main"]; // "Clear"
const char* list_0_weather_0_description = list_0_weather_0["description"]; // "clear sky"
const char* list_0_weather_0_icon = list_0_weather_0["icon"]; // "01d"

int list_0_clouds_all = list_0["clouds"]["all"]; // 1

JsonObject list_0_wind = list_0["wind"];
float list_0_wind_speed = list_0_wind["speed"]; // 9.4
int list_0_wind_deg = list_0_wind["deg"]; // 277
float list_0_wind_gust = list_0_wind["gust"]; // 10.94

int list_0_visibility = list_0["visibility"]; // 10000
int list_0_pop = list_0["pop"]; // 0

const char* list_0_sys_pod = list_0["sys"]["pod"]; // "d"

const char* list_0_dt_txt = list_0["dt_txt"]; // "2021-04-19 00:00:00"

JsonObject city = doc["city"];
long city_id = city["id"]; // 5392329
const char* city_name = city["name"]; // "San Luis Obispo"

float city_coord_lat = city["coord"]["lat"]; // 35.3666
double city_coord_lon = city["coord"]["lon"]; // -120.5343

const char* city_country = city["country"]; // "US"
long city_population = city["population"]; // 269637
int city_timezone = city["timezone"]; // -25200
long city_sunrise = city["sunrise"]; // 1618752312
long city_sunset = city["sunset"]; // 1618799828



//json

Serial.print("The temperature in SLO is : ");
//Serial.println(list_0_main_temp);
localtemp = list_0_main_temp;
Serial.println(localtemp);

 }


void getCypto() { 
  Serial.println();
   Serial.print("Connecting to ");
  Serial.print(server5);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server2, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /v1/bpi/currentprice.json HTTP/1.1");
    client.println("Host: api.coindesk.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //beginMicros = micros();


  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    return;
  }



StaticJsonDocument<1024> doc;

DeserializationError error = deserializeJson(doc, client);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject time = doc["time"];
const char* time_updated = time["updated"]; // "Jun 22, 2021 00:05:00 UTC"
const char* time_updatedISO = time["updatedISO"]; // "2021-06-22T00:05:00+00:00"
const char* time_updateduk = time["updateduk"]; // "Jun 22, 2021 at 01:05 BST"

const char* disclaimer = doc["disclaimer"]; // "This data was produced from the CoinDesk Bitcoin Price ...
const char* chartName = doc["chartName"]; // "Bitcoin"

JsonObject bpi = doc["bpi"];

JsonObject bpi_USD = bpi["USD"];
const char* bpi_USD_code = bpi_USD["code"]; // "USD"
const char* bpi_USD_symbol = bpi_USD["symbol"]; // "&#36;"
const char* bpi_USD_rate = bpi_USD["rate"]; // "31,496.1201"
const char* bpi_USD_description = bpi_USD["description"]; // "United States Dollar"
double bpi_USD_rate_float = bpi_USD["rate_float"]; // 31496.1201

JsonObject bpi_GBP = bpi["GBP"];
const char* bpi_GBP_code = bpi_GBP["code"]; // "GBP"
const char* bpi_GBP_symbol = bpi_GBP["symbol"]; // "&pound;"
const char* bpi_GBP_rate = bpi_GBP["rate"]; // "22,620.6395"
const char* bpi_GBP_description = bpi_GBP["description"]; // "British Pound Sterling"
double bpi_GBP_rate_float = bpi_GBP["rate_float"]; // 22620.6395

JsonObject bpi_EUR = bpi["EUR"];
const char* bpi_EUR_code = bpi_EUR["code"]; // "EUR"
const char* bpi_EUR_symbol = bpi_EUR["symbol"]; // "&euro;"
const char* bpi_EUR_rate = bpi_EUR["rate"]; // "26,439.4810"
const char* bpi_EUR_description = bpi_EUR["description"]; // "Euro"
double bpi_EUR_rate_float = bpi_EUR["rate_float"]; // 26439.481

//json

Serial.print("Bitcoin price is : ");
//Serial.println(bpi_USD_rate);


bitcoin = bpi_USD_rate_float*10;
Serial.println(bitcoin);
  Serial.println();

 }


void getCypto2() { 
  Serial.println();
   Serial.print("Connecting to ");
  Serial.print(server3);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server3, 443)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /api/ticker/btc-usd HTTP/1.1");
    client.println("Host: api.cryptonator.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //beginMicros = micros();

    
  uint16_t index = 0;
    uint16_t index2 = 0;
  char c;
  
  while(client.connected())
  {
    if(client.available())
    {
      c = client.read();
      respBuffer[index++] = c;
      
//      if(index > 300){
//        respBuffer2[index2++] = c;
//      }
      delay(1);
    }
  }
  //client.stop();

  Serial.print("Total size: ");
  Serial.println(index);
  Serial.println(respBuffer);

  


//json
// char input[MAX_INPUT_LENGTH];

StaticJsonDocument<192> doc;

DeserializationError error = deserializeJson(doc, respBuffer, index);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject ticker = doc["ticker"];
const char* ticker_base = ticker["base"]; // "BTC"
const char* ticker_target = ticker["target"]; // "USD"
const char* ticker_price = ticker["price"]; // "56816.18602726"
const char* ticker_volume = ticker["volume"]; // "54067.40903098"
const char* ticker_change = ticker["change"]; // "77.15185814"

long timestamp = doc["timestamp"]; // 1618813023
bool success = doc["success"]; // true
const char* error2 = doc["error"]; // ""

  // Disconnect
  client.stop();
//json

  Serial.print("Bitcoin price is : ");
  Serial.println(ticker_price);
  //bitcoin = rate;

  Serial.println();

 }


void getCypto3() { 
  Serial.println();
   Serial.print("Connecting to ");
  Serial.print(server4);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server4, 443)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /api/v3/ticker/price?symbol=BTCUSDT");
    client.println("Host: https://api.binance.com/");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  beginMicros = micros();

    
  uint16_t index = 0;
    uint16_t index2 = 0;
  char c;
  
  while(client.connected())
  {
    if(client.available())
    {
      c = client.read();
      respBuffer[index++] = c;
      
//      if(index > 300){
//        respBuffer2[index2++] = c;
//      }
      delay(1);
    }
  }
  //client.stop();

  Serial.print("Total size: ");
  Serial.println(index);
  Serial.println(respBuffer);

  


//json
// char input[MAX_INPUT_LENGTH];

StaticJsonDocument<16> doc;

DeserializationError error = deserializeJson(doc, respBuffer, index);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* symbol = doc["symbol"]; // "BTCUSDT"
const char* price = doc["price"]; // "49572.54000000"

  // Disconnect
  client.stop();
//json

  //Serial.print("Bitcoin price is : ");
  Serial.println(price);
  //bitcoin = rate;

  Serial.println();

 }

 void getCypto5() { 
  Serial.println();
   Serial.print("Connecting to ");
  Serial.print(server6);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server6, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /query?function=GLOBAL_QUOTE&symbol=TSLA&apikey=QIN3EDBZAGB66ZDP HTTP/1.1");
    client.println("Host: https://www.alphavantage.co/");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //beginMicros = micros();


  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    return;
  }

// Stream& input;

StaticJsonDocument<512> doc;

DeserializationError error = deserializeJson(doc, client);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject Global_Quote = doc["Global Quote"];
const char* Global_Quote_01_symbol = Global_Quote["01. symbol"]; // "TSLA"
const char* Global_Quote_02_open = Global_Quote["02. open"]; // "624.4800"
const char* Global_Quote_03_high = Global_Quote["03. high"]; // "631.3900"
const char* Global_Quote_04_low = Global_Quote["04. low"]; // "608.8800"
const char* Global_Quote_05_price = Global_Quote["05. price"]; // "620.8300"
const char* Global_Quote_06_volume = Global_Quote["06. volume"]; // "24408103"
const char* Global_Quote_07_latest_trading_day = Global_Quote["07. latest trading day"]; // "2021-06-21"
const char* Global_Quote_08_previous_close = Global_Quote["08. previous close"]; // "623.3100"
const char* Global_Quote_09_change = Global_Quote["09. change"]; // "-2.4800"
const char* Global_Quote_10_change_percent = Global_Quote["10. change percent"]; // "-0.3979%"


  Serial.print("Tesla price is : ");
  Serial.println(Global_Quote_05_price);
  //bitcoin = rate;

  Serial.println();

 }

 void readindoor(){

rawindoortemp = adc->adc0->analogRead(A1);
indoortemp = (rawindoortemp*3.3/adc->adc0->getMaxValue())*100;
indoortempF = indoortemp * 9/5 + 32;

//Serial.print("Indoor Temperature: ");
//Serial.println(indoortempF);

  
 }



// void getCypto4() { 
//  Serial.println();
//   Serial.print("Connecting to ");
//  Serial.print(server6);
//  Serial.println("...");
//
//  // if you get a connection, report back via serial:
//  if (client.connect(server6, 80)) {
//    Serial.print("connected to ");
//    Serial.println(client.remoteIP());
//    // Make a HTTP request:
//    client.println("GET /1.0/tops/last?symbols=tsla");
//    client.println("Host: http://api.iextrading.com/");
//    client.println("Connection: close");
//    client.println();
//  } else {
//    // if you didn't get a connection to the server:
//    Serial.println("connection failed");
//  }
//  //beginMicros = micros();
//
//
//// while(client.available()){
////    String line = client.readStringUntil('\r\n\r\n');
////     Serial.print(line);
////  }
//    
//  uint16_t index = 0;
//  char c;
//
//  
//  while(client.connected())
//  {
//    if(client.available())
//    {
//      c = client.read();
//      respBuffer[index++] = c;
//      Serial.print(c);
//      continue;
//    }
//  }
//  //client.stop();
//
//  Serial.print("Total size: ");
//  Serial.println(index);
//  Serial.println(respBuffer);
//  
//
//
////json
//// char input[MAX_INPUT_LENGTH];
//// char input[MAX_INPUT_LENGTH];
//// char input[MAX_INPUT_LENGTH];
//
//StaticJsonDocument<48> doc;
//
//DeserializationError error = deserializeJson(doc, respBuffer, index);
//
//if (error) {
//  Serial.print(F("deserializeJson() failed: "));
//  Serial.println(error.f_str());
//  return;
//}
//
//JsonObject root_0 = doc[0];
//const char* root_0_symbol = root_0["symbol"]; // "TSLA"
//float root_0_price = root_0["price"]; // 709.8
//int root_0_size = root_0["size"]; // 100
//long long root_0_time = root_0["time"]; // 1619812798951
//
//
//  client.stop();
////json
//
//  Serial.print("Bitcoin price is : ");
//  Serial.println(root_0_price);
//  //bitcoin = rate;
//
//  Serial.println();
//
// }


void getstock() { 
  Serial.println();
   Serial.print("Connecting to ");
  Serial.print(serverstock);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(serverstock, 443)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET http://api.marketstack.com/v1/eod/latest?access_key=921f2ce7de70849fa966895215ce4a2a&symbols=tsla HTTP/1.1");
    client.println("Host: api.marketstack.com");
    client.println("Connection: keep-alive");
    client.println("Upgrade-Insecure-Requests: 1");
    client.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36");
    client.println("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    client.println("Accept-Encoding: gzip, deflate");
    client.println("Accept-Language: en-US,en;q=0.9");
    
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  //beginMicros = micros();


  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    return;
  }



StaticJsonDocument<384> doc;

DeserializationError error = deserializeJson(doc, client);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject pagination = doc["pagination"];
int pagination_limit = pagination["limit"]; // 100
int pagination_offset = pagination["offset"]; // 0
int pagination_count = pagination["count"]; // 1
int pagination_total = pagination["total"]; // 1

JsonObject data_0 = doc["data"][0];
float data_0_open = data_0["open"]; // 689.58
float data_0_high = data_0["high"]; // 693.7999
float data_0_low = data_0["low"]; // 668.7
float data_0_close = data_0["close"]; // 671.87
long data_0_volume = data_0["volume"]; // 32496707
// data_0["adj_high"] is null
// data_0["adj_low"] is null
float data_0_adj_close = data_0["adj_close"]; // 671.87
// data_0["adj_open"] is null
// data_0["adj_volume"] is null
int data_0_split_factor = data_0["split_factor"]; // 1
const char* data_0_symbol = data_0["symbol"]; // "TSLA"
const char* data_0_exchange = data_0["exchange"]; // "XNAS"
const char* data_0_date = data_0["date"]; // "2021-06-25T00:00:00+0000"

//json

Serial.print("Stock price is : ");
//Serial.println(bpi_USD_rate);


stockprice = data_0_close;
Serial.println(stockprice);
  Serial.println();

 }


 String getyahoo()
{

  int icomp = 0;
  String body = "";
  unsigned long now;
  bool responseReceived;
   if (client.connect(yahoo, 443) == 1) {
    Serial.print("connecting to yahoo-");

    client.println("GET /quote/TSLA%3FP%3DTSLA?p=TSLA%3FP%3DTSLA HTTP/1.1");
    client.print("Host: ");
    client.println(yahoo);
    client.println("Connection: keep-alive");
    client.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.106 Safari/537.36");
    client.println("Accept: */*");  
    client.print("Referer: http://");
    client.print(yahoo);
    client.println("Accept-Encoding: gzip, deflate");
    client.println("Accept-Language: en-US,en;q=0.9");
     client.println("");
    client.println("");
    //delay(10);
    client.println();

    char c;
    now = millis();
    responseReceived = false;
    bool finishedHeaders = false;
    bool currentLineIsBlank = true;
    while (millis() - now < 1000 || icomp == 0 ) {
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
        responseReceived = true;


        if (!finishedHeaders) {
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
          }
        } else {
          body = body + c;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
//        if(body.indexOf("</response>")>=0){
//          icomp = 1;
//          //Serial.println("icomp=1");
//          }

      }
      if (responseReceived) {
      }
    }

    client.stop();
  }
  return body;
}
