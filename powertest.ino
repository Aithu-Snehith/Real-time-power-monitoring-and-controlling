#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

int freq = 2000;
int channel = 0;
int resolution = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(34 , INPUT);
  pinMode(35 , INPUT);
  pinMode(21,OUTPUT);
  pinMode(17,OUTPUT);
  
  
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(21, channel);

  WiFi.begin("Snehith","JjfOCSzk");

  while (WiFi.status() != WL_CONNECTED) 
  {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }
}

int bulb_on = 0 ;
int val_i = 0 , bright = 0 , val_v =0;
float tempv_i=0 , tempv_v=0 ,rms_v=0 , rms_i = 0 , sum_i = 0 , sum_v = 0 , inst_v = 0, inst_i = 0 , a=0 ,b=0;
float sum_p = 0 ,real_p = 0 , appr_p = 0 , pf = 0 ; 

void loop() {
  // put your main code here, to run repeatedly:
  val_i = 0 , bright = 0 , val_v =0;
  tempv_i=0 , tempv_v=0 ,rms_v=0 , rms_i = 0 , sum_i = 0 , sum_v = 0 , inst_v = 0, inst_i = 0;
  sum_p = 0 ,real_p = 0 , appr_p = 0 , pf = 0 ; 

  if (WiFi.status() == WL_CONNECTED) 
  { //Check WiFi connection status
    Serial.println("connected");
    a = millis();
    for(int i = 0 ; i < 1480 ; i++)
    {
      /*if(Serial.available())
      {
        bright = Serial.parseInt() ;
        ledcWrite(channel, bright);   
      }*/

      
      
      val_v = analogRead(35);
      val_i = analogRead(34);
  //    Serial.print(val_v);
  //    Serial.print("  ");
  //    Serial.print(val_i);
  //    //Serial.print("  ..");
  //    Serial.println();
      val_v = val_v - 1810 ;
      val_i = val_i - 1840 ;
     
      tempv_v = (val_v * 3.3)/4095.0 ;
      tempv_i = (val_i * 3.3)/4095.0 ;
  //    Serial.print(volt_temp);
  //    Serial.print("  ");
      inst_v = tempv_v * (330.0/47.0) * (230.0/9.0) ;
      inst_i = (tempv_i)*30/7.0;
  
      sum_p = sum_p + abs(inst_i * inst_v) ;
  //    Serial.print(current);
  //    Serial.print("  ");
        sum_v = sum_v + inst_v * inst_v ;
        sum_i = sum_i + inst_i * inst_i ;
      if(i == 0) b= millis();
  //    Serial.print(sum);
  //    Serial.print("  ");
    }
      
      Serial.println(b-a);
      rms_v = sqrt(sum_v / 1480.0) ;
//      Serial.print("rms voltage = ");
//      Serial.print(rms_v);
  
      rms_i = sqrt(sum_i / 1480.0) ;
//      Serial.print("  rms current = ");
//      Serial.print(rms_i);
  
      real_p = sum_p / 1480.0 ;
//      Serial.print("  real power = ");
//      Serial.print(real_p);
  
      appr_p = rms_v * rms_i ;
//      Serial.print("  apparent power = ");
//      Serial.print(appr_p);
  
      pf = real_p/appr_p ;
//      Serial.print("  power factor = ");
//      Serial.print(pf);*/
  

      // Step 1: Reserve memory space
      //
      StaticJsonBuffer<200> jsonBuffer;
      
      //
      // Step 2: Build object tree in memory
      //
      JsonObject& root = jsonBuffer.createObject();
      root["current"] = rms_i;
      root["voltage"] = rms_v;
      root["power factor"] = pf;
      root["power"] = real_p;
      root["bulb_status"] = 0;
      
      //JsonArray& data = root.createNestedArray("data");
      //data.add(48.756080);
      //data.add(2.302038);
      
      //
      // Step 3: Generate the JSON string
      //
      root.printTo(Serial);
     
      char JSONmessageBuffer[300];
      root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      
      HTTPClient http;    //Declare object of class HTTPClient
 
      http.begin("http://teamzeus.tech/api");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
 
      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload

      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
      
      int len = payload.length() - 1;
      
      for(int i = len-1 ; i>=0 ; i--)
      {
        if(i == len-1)
        {
//          Serial.print("BUlB state: ");
//          Serial.println(payload[i]);
          if(payload[i] == '0') 
          {
            bulb_on = 0;
//            Serial.println("Entered LOW");
            digitalWrite(17 , LOW);
          }
          if(payload[i] == '1')
          {
            bulb_on = 1;
//            Serial.println("Entered HIGH");
            digitalWrite(17 , HIGH);
          }
        }
        else if(i == len -3)
        {
          bright = bright + (payload[i]- '0'); 
        }
        else if(i == len -4)
        {
          if((payload[i] - '0') >= 0 && (payload[i] - '0') <=9 )  bright = bright + (10 *(payload[i]- '0')); 
        }
        else if(i == len -5)
        {
          if((payload[i] - '0') >= 0 && (payload[i] - '0') <=9 )  bright = bright + (100 *(payload[i]- '0')); 
        }
      }
//      Serial.print("value out : ");
//        Serial.println(digitalRead(2));

      ledcWrite(channel, bright);
      Serial.print("  brightness = ");
      Serial.print(bright);
      
      Serial.println();
      delay(1000);
  }
}
