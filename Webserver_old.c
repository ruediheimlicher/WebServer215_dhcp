//
//  Webserver_old.c
//  WebServer
//
//  Created by Ruedi Heimlicher on 22.07.2016.
//
//

/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Chip type           : Atmega88 or Atmega168 or Atmega328 with ENC28J60
 * Note: there is a version number in the text. Search for tuxgraphics
 *********************************************/
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
//#include <sys/types.h>

//#include <netinet/in_systm.h>
//#include <netdb.h>




#include "ip_arp_udp_tcp.c"
#include "websrv_help_functions.c"
#include "enc28j60.h"
//#include "timeout.h"
#include "net.h"

#include <avr/wdt.h>
#include "lcd.c"
#include "adc.c"
#include "current.c"

#include "datum.c"
#include "version.c"
#include "homedata.c"

#include "eepromhelper.c"
 */
//***********************************
//									*
//									*
//***********************************
//
#include "usbdrv.h"
#include "usbdrv.c"

#include "oddebug.h"

//									*
//***********************************


//#define SDAPIN		4
//#define SCLPIN		5

/*
 #define TASTE1		38
 #define TASTE2		46
 #define TASTE3		54
 #define TASTE4		72
 #define TASTE5		95
 #define TASTE6		115
 #define TASTE7		155
 #define TASTE8		186
 #define TASTE9		205
 #define TASTEL		225
 #define TASTE0		235
 #define TASTER		245
 */

#define	LOOPLEDPORT		PORTB
#define	LOOPLEDPORTPIN	DDRB
#define	LOOPLED			1
#define	SENDLED			0



#define STARTDELAYBIT		0
#define WDTBIT             7
#define TASTATURPIN			0           //	Eingang fuer Tastatur
#define ECHOPIN            5           //	Ausgang fuer Impulsanzeige
#define ANALOGPIN 6
#define MASTERCONTROLPIN	4           // Eingang fuer MasterControl: Meldung MasterReset

#define INT0PIN            2           // Pin Int0
#define INT1PIN            3           // Pin Int1

#define TIMER0_SEKUNDENTAKT            50000           // Anz Overflows von timer0 bis 1 Sekunde


volatile uint16_t EventCounter=0;
static char baseurl[]="http://ruediheimlichercurrent.dyndns.org/";

static void usbPoll(void);
static inline usbMsgLen_t usbDriverDescriptor(usbRequest_t *rq);
/* *************************************************************************  */
/* Eigene Deklarationen                                                       */
/* *************************************************************************  */

volatile uint16_t					timer2_counter=0;

//enum webtaskflag{IDLE, TWISTATUS,EEPROMREAD, EEPROMWRITE};

static uint8_t  webtaskflag =0;

static uint8_t monitoredhost[4] = {10,0,0,7};


static char CurrentDataString[64];

static char* teststring = "pw=Pong&strom0=360\0";

volatile uint8_t oldErrCounter=0;

static volatile uint8_t stepcounter=0;

static volatile uint16_t timer0counter=0;
static volatile uint16_t sekundencounter=0;


// Prototypes
void lcdinit(void);
void r_itoa16(int16_t zahl, char* string);
void tempbis99(uint16_t temperatur,char*tempbuffer);


// the password string (only the first 5 char checked), (only a-z,0-9,_ characters):
static char password[10]="ideur00"; // must not be longer than 9 char
static char resetpassword[10]="ideur!00!"; // must not be longer than 9 char


uint8_t TastenStatus=0;
uint16_t Tastencount=0;
uint16_t Tastenprellen=0x01F;


//static volatile uint8_t pingnow=1; // 1 means time has run out send a ping now
//static volatile uint8_t resetnow=0;
//static volatile uint8_t reinitmac=0;
//static uint8_t sendping=1; // 1 we send ping (and receive ping), 0 we receive ping only
static volatile uint8_t pingtimer=1; // > 0 means wd running
//static uint8_t pinginterval=30; // after how many seconds to send or receive a ping (value range: 2 - 250)
static char *errmsg; // error text

unsigned char TWI_Transceiver_Busy( void );
//static volatile uint8_t twibuffer[twi_buffer_size+1]; // Buffer fuer Data aus/fuer EEPROM
static volatile char twiadresse[4]; // EEPROM-Adresse
//static volatile uint8_t hbyte[4];
//static volatile uint8_t lbyte[4];
extern volatile uint8_t twiflag;
static uint8_t aktuelleDatenbreite=8;
static volatile uint8_t send_cmd=0;


#define TIMERIMPULSDAUER            10    // us

#define CURRENTSEND                 0     // Daten an server senden

volatile uint16_t                   wattstunden=0;
volatile uint16_t                   kilowattstunden=0;
volatile uint32_t                   webleistung=0;

volatile float leistung =1;
float lastleistung =1;
uint8_t lastcounter=0;
volatile uint8_t  anzeigewert =0;

static char stromstring[10];



void timer0(void);
uint8_t WochentagLesen(unsigned char ADRESSE, uint8_t hByte, uint8_t lByte, uint8_t *Daten);
uint8_t SlavedatenLesen(const unsigned char ADRESSE, uint8_t *Daten);
void lcd_put_tempAbMinus20(uint16_t temperatur);

// hostip-Stuff
volatile uint8_t hostip[4];

const char PROGMEM pfeilvollrechts[]={0x00,0x00,0x7F,0x3E,0x1C,0x08};
volatile uint8_t                 curr_model=0; // aktuelles modell
uint8_t              EEMEM       speichermodel=0;

/* ***** EEPROM-Stuff ********************************* */
uint8_t EEMEM ip0;
uint8_t EEMEM ip1;
uint8_t EEMEM ip2;
uint8_t EEMEM ip3;
uint8_t EEMEM iparray[4];



/* ************************************************************************ */
/* Ende Eigene Deklarationen																 */
/* ************************************************************************ */


// Note: This software implements a web server and a web browser.
// The web server is at "myip" and the browser tries to access "websrvip".
//
// Please modify the following lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
//static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x29};

//RH4702 52 48 34 37 30 33
static uint8_t mymac[6] = {0x52,0x48,0x34,0x37,0x30,0x49};
//static uint8_t mymac[6] = {0x52,0x48,0x34,0x37,0x30,0x99};

// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX
// This web server's own IP.

// IP des Webservers
static uint8_t myip[4] = {192,168,1,215};



// IP address of the web server to contact (IP of the first portion of the URL):
//static uint8_t websrvip[4] = {77,37,2,152};
// ruediheimlicher
//static uint8_t websrvip[4] = {213,188,35,156}; //213,188,35,156 30.7.2014 msh

//static uint8_t websrvip[4] = {193,17,85,42}; // ruediheimlicher nine
//static uint8_t websrvip[4] = {64,37,49,112}; // 64.37.49.112   28.02.2015 hostswiss // Pfade in .pl angepasst: cgi-bin neu in root dir

// **************************************************
// **************************************************
// Anpassen bei Aenderung:

//static uint8_t websrvip[4] = {217,26,52,16};//        217.26.52.16  24.03.2015 hostpoint
static uint8_t websrvip[4] = {217,26,53,231};//        217.26.53.231  18.07.2016 hostpoint


// **************************************************
// **************************************************
// The name of the virtual host which you want to contact at websrvip (hostname of the first portion of the URL):


#define WEBSERVER_VHOST "www.ruediheimlicher.ch"


// Default gateway. The ip address of your DSL router. It can be set to the same as
// websrvip the case where there is no default GW to access the
// web server (=web server is on the same lan as this host)

// ************************************************
// IP der Basisstation !!!!!

// Viereckige Basisstation:
static uint8_t gwip[4] = {192,168,1,1};// Rueti

// ************************************************

static char urlvarstr[21];
// listen port for tcp/www:
#define MYWWWPORT 1401
#define MYWWWTESTPORT 1414

#define BUFFER_SIZE 800


static uint8_t buf[BUFFER_SIZE+1];
static uint8_t pingsrcip[4];
static uint8_t start_web_client=0;
static uint8_t web_client_sendok=0; // Anzahl callbackaufrufe

static uint8_t  callbackerrcounter=0;

#define CURRENTSEND                 0     // Bit fuer: Daten an Server senden
#define CURRENTSTOP                 1     // Bit fuer: Impulse ignorieren
#define CURRENTWAIT                 2     // Bit fuer: Impulse wieder bearbeiten
#define CURRENTMESSUNG              3     // Bit fuer: Messen
#define DATASEND                    4     // Bit fuer: Daten enden
#define DATAOK                      5     // Data kan gesendet weren
#define DATAPEND                    6     // Senden ist pendent
#define DATALOOP                    7     // wird von loopcount1 gesetzt, startet senden


void str_cpy(char *ziel,char *quelle)
{
   uint8_t lz=strlen(ziel); //startpos fuer cat
   //printf("Quelle: %s Ziellaenge: %d\n",quelle,lz);
   uint8_t lq=strlen(quelle);
   //printf("Quelle: %s Quelllaenge: %d\n",quelle,lq);
   uint8_t i;
   for(i=0;i<lq;i++)
   {
      //printf("i: %d quelle[i]: %c\n",i,quelle[i]);
      ziel[i]=quelle[i];
   }
   lz=strlen(ziel);
   ziel[lz]='\0';
}

void str_cat(char *ziel,char *quelle)
{
   uint8_t lz=strlen(ziel); //startpos fuer cat
   //printf("Quelle: %s Ziellaenge: %d\n",quelle,lz);
   uint8_t lq=strlen(quelle);
   //printf("Quelle: %s Quelllaenge: %d\n",quelle,lq);
   uint8_t i;
   for(i=0;i<lq;i++)
   {
      //printf("i: %d quelle[i]: %c\n",i,quelle[i]);
      ziel[lz+i]=quelle[i];
      
   }
   //printf("ziel: %s\n",ziel);
   lz=strlen(ziel);
   ziel[lz]='\0';
}


// http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char *trimwhitespace(char *str)
{
   char *end;
   
   // Trim leading space
   while(isspace(*str)) str++;
   
   if(*str == 0)  // All spaces?
      return str;
   
   // Trim trailing space
   end = str + strlen(str) - 1;
   while(end > str && isspace(*end)) end--;
   
   // Write new null terminator
   *(end+1) = 0;
   
   return str;
}

void timer0() // Analoganzeige Messinstrument
{
   //----------------------------------------------------
   // Set up timer 0
   //----------------------------------------------------
   /*
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS00) | _BV(CS02);
    OCR0A = 0x2;
    TIMSK0 = _BV(OCIE0A);
    */
   
   DDRD |= (1<< PD6);   // OC0A Output
   
   TCCR0A |= (1<<WGM00);   // fast PWM  top = 0xff
   TCCR0A |= (1<<WGM01);   // PWM
   //TCCR0A |= (1<<WGM02);   // PWM
   
   TCCR0A |= (1<<COM0A1);   // set OC0A at bottom, clear OC0A on compare match
   TCCR0B |= 1<<CS02;
   TCCR0B |= 1<<CS00;
   
   OCR0A=0;
   TIMSK0 |= (1<<OCIE0A);
   
   
}

ISR(TIMER0_COMPA_vect)
{
   
   OCR0A = anzeigewert;
   //OCR0A++;
   PORTD &= ~(1<<ANALOGPIN); //LO
   
   
}


ISR(TIMER0_OVF_vect)
{
   timer0counter++;
   if (timer0counter== TIMER0_SEKUNDENTAKT)
   {
      OSZITOGG;
      sekundencounter++;
      timer0counter=0;
      //lcd_gotoxy(15,0);
      //lcd_putint(timer0counter);
   }
   PORTD |= (1<<ANALOGPIN);
}


uint16_t http200ok(void)
{
   return(fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n")));
}

// we were ping-ed by somebody, store the ip of the ping sender
// and trigger an upload to http://tuxgraphics.org/cgi-bin/upld
// This is just for testing and demonstration purpose
void ping_callback(uint8_t *ip)
{
   uint8_t i=0;
   // trigger only first time in case we get many ping in a row:
   if (start_web_client==0)
   {
      start_web_client=1;
      //			lcd_gotoxy(12,0);
      //			lcd_puts("ping\0");
      // save IP from where the ping came:
      lcd_gotoxy(0,2);
      
      while(i<4)
      {
         pingsrcip[i]=ip[i];
         lcd_putint(pingsrcip[i]);
         if (i<3)
         {
            lcd_putc('.');
         }
         i++;
      }
      
      
   }
}

void strom_browserresult_callback(uint8_t statuscode,uint16_t datapos)
{
   // datapos is not used in this example
   if (statuscode==0)
   {
      
      lcd_gotoxy(12,3);
      lcd_puts("           \0");
      lcd_gotoxy(12,3);
      lcd_puts("s cb OK \0");
      
      // lcd_gotoxy(19,0);
      // lcd_putc(' ');
      lcd_gotoxy(19,0);
      lcd_putc('+');
      
      webstatus &= ~(1<<DATASEND); // Datasend auftrag ok
      
      webstatus &= ~(1<<DATAPEND); // Quittung fuer callback-erhalt
      
      // Messungen wieder starten
      
      webstatus &= ~(1<<CURRENTSTOP); // Messung wieder starten
      webstatus |= (1<<CURRENTWAIT); // Beim naechsten Impuls Messungen wieder starten
      sei();
      
      web_client_sendok++;
      
      
   }
   else
   {
      
      lcd_gotoxy(0,3);
      lcd_puts("           \0");
      lcd_gotoxy(0,3);
      lcd_puts("s cb err \0");
      lcd_puthex(statuscode);
      
      lcd_gotoxy(19,0);
      lcd_putc(' ');
      lcd_gotoxy(19,0);
      lcd_putc('-');
   }
}

void home_browserresult_callback(uint8_t statuscode,uint16_t datapos)
{
   // datapos is not used in this example
   if (statuscode==0)
   {
      lcd_gotoxy(0,0);
      lcd_puts("        \0");
      lcd_gotoxy(0,0);
      lcd_puts("h cb OK\0");
      web_client_sendok++;
      //				sei();
      
   }
   else
   {
      lcd_gotoxy(0,0);
      lcd_puts("        \0");
      lcd_gotoxy(0,0);
      lcd_puts("h cb err\0");
      lcd_puthex(statuscode);
      
   }
}



void alarm_browserresult_callback(uint8_t statuscode,uint16_t datapos)
{
   // datapos is not used in this example
   if (statuscode==0)
   {
      
      lcd_gotoxy(0,0);
      lcd_puts("        \0");
      lcd_gotoxy(0,0);
      lcd_puts("a cb OK\0");
      
      web_client_sendok++;
      //				sei();
      
   }
   else
   {
      lcd_gotoxy(0,0);
      lcd_puts("         \0");
      lcd_gotoxy(0,0);
      lcd_puts("a cb err\0");
      lcd_puthex(statuscode);
      
   }
}

/* ************************************************************************ */
/* Eigene Funktionen														*/
/* ************************************************************************ */

uint8_t verify_password(char *str)
{
   // the first characters of the received string are
   // a simple password/cookie:
   if (strncmp(password,str,7)==0)
   {
      return(1);                 // PW OK
   }
   return(0);                    //PW falsch
}


uint8_t verify_reset_password(char *str)
{
   // the first characters of the received string are
   // a simple password/cookie:
   if (strncmp(resetpassword,str,7)==0)
   {
      return(1); // Reset-PW OK
   }
   return(0); //Reset-PW falsch
}


void delay_ms(unsigned int ms)/* delay for a minimum of <ms> */
{
   // we use a calibrated macro. This is more
   // accurate and not so much compiler dependent
   // as self made code.
   while(ms){
      _delay_ms(0.96);
      ms--;
   }
}

uint8_t Hex2Int(char *s)
{
   long res;
   char *Chars = "0123456789ABCDEF", *p;
   
   if (strlen(s) > 8)
   /* Error ... */ ;
   
   for (res = 0L; *s; s++) {
      if ((p = strchr(Chars, toupper(*s))) == NULL)
      /* Error ... */ ;
      res = (res << 4) + (p-Chars);
   }
   return res;
}

void tempbis99(uint16_t temperatur,char*tempbuffer)
{
   char buffer[8]={};
   //uint16_t temp=(temperatur-127)*5;
   uint16_t temp=temperatur*5;
   //itoa(temp, buffer,10);
   
   r_itoa16(temp,buffer);
   //lcd_puts(buffer);
   //lcd_putc('*');
   //char outstring[7]={};
   
   tempbuffer[6]='\0';
   tempbuffer[5]=' ';
   tempbuffer[4]=buffer[6];
   tempbuffer[3]='.';
   tempbuffer[2]=buffer[5];
   if (abs(temp)<100)
   {
      tempbuffer[1]=' ';
   }
   else
   {
      tempbuffer[1]=buffer[4];
   }
   tempbuffer[0]=buffer[0];
}

void tempAbMinus20(uint16_t temperatur,char*tempbuffer)
{
   char buffer[8]={};
   int16_t temp=(temperatur)*5;
   temp -=200;
   char Vorzeichen=' ';
   if (temp < 0)
   {
      Vorzeichen='-';
   }
   
   r_itoa16(temp,buffer);
   //		lcd_puts(buffer);
   //		lcd_putc(' * ');
   
   //		char outstring[7]={};
   
   tempbuffer[6]='\0';
   //outstring[5]=0xDF; // Grad-Zeichen
   tempbuffer[5]=' ';
   tempbuffer[4]=buffer[6];
   tempbuffer[3]='.';
   tempbuffer[2]=buffer[5];
   if (abs(temp)<100)
   {
      tempbuffer[1]=Vorzeichen;
      tempbuffer[0]=' ';
   }
   else
   {
      tempbuffer[1]=buffer[4];
      tempbuffer[0]=Vorzeichen;
   }
   //		lcd_puts(outstring);
}


// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in the global var strbuf_A

// Andere Version in Webserver help funktions
/*
 uint8_t find_key_val(char *str,char *key)
 {
 uint8_t found=0;
 uint8_t i=0;
 char *kp;
 kp=key;
 while(*str &&  *str!=' ' && found==0){
 if (*str == *kp)
 {
 kp++;
 if (*kp == '\0')
 {
 str++;
 kp=key;
 if (*str == '=')
 {
 found=1;
 }
 }
 }else
 {
 kp=key;
 }
 str++;
 }
 if (found==1){
 // copy the value to a buffer and terminate it with '\0'
 while(*str &&  *str!=' ' && *str!='&' && i<STR_BUFFER_SIZE)
 {
 strbuf_A[i]=*str;
 i++;
 str++;
 }
 strbuf_A[i]='\0';
 }
 // return the length of the value
 return(i);
 }
 */
#pragma mark eeprom
// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html

void write_eeprom_wsrvip(uint8_t * ipArray)
{
   eeprom_write_byte(&speichermodel, curr_model);
   
}

void read_eeprom_swsrvip(uint8_t* ipStart)
{
   // https://www.mikrocontroller.net/topic/161881
   uint8_t addr0  = eeprom_read_byte((uint8_t*)ipStart);
   
   // readBlock
   uint8_t ipString[4];
   eeprom_read_block((void*)&ipString, (const void*)ipStart, 4);
}



#pragma mark analye_get_url

// takes a string of the form ack?pw=xxx&rst=1 and analyse it
// return values:  0 error
//                 1 resetpage and password OK
//                 4 stop wd
//                 5 start wd
//                 2 /mod page
//                 3 /now page
//                 6 /cnf page

uint8_t analyse_get_url(char *str)	// codesnippet von Watchdog
{
   char actionbuf[32];
   errmsg="inv.pw";
   
   webtaskflag =0; //webtaskflag zuruecksetzen. webtaskflag bestimmt aktion, die ausgefuehrt werden soll. Wird an Master weitergegeben.
   // ack
   
   // str: ../ack?pw=ideur00&rst=1
   if (strncmp("ack",str,3)==0)
   {
      lcd_clr_line(1);
      lcd_gotoxy(0,1);
      lcd_puts("ack\0");
      
      // Wert des Passwortes eruieren
      if (find_key_val(str,actionbuf,10,"pw"))
      {
         urldecode(actionbuf);
         
         // Reset-PW?
         if (verify_reset_password(actionbuf))
         {
            return 15;
         }
         
         // Passwort kontrollieren
         if (verify_password(actionbuf))
         {
         }
         
      }
      
      return(0);
      
   }//ack
   
   if (strncmp("twi",str,3)==0)										//	Daten von HC beginnen mit "twi"
   {
      //lcd_clr_line(1);
      lcd_gotoxy(0,2);
      //lcd_puts("twi\0");
      
      
      // Wert des Passwortes eruieren
      if (find_key_val(str,actionbuf,10,"pw"))					//	Passwort kommt an zweiter Stelle
      {
         urldecode(actionbuf);
         webtaskflag=0;
         lcd_puts(actionbuf);
         lcd_gotoxy(0,2);
         lcd_puts("pw da");
         // Passwort kontrollieren
         
         
         if (verify_password(actionbuf))							// Passwort ist OK
         {
            //lcd_puts(" pw ok");
            //OSZILO;
            
            // hostip empfangen
            {
               /*
                // http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__eeprom_1gac5c2be42eb170be7a26fe8b7cce4bc4d.html
                void eeprom_write_block(const void * __src, void * __dst, size_t __n);
                */
               // https://www.mikrocontroller.net/articles/AVR-GCC-Tutorial#Block_lesen.2Fschreiben
               /*
                Datenblock in EEPROM schreiben
                eeprom_write_block (myByteBuffer, eeFooByteArray1, sizeof(myByteBuffer));
                
                Datenblock aus EEPROM lesen
                void eeprom_read_block(void * __dst,const void * __src,size_t __n)
                */
               /*
                eeprom_write_byte(&ip0, 192l);
                eeprom_write_byte(&ip1, temp1);
                eeprom_write_byte(&ip2, temp2);
                eeprom_write_byte(&ip3, temp3);
                
                */
               if (find_key_val(str,actionbuf,10,"data0")) // data0 von hostip
               {
                  eeprom_write_byte(&ip0, atoi(actionbuf));
                  websrvip[0]=atoi(actionbuf);
               }
               if (find_key_val(str,actionbuf,10,"data1")) // data1 von hostip
               {
                  eeprom_write_byte(&ip1, atoi(actionbuf));
                  websrvip[1]=atoi(actionbuf);
               }
               if (find_key_val(str,actionbuf,10,"data2")) // data2 von hostip
               {
                  eeprom_write_byte(&ip2, atoi(actionbuf));
                  websrvip[2]=atoi(actionbuf);
               }
               if (find_key_val(str,actionbuf,10,"data3")) // data3 von hostip
               {
                  eeprom_write_byte(&ip3, atoi(actionbuf));
                  websrvip[3]=atoi(actionbuf);
               }
               uint8_t tempiparray[4];
               eeprom_read_block(&tempiparray,&iparray,sizeof(tempiparray));
               
               return (16);
            }
            
            if (find_key_val(str,actionbuf,10,"status"))		// Status soll umgeschaltet werden
            {
               return 1;
            }//st
            
            // Daten fuer EEPROM von Homeserver empfangen
            
            if (find_key_val(str,actionbuf,10,"wadr"))			// EEPROM-Daten werden von Homeserver gesendet
            {
               return (9);												// Empfang bestätigen
            } // wadr
            
            if (find_key_val(str,actionbuf,10,"iswriteok"))		// Anfrage ob writeok
            {
               
               return (7);
            }
            
            if (find_key_val(str,actionbuf,12,"isstat0ok"))		// Anfrage ob statusok isstat0ok
            {
               return (10);
            }
            
            if (find_key_val(str,actionbuf,10,"reset")) // HomeCentral reseten
            {
               
            }
            
            // Auslesen der Daten
            if (find_key_val(str,actionbuf,10,"data0")) // HomeCentral reseten
            {
               
               if (actionbuf[0]=='0') // data
               {
                  return (25);
               }
               if (actionbuf[0]=='1') // data
               {
                  return (26);
               }
            }
         }//verify pw
      }//find_key pw
      return(0);
   }//twi
   errmsg="inv.url";
   return(0);
}

uint16_t print_webpage_ok(uint8_t *buf,uint8_t *okcode)
{
   // Schickt den okcode als Bestaetigung fuer den Empfang des Requests
   uint16_t plen;
   plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
   plen=fill_tcp_data_p(buf,plen,PSTR("<p>okcode="));
   plen=fill_tcp_data(buf,plen,(void*)okcode);
   return plen;
}

uint16_t print_webpage_hostip(uint8_t *buf,uint8_t *okcode)
{
   
   // Schickt den okcode als Bestaetigung fuer den Empfang der hostip
   uint16_t plen;
   plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
   plen=fill_tcp_data_p(buf,plen,PSTR("<p>okcode="));
   plen=fill_tcp_data(buf,plen,(void*)okcode);
   return plen;
   
}


#pragma mark Webpage_status

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage_status(uint8_t *buf)
{
   uint16_t plen=0;
   //char vstr[5];
   plen=http200ok();
   
   plen=fill_tcp_data_p(buf,plen,PSTR("<h1>HomeCurrent</h1>"));
   //
   
   //
   if (TEST)
   {
      plen=fill_tcp_data_p(buf,plen,PSTR("<p>  HomeCurrent Test<br>  Falkenstrasse 20<br>  8630 Rueti"));
   }
   else
   {
      plen=fill_tcp_data_p(buf,plen,PSTR("<p>  HomeCurrent<br>  Falkenstrasse 20<br>  8630 Rueti"));
   }
   
   
   plen=fill_tcp_data_p(buf,plen,PSTR("<hr><h4><font color=\"#00FF00\">Status</h4></font></p>"));
   
   
   //return(plen);
   plen=fill_tcp_data_p(buf,plen,PSTR("<p>Leistung: "));
   
   //Temperatur=WebRxDaten[2];
   //Temperatur=inbuffer[2]; // Vorlauf
   //Temperatur=Vorlauf;
   //tempbis99(Temperatur,TemperaturString);
   
   //r_itoa(Temperatur,TemperaturStringV);
   
   plen=fill_tcp_data(buf,plen,stromstring);
   
   plen=fill_tcp_data_p(buf,plen,PSTR(" Watt</p>"));
   
   
   //return(plen);
   
   // Taste und Eingabe fuer Passwort
   plen=fill_tcp_data_p(buf,plen,PSTR("<form action=/ack method=get>"));
   plen=fill_tcp_data_p(buf,plen,PSTR("<p>\nPasswort: <input type=password size=10 name=pw ><input type=hidden name=tst value=1>  <input type=submit value=\"Bearbeiten\"></p></form>"));
   
   plen=fill_tcp_data_p(buf,plen,PSTR("<p><hr>"));
   plen=fill_tcp_data(buf,plen,DATUM);
   plen=fill_tcp_data_p(buf,plen,PSTR("  Ruedi Heimlicher"));
   plen=fill_tcp_data_p(buf,plen,PSTR("<br>Version :"));
   plen=fill_tcp_data(buf,plen,VERSION);
   plen=fill_tcp_data_p(buf,plen,PSTR("\n<hr></p>"));
   
   //
   
   /*
    // Tux
    plen=fill_tcp_data_p(buf,plen,PSTR("<h2>web client status</h2>\n<pre>\n"));
    
    char teststring[24];
    strcpy(teststring,"Data");
    strcat(teststring,"-\0");
    strcat(teststring,"Uploads \0");
    strcat(teststring,"mit \0");
    strcat(teststring,"ping : \0");
    plen=fill_tcp_data(buf,plen,teststring);
    
    plen=fill_tcp_data_p(buf,plen,PSTR("Data-Uploads mit ping: "));
    // convert number to string:
    itoa(web_client_attempts,vstr,10);
    plen=fill_tcp_data(buf,plen,vstr);
    plen=fill_tcp_data_p(buf,plen,PSTR("\nData-Uploads aufs Web: "));
    // convert number to string:
    itoa(web_client_sendok,vstr,10);
    plen=fill_tcp_data(buf,plen,vstr);
    plen=fill_tcp_data_p(buf,plen,PSTR("\n</pre><br><hr>"));
    */
   return(plen);
}

uint16_t print_webpage_data(uint8_t *buf,uint8_t *data)
{
   // Schickt die Daten an den cronjob
   uint16_t plen=0;
   plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
   //plen=fill_tcp_data_p(buf,plen,PSTR("<p>data="));
   plen=fill_tcp_data(buf,plen,(void*)data);
   
   return plen;
}


void master_init(void)
{
   
   DDRB |= (1<<PORTB1);	//Bit 1 von PORT B als Ausgang für Kontroll-LED
   PORTB |= (1<<PORTB1);	//Pull-up
   DDRB |= (1<<PORTB0);	//Bit 1 von PORT B als Ausgang für Kontroll-LED
   PORTB |= (1<<PORTB0);	//Pull-up
   
   DDRD |=(1<<ECHOPIN); //Pin 5 von Port D als Ausgang fuer Impuls-Echo
   PORTD &= ~(1<<ECHOPIN); //LO
   
   DDRD |=(1<<ANALOGPIN);
   PORTD &= ~(1<<ANALOGPIN); //LO
   
   // Eventuell: PORTD5 verwenden, Relais auf Platine
   
   //  DDRD &=~(1<<INT0PIN); //Pin 2 von Port D als Eingang fuer Interrupt Impuls
   //	PORTD |=(1<<INT0PIN); //HI
   DDRD &=~(1<<INT1PIN); //Pin 3 von Port D als Eingang fuer Interrupt Impuls
   PORTD |=(1<<INT1PIN); //HI
   
   
   DDRD &= ~(1<<MASTERCONTROLPIN); // Pin 4 von PORT D als Eingang fuer MasterControl
   PORTD |= (1<<MASTERCONTROLPIN);	// HI
   
   
   
}

void initOSZI(void)
{
   OSZIPORTDDR |= (1<<PULS);
   OSZIPORT |= (1<<PULS); // HI
}

void lcdinit()
{
   //*********************************************************************
   //	Definitionen LCD im Hauptprogramm
   //	Definitionen in lcd.h
   //*********************************************************************
   
   LCD_DDR |= (1<<LCD_RSDS_PIN); //PIN 3 von PORT D als Ausgang fuer LCD_RSDS_PIN
   LCD_DDR |= (1<<LCD_ENABLE_PIN); //PIN 4 von PORT D als Ausgang fuer LCD_ENABLE_PIN
   LCD_DDR |= (1<<LCD_CLOCK_PIN); //PIN 5 von PORT D als Ausgang fuer LCD_CLOCK_PIN
   /* initialize the LCD */
   lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
   lcd_puts("LCD Init\0");
   delay_ms(300);
   lcd_cls();
   
}

/*
 http://www.lothar-miller.de/s9y/archives/25-Filter-in-C.html
 Filter in C
 
 Ein Filter in der Art eines RC-Filters (PT1-Glied) kann in einem uC relativ leicht implementiert werden. Dazu bedarf es, wie beim RC-Glied eines "Summenspeichers" (der Kondensator) und einer Gewichtung (Widerstand bzw. Zeitkonstante).
 */
/*
 unsigned long mittelwert(unsigned long newval)
 {
 static unsigned long avgsum = 0;
 // Filterlängen in 2er-Potenzen --> Compiler optimiert
 avgsum -= avgsum/128;
 avgsum += newval;
 return avgsum/128;
 }
 */
/*
 Etwas spannender wird es, wenn die Initialisierung des Startwertes nicht so lange dauern soll. Wenn ich beispielsweise nach dem 1. Messwert diesen Wert und ab dem 2. Messwert bis zur Filterbreite den Mittelwert aus allen Messungen haben möchte, dann ist eine andere Behandlung der Initialisierung nötig.
 */
unsigned long mittelwert(unsigned long newval,int faktor)
{
   static short n = 0;
   static unsigned long avgsum = 0;
   if (n<faktor)
   {
      n++;
      avgsum += newval;
      return avgsum/n;
   }
   else
   {
      // Konstanten kann der Compiler besser optimieren
      avgsum -= avgsum/faktor;
      avgsum += newval;
      return avgsum/faktor;
   }
}

unsigned long gleitmittelwert(unsigned long newval,int faktor)
{
   static short n = 0;
   static unsigned long avgsum = 0;
   if (n<faktor)
   {
      n++;
      avgsum += newval;
      return avgsum/n;
   }
   else
   {
      // Konstanten kann der Compiler besser optimieren
      avgsum -= avgsum/faktor;
      avgsum += newval;
      return avgsum/faktor;
      
   }
}




/*
 
 Der Aufruf erfolgt z.B. anhand eines Timer-Flags:
 
 int main(int argc, char* argv[])
 {
 :
 while(1) {
 if(ti.Akt>ti.Calc) {
 avgwert = mittelwert(value[i]);
 printf("i: %3d --> Wert: %d --> Mittelwert: %d\n",i, value[i], mw);
 }
 }
 }
 
 */




void WDT_off(void)
{
   cli();
   wdt_reset();
   /* Clear WDRF in MCUSR */
   MCUSR &= ~(1<<WDRF);
   /* Write logical one to WDCE and WDE */
   /* Keep old prescaler setting to prevent unintentional time-out
    */
   WDTCSR |= (1<<WDCE) | (1<<WDE);
   /* Turn off WDT */
   WDTCSR = 0x00;
   sei();
}

uint8_t i=0;

/*
 int hostsuchen (int argc, char **argv)
 {
 struct hostent *he;
 struct in_addr a;
 
 if (argc != 2)
 {
 fprintf(stderr, "usage: %s hostname\n", argv[0]);
 return 1;
 }
 he = gethostbyname (argv[1]);
 if (he)
 {
 printf("name: %s\n", he->h_name);
 while (*he->h_aliases)
 printf("alias: %s\n", *he->h_aliases++);
 while (*he->h_addr_list)
 {
 bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
 printf("address: %s\n", inet_ntoa(a));
 }
 }
 else
 herror(argv[0]);
 return 0;
 }
 */
/* ************************************************************************ */
/* Ende Eigene Funktionen														*/
/* ************************************************************************ */


#pragma mark main
int main(void)
{
   if (TEST)
   {
      myip[3] = 216;
   }
   
   /* ************************************************************************ */
   /* Eigene Main														*/
   /* ************************************************************************ */
   //JTAG deaktivieren (datasheet 231)
   //	MCUCSR |=(1<<7);
   //	MCUCSR |=(1<<7);
   
   MCUSR = 0;
   wdt_disable();
   //SLAVE
   //uint16_t Tastenprellen=0x0fff;
   uint16_t loopcount0=0;
   uint16_t loopcount1=0;
   
   // ETH
   //uint16_t plen;
   uint8_t i=0;
   int8_t cmd;
   
   
   // set the clock speed to "no pre-scaler" (8MHz with internal osc or
   // full external speed)
   // set the clock prescaler. First write CLKPCE to enable setting of clock the
   // next four instructions.
   CLKPR=(1<<CLKPCE); // change enable
   CLKPR=0; // "no pre-scaler"
   delay_ms(1);
   
   i=1;
   //	WDT_off();
   //init the ethernet/ip layer:
   //	init_ip_arp_udp_tcp(mymac,myip,MYWWWPORT);
   // timer_init();
   
   //     sei(); // interrupt enable
   master_init();
   
   lcdinit();
   lcd_puts("Guten Tag \0");
   lcd_gotoxy(10,0);
   lcd_puts("V:\0");
   lcd_puts(VERSION);
   /*
    char versionnummer[7];
    strcpy(versionnummer,&VERSION[13]);
    versionnummer[6] = '\0';
    lcd_puts(versionnummer);
    */
   delay_ms(1600);
   //lcd_cls();
   
   TWBR =0;
   
   
   
   //Init_SPI_Master();
   initOSZI();
   /* ************************************************************************ */
   /* Ende Eigene Main														*/
   /* ************************************************************************ */
   
   uint16_t dat_p;
   char str[30];
   
   // set the clock speed to "no pre-scaler" (8MHz with internal osc or
   // full external speed)
   // set the clock prescaler. First write CLKPCE to enable setting of clock the
   // next four instructions.
   CLKPR=(1<<CLKPCE); // change enable
   CLKPR=0; // "no pre-scaler"
   _delay_loop_1(0); // 60us
   
   /*initialize enc28j60*/
   enc28j60Init(mymac);
   enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
   _delay_loop_1(0); // 60us
   
   
   
   /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
   // LEDB=yellow LEDA=green
   //
   // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
   // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
   
   
   enc28j60PhyWrite(PHLCON,0x476);
   
   //DDRB|= (1<<DDB1); // LED, enable PB1, LED as output
   //PORTD &=~(1<<PD0);;
   
   //init the web server ethernet/ip layer:
   
   if (TEST)
   {
      init_ip_arp_udp_tcp(mymac,myip,MYWWWTESTPORT);
   }
   else
   {
      init_ip_arp_udp_tcp(mymac,myip,MYWWWPORT);
   }
   
   // init the web client:
   client_set_gwip(gwip);  // e.g internal IP of dsl router
   
   
   //   client_set_wwwip(websrvip);
   
#pragma mark eeprom
   // websrvip[4] = {217,26,53,231}
   uint8_t temp0 = 217;
   uint8_t temp1 = 26;
   uint8_t temp2 = 53;
   uint8_t temp3 = 231;
   eeprom_write_byte(&ip0, temp0);
   eeprom_write_byte(&ip1, temp1);
   eeprom_write_byte(&ip2, temp2);
   eeprom_write_byte(&ip3, temp3);
   volatile uint8_t temparray[] = {192,168,1,215};
   //lcd_gotoxy(10,3);
   //lcd_putint(temparray[0]);
   
   // void eeprom_read_block(void * __dst,const void * __src,size_t __n)
   
   
   
   // eeprom_write_block((const void*)iparray,(void*)temparray,sizeof(temparray)+1);
   
   /* funktioniert, loeshcht aber ip0..
    {
    uint8_t localBlock[] = {0,1,'2','3','4','5','6','7','8','9'};
    uint16_t size = 10;
    uint16_t address = 0;
    
    eeprom_write_block((const void*)temparray,(void*)address,size);
    }
    _delay_ms(5);
    
    {
    uint8_t localBlock[10];
    uint16_t size = 10;
    uint16_t address = 0;
    
    eeprom_read_block((void*)&localBlock,(const void*)address,size);
    
    lcd_gotoxy(0,2);
    lcd_putint(localBlock[0]);
    lcd_putc('.');
    lcd_putint(localBlock[1]);
    lcd_putc('.');
    lcd_putint(localBlock[2]);
    lcd_putc('.');
    lcd_putint(localBlock[3]);
    
    }
    
    */
   
   _delay_ms(5);
   /*
    http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__eeprom_1ga0ebd0e867b6f4a03d053801d3508f8de.html
    void eeprom_read_block(void * __dst, const void * __src, size_t __n)
    */
   volatile uint8_t tempiparray[4];
   //eeprom_read_block((void*)&tempiparray,(const void*)&iparray,sizeof(tempiparray)+1);
   
   
   uint8_t i0 = eeprom_read_byte(&ip0);
   lcd_gotoxy(0,3);
   lcd_putint(i0);
   
   /*
    tempiparray[0] = eeprom_read_byte(&ip0);
    tempiparray[1] = eeprom_read_byte(&ip1);
    tempiparray[2] = eeprom_read_byte(&ip2);
    tempiparray[3] = eeprom_read_byte(&ip3);
    eeprom_write_byte(&ip0, ++i0);
    
    lcd_gotoxy(0,2);
    lcd_putint(tempiparray[0]);
    lcd_putc('.');
    lcd_putint(tempiparray[1]);
    lcd_putc('.');
    lcd_putint(tempiparray[2]);
    lcd_putc('.');
    lcd_putint(tempiparray[3]);
    */
   websrvip[0] = eeprom_read_byte(&ip0);
   websrvip[1] = eeprom_read_byte(&ip1);
   websrvip[2] = eeprom_read_byte(&ip2);
   websrvip[3] = eeprom_read_byte(&ip3);
   // eeprom_write_byte(&ip0, ++i0);
   
   lcd_gotoxy(0,2);
   lcd_putint(websrvip[0]);
   lcd_putc('.');
   lcd_putint(websrvip[1]);
   lcd_putc('.');
   lcd_putint(websrvip[2]);
   lcd_putc('.');
   lcd_putint(websrvip[3]);
   
   
   client_set_wwwip(websrvip);
   
   
   register_ping_rec_callback(&ping_callback);
   
   
   
   timer0();
   
   //
   lcd_clr_line(1);
   lcd_gotoxy(0,0);
   lcd_puts("          \0");
   InitCurrent();
   timer2();
   
   static volatile uint8_t paketcounter=0;
   
   // client_set_wwwip(websrvip);
   
#pragma  mark "while"
   
   //   webstatus |= (1<<DATASEND);
   sei();
   lcd_clr_line(0);
   while(1)
   {
      //n = rand() % 100 + 1;
      sei();
      //      usbPoll();
      //Blinkanzeige
      loopcount0++;
      if (loopcount0>=0x0FFF)
      {
         loopcount0=0;
         
         if (webstatus & (1<<DATAPEND)&& loopcount1 > 6) // callback simulieren
         {
            callbackerrcounter++;
            errstatus |= (1<<CALLBACKERR);
            
            webstatus &= ~(1<<DATASEND);
            
            webstatus &= ~(1<<DATAPEND);
            
            // Messungen wieder starten
            
            webstatus &= ~(1<<CURRENTSTOP);
            webstatus |= (1<<CURRENTWAIT); // Beim naechsten Impuls Messungen wieder starten
            sei();
            
            
            //loopcount1=0;
         }
         
         
         
         if (loopcount1 >= 0x040)
         {
            
            loopcount1 = 0;
            //OSZITOGG;
            LOOPLEDPORT ^= (1<<SENDLED);           // TWILED setzen, Warnung
            //				webstatus |= (1<<DATALOOP);
            //TWBR=0;
            //lcdinit();
            //lcd_gotoxy(0,0);
            //lcd_putc('a');
            uint8_t i0 = eeprom_read_byte(&ip0);
            //           lcd_gotoxy(5,3);
            //          lcd_putint(i0);
            eeprom_write_byte(&ip0, ++i0);
         }
         else
         {
            loopcount1++;
            if (loopcount1 == 20)
            {
               //lcd_gotoxy(16,1);
               //lcd_putc(' ');
            }
            
            
         }
         LOOPLEDPORT ^=(1<<LOOPLED);
      }
      
      //**	Beginn Current-Routinen	***********************
      
      if (currentstatus & (1<<IMPULSBIT)) // neuer Impuls angekommen, Zaehlung lauft
      {
         PORTD |=(1<<ECHOPIN);
         
         messungcounter ++;
         currentstatus++; // ein Wert mehr gemessen
         
         impulszeitsumme += impulszeit/ANZAHLWERTE;      // Wert aufsummieren
         
         if (filtercount == 0) // neues Paket
         {
            filtermittelwert = impulszeit;
         }
         else
         {
            if (filtercount < filterfaktor)
            {
               filtermittelwert = ((filtercount-1)* filtermittelwert + impulszeit)/filtercount;
               
               //lcd_gotoxy(19,1);
               //lcd_putc('a');
               
            }
            else
            {
               filtermittelwert = ((filterfaktor-1)* filtermittelwert + impulszeit)/filterfaktor;
               
               
               //lcd_gotoxy(19,1);
               //lcd_putc('f');
               
            }
            
         }
         
         char filterstromstring[8];
         filtercount++;
         /*
          lcd_gotoxy(0,0);
          lcd_putint16(impulszeit/100);
          lcd_gotoxy(10,0);
          lcd_putint16(filtermittelwert/100);
          lcd_gotoxy(10,1);
          lcd_putint(filtercount);
          lcd_putc('*');
          
          dtostrf(filtermittelwert,5,1,filterstromstring);
          //lcd_puts(filterstromstring);
          */
         
         if (filtercount & (filterfaktor == 0)) // Wert anzeigen
         {
            //lcd_gotoxy(10,0);
            //lcd_putint16(filtermittelwert);
            //lcd_putc('*');
            //dtostrf(filtermittelwert,5,1,filterstromstring);
            //lcd_puts(filterstromstring);
         }
         
         
         if ((currentstatus & 0x0F) == ANZAHLWERTE)      // genuegend Werte
         {
            
            lcd_gotoxy(19,0);
            lcd_putc(' ');
            //lcd_putc(' ');
            //lcd_gotoxy(6,1);
            //lcd_putc(' ');
            
            //lcd_gotoxy(16,1);
            //lcd_puts("  \0");
            //lcd_gotoxy(0,1);
            //lcd_puts("    \0");
            
            //lcd_gotoxy(0,1);
            //lcd_putint(messungcounter);
            
            paketcounter++;
            
            //lcd_gotoxy(0,0);
            //lcd_puts("  \0");
            /*
             if ((paketcounter & 1)==0)
             {
             lcd_gotoxy(8,1);
             lcd_putc(':');
             
             }
             else
             {
             lcd_gotoxy(8,1);
             lcd_putc(' ');
             }
             */
            //lcd_gotoxy(0,0);
            //lcd_putint2(paketcounter);
            
            
            impulsmittelwert = impulszeitsumme;
            impulszeitsumme = 0;
            
            currentstatus &= 0xF0; // Bit 0-3 reset
            
            
            //uint8_t lb= impulsmittelwert & 0xFF;
            //uint8_t hb = (impulsmittelwert>>8) & 0xFF;
            
            //                lcd_gotoxy(0,1);
            //               lcd_putc('I');
            //lcd_puts("INT0 \0");
            
            //lcd_puthex(hb);
            //lcd_puthex(lb);
            //lcd_putc(':');
            
            //char impstring[12];
            //dtostrf(impulsmittelwert,8,2,impstring);
            lcd_gotoxy(0,0);
            //lcd_puts(impstring);
            //lcd_putc(':');
            lcd_putint16(impulsmittelwert);
            lcd_putc('*');
            
            
            // lcd_gotoxy(5,0);
            // lcd_putint(sendintervallzeit);
            // lcd_putc('$');
            
            /*
             Impulsdauer: impulsmittelwert * TIMERIMPULSDAUER (10us)
             Umrechnung auf ms: /1000
             Energie pro Zählerimpuls: 360 Ws
             Leistung: (Energie pro Zählerimpuls)/Impulsabstand
             Umrechnung auf Sekunden: *1000
             Faktor: *100000
             */
            
            //     leistung = 0xFFFF/impulsmittelwert;
            if (impulsmittelwert)
            {
               leistung = 360.0/impulsmittelwert*100000.0;// 480us
               
               // webleistung = (uint32_t)360.0/impulsmittelwert*1000000.0;
               webleistung = (uint32_t)360.0/impulsmittelwert*100000.0;
               
               
               lcd_gotoxy(0,1);
               lcd_putint16(webleistung);
               lcd_putc('*');
            }
            
            //     Stromzaehler
            
            wattstunden = impulscount/10; // 310us
            
            
            //OSZILO;
            /*
             // ganze Anzeige 55 ms
             lcd_gotoxy(9,1);
             lcd_putint(wattstunden/1000);
             lcd_putc('.');
             lcd_putint3(wattstunden);
             lcd_putc('W');
             lcd_putc('h');
             */
            //OSZIHI;
            
            
            // dtostrf(leistung,5,0,stromstring); // fuehrt zu 'strom=++123' in URL fuer strom.pl. Funktionierte trotzdem
            
            //         dtostrf(leistung,5,1,stromstring); // 800us
            
            
            //lcd_gotoxy(0,0);
            //lcd_putc('L');
            //lcd_putc(':');
            
            
            if (!(paketcounter == 1))
            {
               
               //lcd_puts("     \0");
               
               //lcd_gotoxy(2,0);
               //lcd_puts(stromstring);
               //lcd_putc(' ');
               //lcd_putc('W');
            }
            //lcd_putc('*');
            //lcd_putc(' ');
            //lcd_putint16(leistung);
            //lcd_putc(' ');
            
            /*
             if (abs(leistung-lastleistung) > 10)
             {
             lastcounter++;
             
             if (lastcounter>3)
             {
             char diff[10];
             dtostrf(leistung-lastleistung,7,2,diff);
             lcd_gotoxy(10,1);
             lcd_putc('D');
             lcd_putc(':');
             lcd_puts(diff);
             lastleistung = leistung;
             }
             }
             else
             {
             lastcounter=0;
             }
             */
            
            // if (paketcounter  >= ANZAHLPAKETE)
            if (webstatus & (1<<DATALOOP))
            {
               
               webstatus &= ~(1<<DATALOOP);
               
               //uint16_t zufall = rand() % 0x0F + 1;;
               
               //lcd_putc(' ');
               //lcd_putint12(zufall);
               //leistung += zufall;
               
               
               
               //dtostrf(leistung,5,1,stromstring); // 800us
               dtostrf(webleistung,10,0,stromstring); // 800us
               
               
               paketcounter=0;
               
               uint16_t tempmitte = 0;
               for (i=0;i<4;i++)
               {
                  tempmitte+= stromimpulsmittelwertarray[i];
               }
               tempmitte/= 4;
               lcd_gotoxy(14,0);
               lcd_putc('m');
               lcd_putint12(tempmitte);
               //         filtercount =0;
               
               //if (TEST)
               {
                  //lcd_gotoxy(0,0);
                  //lcd_putint(messungcounter);
                  //lcd_putc(' ');
                  //OSZILO;
                  
                  
                  lcd_gotoxy(9,1);
                  lcd_putint(wattstunden/1000);
                  lcd_putc('.');
                  lcd_putint3(wattstunden);
                  lcd_putc('W');
                  lcd_putc('h');
                  
                  //OSZIHI;
               }
               
               if (!(webstatus & (1<<DATAPEND))) // wartet nicht auf callback
               {
                  // stromstring bilden
                  char key1[]="pw=\0";
                  char sstr[]="Pong\0";
                  
                  strcpy(CurrentDataString,key1);
                  strcat(CurrentDataString,sstr);
                  
                  strcat(CurrentDataString,"&strom0=\0");
                  char webstromstring[16]={};
                  
                  //      urlencode(stromstring,webstromstring);
                  
                  strcpy(webstromstring,stromstring);
                  lcd_gotoxy(0,2);
                  //lcd_puts(stromstring);
                  //lcd_putc('-');
                  //lcd_puts(webstromstring);
                  //lcd_putc('-');
                  
                  char* tempstromstring = (char*)trimwhitespace(webstromstring);
                  lcd_puts(tempstromstring);
                  //lcd_putc('$');
                  
                  //strcat(CurrentDataString,stromstring);
                  strcat(CurrentDataString,tempstromstring);
                  
                  /*
                   // kontolle
                   char substr[20];
                   uint8_t l=strlen(CurrentDataString);
                   strncpy(substr, CurrentDataString+8, (l));
                   lcd_gotoxy(0,1);
                   lcd_puts(substr);
                   _delay_ms(10);
                   */
               }
               
               
               
               // senden aktivieren
               webstatus |= (1<<DATASEND);
               webstatus |= (1<<DATAOK);
               
               webstatus |= (1<<CURRENTSTOP);
               
               webstatus |= (1<<CURRENTWAIT);
               
               paketcounter=0;
               //sendWebCount++;
               //           lcd_gotoxy(6,1);
               //           lcd_putc('>');
               
               
            } // if DATALOOP
            
            //anzeigewert = 0xFF/0x8000*leistung; // 0x8000/0x255 = 0x81
            
            //anzeigewert = leistung/0x81;
            
            anzeigewert = leistung /0x18;
            
            // if (TEST)
            {
               lcd_gotoxy(9,0);
               lcd_putint(anzeigewert);
            }
            
            webstatus |= (1<<CURRENTSEND);
            
         } // genuegend Werte
         else
         {
            //lcd_gotoxy(8,1);
            //lcd_puts("    \0");
            
         }
         
         PORTD &= ~(1<<ECHOPIN);
         impulszeit=0;
         currentstatus &= ~(1<<IMPULSBIT);
         
         
      }
      //**    End Current-Routinen*************************
      
      
      if (sendWebCount >2)
      {
         //start_web_client=1;
         sendWebCount=0;
      }
      
      webstatus |= (1<<DATASEND);
      
      // strom busy?
      if (webstatus & (1<<DATASEND))
      {
#pragma mark packetloop
         
         // **	Beginn Ethernet-Routinen	***********************
         
         // handle ping and wait for a tcp packet
         
         //cli();
         
         dat_p=packetloop_icmp_tcp(buf,enc28j60PacketReceive(BUFFER_SIZE, buf));
         //dat_p=1;
         
         if(dat_p==0) // Kein Aufruf, eigene Daten senden an Homeserver
         {
            
            if ((start_web_client==1)) // In Ping_Calback gesetzt: Ping erhalten
            {
               //OSZILO;
               
               //lcd_gotoxy(0,0);
               //lcd_puts("    \0");
               lcd_gotoxy(12,0);
               lcd_puts("ping ok\0");
               lcd_clr_line(1);
               delay_ms(100);
               start_web_client=2; // nur erstes ping beantworten. start_web_client wird in pl-send auf 0 gesetzt
               
               mk_net_str(str,pingsrcip,4,'.',10);
               char* pingsstr="ideur01\0";
               
               urlencode(pingsstr,urlvarstr);
               //lcd_gotoxy(0,1);
               //lcd_puts(urlvarstr);
               //delay_ms(1000);
               
            }
            
            //if (sendWebCount == 2) // StromDaten an HomeServer schicken
            if (webstatus & (1<<DATAOK) )
            {
               //lcd_clr_line(2);
               //OSZILO;
               
               // start_web_client=2;
               //strcat("pw=Pong&strom=360\0",(char*)teststring);
               
               
               start_web_client=0; // ping wieder ermoeglichen
               
               // Daten an strom.pl schicken
               
               client_browse_url((char*)PSTR("/cgi-bin/strom.pl?"),CurrentDataString,(char*)PSTR(WEBSERVER_VHOST),&strom_browserresult_callback);
               
               sendWebCount++;
               lcd_gotoxy(0,3);
               lcd_putint(sendWebCount);
               webstatus &= ~(1<<DATAOK); // client_browse nur einmal
               webstatus |= (1<<DATAPEND);
               
               lcd_gotoxy(19,0);
               lcd_putc('>');
               
               
               //OSZIHI;
            }
            
            continue;
         } // dat_p=0
         else
         {
            
            
            // lcd_gotoxy(5,1);
            //lcd_puts("dat_p\0");
            // lcd_putint(cmd);
            
         }
         
         sei();
         
         /*
          if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0)
          {
          // head, post and other methods:
          //
          // for possible status codes see:
          
          // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
          goto SENDTCP;
          }
          */
         
         
         if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0) // Slash am Ende der URL, Status-Seite senden
         {
            lcd_gotoxy(0,0);
            lcd_puts("   \0");
            lcd_gotoxy(0,0);
            lcd_puts("+/+\0");
            dat_p=http200ok(); // Header setzen
            dat_p=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
            if (TEST)
            {
               dat_p=fill_tcp_data_p(buf,dat_p,PSTR("<h1>HomeCurrent Test 200 OK</h1>"));
            }
            else
            {
               dat_p=fill_tcp_data_p(buf,dat_p,PSTR("<h1>HomeCurrent 200 OK</h1>"));
            }
            dat_p=print_webpage_status(buf);
            goto SENDTCP;
         }
         else
         {
            // Teil der URL mit Form xyz?uv=... analysieren
            
#pragma mark cmd
            
            //out_startdaten=DATATASK;	// default
            
            // out_daten setzen
            cmd=analyse_get_url((char *)&(buf[dat_p+5]));
            lcd_gotoxy(10,1);
            //OSZIHI;
            lcd_putint12(MYWWWPORT);
            
            lcd_gotoxy(0,3);
            lcd_puts("cmd:*\0");
            lcd_putint(cmd);
            lcd_putc('*');
            if (cmd == 1)
            {
               //dat_p = print_webpage_confirm(buf);
            }
            else if (cmd == 2)	// TWI > OFF
            {
#pragma mark cmd 2
            }
            
            else if (cmd == 25)	// Data lesen
            {
#pragma mark cmd 25
               dat_p=http200ok(); // Header setzen
               dat_p=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
               dat_p = print_webpage_data(buf,(void*)CurrentDataString); // pw=Pong&strom=1234
            }
            
            else if (cmd == 16)	// data0
            {
#pragma mark cmd 16
               
               lcd_gotoxy(0,2);
               lcd_putc('*');
               
               lcd_putint(websrvip[0]);
               lcd_putc('.');
               lcd_putint(websrvip[1]);
               lcd_putc('.');
               lcd_putint(websrvip[2]);
               lcd_putc('.');
               lcd_putint(websrvip[3]);
               lcd_putc('*');
               
               client_set_wwwip(websrvip);
               
               dat_p=http200ok(); // Header setzen
               dat_p=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
               
               dat_p = print_webpage_hostip(buf,(void*)"allok");
               
            }
            
            
            else
            {
               dat_p=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Zugriff heute verweigert</h1>"));
            }
            cmd=0;
            // Eingangsdaten reseten, sofern nicht ein Status0-Wait im Gang ist:
            //if ((pendenzstatus & (1<<SEND_STATUS0_BIT)))
            {
               
            }
            
            goto SENDTCP;
         }
         //
         
      SENDTCP:
         //OSZIHI;
         www_server_reply(buf,dat_p); // send data
         
         
      } // strom not busy
      else
      {
         lcd_gotoxy(5,0);
         lcd_puts("busy\0");
         
      }
   }
   return (0);
}
