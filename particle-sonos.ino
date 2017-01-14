// This #include statement was automatically added by the Particle IDE.
#include "application.h"

/*
    This is mostly a copy of the hover code in GIT
    https://raw.githubusercontent.com/hoverlabs/hover_particle/master/examples/HoverSonos/HoverSonos.ino
	The rest is from some code i was using to control hue lights, so there may be some cruft and oddly named stuff in here
*/
// You need to add your sonos' IP address here
byte sonosip[] = { 192, 168, 1, 230 };
 
//#define SONOS_PAUSE "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause></s:Body></s:Envelope>\r\n"
//#define SONOS_PLAY  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play></s:Body></s:Envelope>\r\n"

// SONOS SETTINGS

#define SOAP_HEADER "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>"
#define SOAP_FOOTER "</s:Body></s:Envelope>\r\n"

#define SONOS_PAUSE "<u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause>" 
#define SONOS_PLAY "<u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play>"
#define SONOS_NEXT "<u:Next xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Next>"
#define SONOS_PREVIOUS "<u:Previous xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Previous>"
#define SONOS_SETVOLUME_HEADER "<u:SetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>"
#define SONOS_SETVOLUME_FOOTER "</DesiredVolume></u:SetVolume></s:Body></s:Envelope>"
#define SONOS_TRANSPORT_INFO "<u:GetPositionInfo xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:GetPositionInfo>"
#define SONOS_GETVOLUME "<u:GetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel></u:GetVolume>"

#define PAUSE 0
#define PLAY 1
#define NEXT 2
#define PREVIOUS 3
#define VOLUME 4
#define TRANSPORTINFO 5
#define GETVOLUME 6
// END SONOS

// HUE SETTINGS

// END HUE



TCPClient sClient;
//TCPClient hClient;

unsigned long timeout;
int sonosVolume;
char songTitle[32];
char songArtist[32];
char songVolume[12];
//char songAlbum[32];


// used when printing 3D position coordinates. Using a smaller interval will result in a 'spammy' console. Set to update every 150ms by default.  
long interval = 150;        
long previousMillis = 0;


//jt added
int8_t pl16 = D0;
int8_t plMS = D1;
int8_t plRK = D7;
int8_t ofBu = D2;
int8_t onBu = D3;
int8_t hueL = D4;
int8_t volU = D5;
int8_t volD = D6;

int8_t sonAct;

int8_t sonStartVol = 7; // sonos start volume
int8_t sonVolInc   = 2; // sonos volume change incremeent

bool LEDnew = false;
bool schange = false;
bool huesonos = false;
bool sonosPlay;
bool oldHue;
bool newHue;
bool huestate;
// end jt added


void setup()
{
    Serial.begin(9600);
    
    // jt added 
    delay(1000);

//     pinMode(pl16, INPUT_PULLUP);
    pinMode(pl16, INPUT_PULLDOWN);
    pinMode(plMS, INPUT_PULLDOWN);
    pinMode(plRK, INPUT_PULLDOWN);
    pinMode(onBu, INPUT_PULLDOWN);
    pinMode(ofBu, INPUT_PULLDOWN);
    pinMode(hueL, INPUT_PULLDOWN);
    pinMode(volU, INPUT_PULLDOWN);
    pinMode(volD, INPUT_PULLDOWN);
    delay(1000);
    
    newHue = digitalRead(hueL);
    oldHue = newHue;
    sonosVolume=sonStartVol;
    // end jt added
    
    
}
 
void loop()
{
    
  unsigned long currentMillis = millis();    // used for updating 3D position coordinates. Set to update every 150ms by default. 
    while(digitalRead(onBu) == HIGH) {
        Serial.println("onButton ==high");
        schange = true;
        delay(500);
        huesonos = false;
        sonAct = 1;
    }
    while(digitalRead(ofBu) == HIGH) {
        Serial.println("offButton ==high");
        schange = true;
        delay(500);
        huesonos = false;
        sonAct = 0;
    }
    while(digitalRead(pl16) == HIGH) {
        Serial.println("pl16 ==High");
        schange = true;
        delay(500);
        huesonos = false;
        sonAct = 0;
        // ACTION
    }
    /*
    pinMode(hueL, INPUT_PULLUP);
    pinMode(volU, INPUT_PULLUP);
    pinMode(volD, INPUT_PULLUP);
    */
    while(digitalRead(plMS) == HIGH) {
        Serial.println("plMS ==High");
        schange = true;
        delay(500);
        huesonos = false;
        sonAct = 2;
        // ACTION
    }
    while(digitalRead(plRK) == HIGH) {
        Serial.println("plRK ==High");
        schange = true;
        delay(500);
        huesonos = false;
        sonAct = 3;
        // ACTION
    }
    while(digitalRead(volU) == HIGH) {
        Serial.println("volU ==High");
        schange = true;
        delay(500);
        huesonos = false;
        sonosVolume += sonVolInc;
        sonAct = 4;
        // ACTION
    }
    while(digitalRead(volD) == HIGH) {
        Serial.println("volD ==High");
        schange = true;
        delay(500);
        huesonos = false;
        sonosVolume -= sonVolInc;
        sonAct = 4;
        // ACTION
    }
    newHue = digitalRead(hueL);
    if (newHue != oldHue){
        Serial.println("hueL change");
        huesonos = true;
        
        // switch the lights
        schange = true;
        oldHue = newHue;
        
        // huestate != huestate;
        if (huestate) {
            huestate = false;
            Serial.println("huestate false now");
        } else {
            huestate = true;
            Serial.println("huesstate true now");
        }
        Serial.println(huestate);
        delay(500);
    }
        
    if (schange){
        Serial.println("going schange == true");
        schange = false;
        if (huesonos) { 
            // Chnage light state
        }
        else {
            if (sonAct==1){
                // set a low starting volumne, just in case...
                // ideally I'd wait for x minutes to pass, if < timeout just play, else set the volume low and play
                sonosVolume=5;
                sonos(4);
            }
            sonos(sonAct); //("Pause");
        }
        delay(1000);
        //lMillis = millis();
    }

}




void sOut(const char *s)
{
    sClient.write( (const uint8_t*)s, strlen(s) );
    Serial.println(s);
}




void sonos(int cmd)
{
  char buf[512];
  char soapmsg[1024];
  char cmdtag[20];
   
  //sonosVolume = 35;
  
  if (sClient.connect(sonosip, 1400)) {
    Serial.println("client is connected successfully");

    switch (cmd) {
        case PAUSE:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PAUSE, SOAP_FOOTER); 
            strcpy(cmdtag, "Pause");
        break;
        case PLAY:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PLAY, SOAP_FOOTER); 
            strcpy(cmdtag, "Play");
        break;
        case NEXT:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_NEXT, SOAP_FOOTER); 
            strcpy(cmdtag, "Next");
        break;
        case PREVIOUS:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PREVIOUS, SOAP_FOOTER); 
            strcpy(cmdtag, "Previous");
        break;
        case VOLUME:
            sprintf(soapmsg, "%s%s%i%s", SOAP_HEADER, SONOS_SETVOLUME_HEADER, sonosVolume, SONOS_SETVOLUME_FOOTER); 
            strcpy(cmdtag, "SetVolume");
        break;
        case TRANSPORTINFO:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_TRANSPORT_INFO, SOAP_FOOTER); 
            strcpy(cmdtag, "GetPositionInfo");
        break;
        case GETVOLUME:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_GETVOLUME, SOAP_FOOTER); 
            strcpy(cmdtag, "GetVolume");
        break;
    }
    
    if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
        sOut("POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
    } else {
        sOut("POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n");
    }

    sprintf(buf, "Host: %d.%d.%d.%d:1400\r\n", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
    sOut(buf);
    sprintf(buf, "Content-Length: %d\r\n", strlen(soapmsg));
    sOut(buf);
    sOut("Content-Type: text/xml; charset=\"utf-8\"\r\n");
    
    if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n", cmdtag);
    } else {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:AVTransport:1#%s\"\r\n", cmdtag);
    }
    sOut(buf);

    sOut("\r\n");
    sOut(soapmsg);

    
    /*wait 1s for timeout*/
    timeout = millis();
    while ((!sClient.available()) && ((millis() - timeout) < 1000));



    int i = 0;
    char sonosResponse[3072];
    while (sClient.available()) {
        char c = sClient.read();
        Serial.print(c);
        
        if (String(cmdtag) == "GetPositionInfo" || String(cmdtag) == "GetVolume") {
            sonosResponse[i] = c;
            Serial.print(sonosResponse[i]);       //This line prints the entire response
            i++;
        }
        
    }

    if (String(cmdtag) == "GetPositionInfo") {
        sonosResponse[i] = '\0';

        /* Get the song title */
        char *p1 = strcasestr(sonosResponse,"dc:title&gt;");
        char *p2 = strcasestr(sonosResponse,"&lt;/dc:title");

        int c = 0;
        for (p1 = p1 + 12; p1 != p2; p1++) {
            songTitle[c] = *p1;
            c++;
        }
        songTitle[c] = '\0';
        Serial.println(songTitle);

        /* Get the Artist */
        p1 = strcasestr(sonosResponse,"dc:creator&gt;");
        p2 = strcasestr(sonosResponse,"&lt;/dc:creator");
        c = 0;
        
        for (p1 = p1 + 14; p1 != p2; p1++) {
            songArtist[c] = *p1;
            c++;
        }
        songArtist[c] = '\0';
        Serial.println(songArtist);

        
    } else if (String(cmdtag) == "GetVolume") {
        sonosResponse[i] = '\0';
        
        char *p1 = strcasestr(sonosResponse,"<CurrentVolume>");
        char *p2 = strcasestr(sonosResponse,"</CurrentVolume>");

        int c = 0;
        for (p1 = p1 + 15; p1 != p2; p1++) {
            songVolume[c] = *p1;
            c++;
        }
        songVolume[c] = '\0';
        Serial.println(songVolume);
        
        sscanf(songVolume, "%d", &sonosVolume);
        Serial.println(sonosVolume);
        
    }
    
    sClient.stop();

    
  } else {
      Serial.println("Not yet connected to Sonos.");
  }
}
