// This #include statement was automatically added by the Particle IDE.
#include "application.h"

/*
    This is mostly a copy of the hover code in GIT
    https://raw.githubusercontent.com/hoverlabs/hover_particle/master/examples/HoverSonos/HoverSonos.ino
	
	The rest is from some code i was using to control hue lights, so there may be some cruft and oddly named stuff in here

*/

// You need to add your sonos' IP address here
// for eg
// byte sonosip[] = { 192, 168, 1, 230 };
byte sonosip[] = { z, y, x, w };
 
//#define SONOS_PAUSE "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause></s:Body></s:Envelope>\r\n"
//#define SONOS_PLAY  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play></s:Body></s:Envelope>\r\n"

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


TCPClient client;
unsigned long timeout;
int sonosVolume;
char songTitle[32];
char songArtist[32];
char songVolume[12];
//char songAlbum[32];

// used when printing 3D position coordinates. Using a smaller interval will result in a 'spammy' console. Set to update every 150ms by default.  
long interval = 150;        
long previousMillis = 0;


// really we should only need one button - check if sonos is playing, if so pause, else play (or similar)
int onButton = D1; // The pin the play button is on
int offButton = D2; // The pin the pause buttons is on
int LED = D7;

bool LEDnew = false;
bool schange = false;
bool sonosPlay;



void setup()
{
    Serial.begin(9600);
    
    // jt added 
    delay(1000);
    
    pinMode(LED, OUTPUT);
    pinMode(onButton, INPUT_PULLUP); 
    pinMode(offButton, INPUT_PULLUP); 
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    
    // end jt added
    
    
}
 
void loop()
{
    
  unsigned long currentMillis = millis();    // used for updating 3D position coordinates. Set to update every 150ms by default. 
    while(digitalRead(onButton) == LOW) {
        //Serial.println("onButton ==Low");
        schange = true;
        delay(500);
        sonosPlay = true;
    }
    while(digitalRead(offButton) == LOW) {
        //Serial.println("offButton ==Low");
        schange = true;
        delay(500);
        sonosPlay = false;
    }
    
    
    if (schange){
        Serial.println("going schange == true");
        LEDnew = !LEDnew;
        schange = false;
        if (sonosPlay) { 
            sonos(1); //("PLAY");
        }
        else {
            sonos(0); //("Pause");
        }
        delay(1000);
        //lMillis = millis();
    }

}




void out(const char *s)
{
    client.write( (const uint8_t*)s, strlen(s) );
    Serial.println(s);
}




void sonos(int cmd)
{
  char buf[512];
  char soapmsg[1024];
  char cmdtag[20];
   
  sonosVolume = 35;
  
  if (client.connect(sonosip, 1400)) {
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
        out("POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
    } else {
        out("POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n");
    }

    sprintf(buf, "Host: %d.%d.%d.%d:1400\r\n", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
    out(buf);
    sprintf(buf, "Content-Length: %d\r\n", strlen(soapmsg));
    out(buf);
    out("Content-Type: text/xml; charset=\"utf-8\"\r\n");
    
    if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n", cmdtag);
    } else {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:AVTransport:1#%s\"\r\n", cmdtag);
    }
    out(buf);

    out("\r\n");
    out(soapmsg);

    
    /*wait 1s for timeout*/
    timeout = millis();
    while ((!client.available()) && ((millis() - timeout) < 1000));



    int i = 0;
    char sonosResponse[3072];
    while (client.available()) {
        char c = client.read();
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

        /* Get the Album */
        /*p1 = strcasestr(sonosResponse,";upnp:album&gt;");
        p2 = strcasestr(sonosResponse,"&lt;/upnp:album&gt");
        c = 0;
        
        for (p1 = p1 + 15; p1 != p2; p1++) {
            songAlbum[c] = *p1;
            c++;
        }
        songAlbum[c] = '\0';
        Serial.println(songAlbum);*/

        
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
    
    //while (client.available()) client.read(); 
    //delay(100);
    client.stop();

    
  } else {
      Serial.println("Not yet connected to Sonos.");
  }
}
