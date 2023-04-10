/*
Project ChatGPT Client For ESPboy
www.espboy.com
RomanS 2023
MIT license
*/


//SET YOUR PARAMETERS BELOW - "your ssid", "your pass", "your key" keeping ""

#define WIFI_SSID_HERE        "your ssid"
#define WIFI_PASSWORD_HERE    "your pass"
#define CHAT_GPT_API_KEY_HERE "your key"

//SET YOUR PARAMETERS ABOVE




#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"
#include "lib/ESPboyTerminalGUI.h"
#include "lib/ESPboyTerminalGUI.cpp"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClientSecureBearSSL.h>
#include <ChatGPT.hpp>

#define EEPROM_STORE_ADDR 5

#define MAX_MODEL_INDEX 4
int8_t modelNo=0;
const char *model[]={
"gpt-3.5-turbo",
"gpt-3.5-turbo-0301",
"text-davinci-003",
"text-davinci-002",
"code-davinci-002"
};


ESPboyInit myESPboy;
ESPboyTerminalGUI terminalGUIobj(&myESPboy.tft, &myESPboy.mcp);

String inpt="";
String rslt="";

static const char *ssid = WIFI_SSID_HERE;
static const char *password = WIFI_PASSWORD_HERE;

BearSSL::WiFiClientSecure client;
ChatGPT<BearSSL::WiFiClientSecure> chat_gpt(&client, "v1", CHAT_GPT_API_KEY_HERE);


void setup() {
  myESPboy.begin("ChatGPT");
  EEPROM.begin(10);

  EEPROM.get(EEPROM_STORE_ADDR, modelNo);
  if(modelNo < 0 || modelNo > MAX_MODEL_INDEX){
    modelNo = 0;
    EEPROM.write(EEPROM_STORE_ADDR, modelNo);
    EEPROM.commit();
   }

  terminalGUIobj.printConsole("+ and - for mode" , TFT_YELLOW, 0, 0);
  terminalGUIobj.printConsole("long press A = ENTER" , TFT_YELLOW, 0, 0);
  terminalGUIobj.printConsole("Connecting to WiFi" , TFT_PURPLE, 0, 0);
  terminalGUIobj.printConsole("", TFT_BLACK, 0, 0);
  
  WiFi.begin(ssid, password);

  terminalGUIobj.toggleDisplayMode(1);

  String prtLne="";
  while (WiFi.status() != WL_CONNECTED) {
    prtLne+=".";
    terminalGUIobj.printConsole(prtLne, TFT_PURPLE, 0, 1);
    delay(1000);
  }
  terminalGUIobj.printConsole("Ready", TFT_PURPLE, 0, 1);
  terminalGUIobj.printConsole(model[modelNo], TFT_PURPLE, 0, 0);
  client.setInsecure();
}



void loop() {
  uint32_t startStr;
    inpt=terminalGUIobj.getUserInput();
    inpt.trim();
    
    if(inpt == "+"){
      modelNo++;
      if (modelNo > MAX_MODEL_INDEX) modelNo = 0;
      EEPROM.write(EEPROM_STORE_ADDR, modelNo);
      EEPROM.commit();
      terminalGUIobj.printConsole(model[modelNo], TFT_PURPLE, 0, 0);
    };
    
    if(inpt == "-"){
      modelNo--;
      if (modelNo < 0) modelNo = MAX_MODEL_INDEX;
      EEPROM.write(EEPROM_STORE_ADDR, modelNo);
      EEPROM.commit();
      terminalGUIobj.printConsole(model[modelNo], TFT_PURPLE, 0, 0);
    };

    if (inpt != "+" && inpt != "-"){ 
      terminalGUIobj.printConsole(inpt, TFT_YELLOW, 1, 0);
      terminalGUIobj.printConsole("AI thinking...", TFT_PURPLE, 0, 0);
      if (chat_gpt.simple_message(model[modelNo], "user", inpt, rslt)) {
        terminalGUIobj.printConsole("", TFT_BLACK, 0, 1);
        startStr=0;
        while(startStr < rslt.length()){
          terminalGUIobj.printConsole(rslt.substring(startStr, startStr+20), TFT_GREEN, 1, 0);
          delay(1000);
          startStr+=20;
        }
      } 
      else
        terminalGUIobj.printConsole(rslt, TFT_RED, 0, 1);
    terminalGUIobj.printConsole("", TFT_BLACK, 0, 0);
    }
}
