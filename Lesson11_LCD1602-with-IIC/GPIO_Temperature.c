/*
 * Created by keen
 * Modified by keen 
 * Compiling :gcc -Wall -o GPIO_Temperature GPIO_Temperature.c -lwiringPi -lwiringPiDev
 *            gcc -Wall -o GPIO_Temperature GPIO_Temperature.c -lwiringPi -lwiringPiDev ../../Other/bme280_V2_1.o
 *
 * Run : GPIO_Temperature 
 * Date: 28/03/2017
 *   CBG:
 * 	- Show temperature from /sys/bus/w1/devices/28-021480c8caff/w1_slave
 *      -      temperature from DHT11 on wiPi pin 22
 *      -      temperature from DHT22 on wiPi pin 23
 *
 * Local functions:
 *	
 *
 */
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


// for IP-address
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

//PCF8574 Start I/O address
// PCF8754 64+8
#define AF_BASE 64
#define AF_RS (AF_BASE + 0)
#define AF_RW (AF_BASE + 1)
#define AF_E (AF_BASE + 2)
#define AF_LED (AF_BASE + 3)
#define AF_DB4 (AF_BASE + 4)
#define AF_DB5 (AF_BASE + 5)
#define AF_DB6 (AF_BASE + 6)
#define AF_DB7 (AF_BASE + 7)
// Global lcd handle:
static int lcdHandle;

// for DHT11 ======================================================
#define MAXTIMINGS  85
#define DHT11PIN    22 // CBG  wPi pin
#define DHT22PIN    23 // CBG  wPI pin
#define SENSOR_DHT11 90
#define SENSOR_DHT22 91
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

int GetIP_address(char *szAddress, char *szInterface);
float getTemperature();
int DHT_Read(int DhtPin, double *T, double *H, int SensorType);
int BME280(float *Temp, float *Hum, float *press);

int ProcessArgs(int argc, char **argv)
{  
   int i, DisplayAddress=0x3F;

   printf( "Welcome to Elecrow\n");
   printf( "Raspberry Pi LCD1602 with IIC test program\n\t LCD-address: 0x3F\n\n" );
   
   printf("number of args: %d\n", argc);

   for (i=0;i < argc; i++) 
      printf("arg %d: %s\n", i, argv[i]);
   
   printf("Usage: %s [display address]\n\n", argv[0]);
   printf("\t display address =  [0x3F (default) | 0x27]\n");
   
   
   if (argc>1) {
      sscanf(argv[1], "%x", &DisplayAddress);
      printf("Address: %x \n", DisplayAddress);
   }


/*
*/

   return DisplayAddress;

}
//===================================================================
// print the time (no line feed)

char *PrintTime(void)
{
   time_t curTime;
   char *szTime;

   time(&curTime);
   szTime = ctime(&curTime);
//   szTime[0] = 'C';           // just for fun modfiy the time string
   printf("Time: %s", szTime);  // note: szTime includes newline

   return(szTime);
}

//===================================================================
int main(int argc, char**argv)
{
	int i;
	float T, T2;
//	char szIP_address[40];
        char szT[40], szT2[40];
	wiringPiSetup();        //Initialise WiringPi

	int DisplayAddress = ProcessArgs(argc, argv);


	pcf8574Setup(AF_BASE, DisplayAddress);
//	lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0) ;
	lcdHandle = lcdInit (4, 20, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0) ;  // LCD 20x4

	if (lcdHandle < 0) {
   		printf ("lcdInit failed\n") ;
		exit (EXIT_FAILURE) ;
	}

	for(i=0;i<8;i++)
		pinMode(AF_BASE+i,OUTPUT); 	//Will expand the IO port as the output mode
	
	digitalWrite(AF_LED,1); 		//Open back light
	digitalWrite(AF_RW,0); 			//Set the R/Wall to a low level, LCD for the write state
	
	while(1) {
//	   lcdClear(lcdHandle); 			//Clear display
//	   sleep(1);
	
//	   lcdPuts(lcdHandle, "Raspberry PI");  	//Print the text on the LCD at the current cursor postion

//           GetIP_address(szIP_address, "eth0");
//	   lcdPosition(lcdHandle,0,0);           	//Position cursor on the first line in the first column
//	   lcdPuts(lcdHandle, szIP_address);  	//Print the text on the LCD at the current cursor postion
//	   sleep(1);

//	   lcdClear(lcdHandle); 	//Clear display
//	   lcdPosition(lcdHandle,0,0);          
//	   GetIP_address(szIP_address, "wlan0");
//	   lcdPuts(lcdHandle, szIP_address);

	   printf("\n");  // new line
	   char *szTime = PrintTime();
	   lcdPosition(lcdHandle,0,0);          
	   lcdPuts(lcdHandle, szTime);


           getTemperature(&T, &T2);   	// DS18B20 takes about 1 sec to execute
           sprintf(szT , "DS18B20: T=%.3f", T);
           sprintf(szT2, "DS18B20: T=%.3f : %.4f", T2, T - T2);
           printf("%s\n",szT);
           printf("%s\n",szT2);

	   lcdPosition(lcdHandle,0,1);          
//	   lcdClear(lcdHandle); 	//Clear display
	   lcdPuts(lcdHandle, szT);    	// DS18B20

           int Error;
           double T2, H2, T3, H3;

//           int DhtPin = DHT11PIN;
//           int DhtPin = DHT22PIN;

	   printf("DHT22: \n");
	   Error = DHT_Read(DHT22PIN, &T2, &H2, SENSOR_DHT22);		
           if (!Error) {
   	       lcdPosition(lcdHandle,0,2);      // DHT22
       	       lcdPrintf(lcdHandle,"T=%5.1fC; Hum=%5.1f%%", T2, H2);
           } else {
		printf("\n");
           }

           sleep(5);
	   printf("DHT11: \n");
	   Error = DHT_Read(DHT11PIN, &T3, &H3, SENSOR_DHT11);		
           if (!Error) {
   	       lcdPosition(lcdHandle,0,3);     	// DHT11    
       	       lcdPrintf(lcdHandle,"T=%5.1fC; Hum=%5.1f%%", T3, H3);
           } else {
		printf("\n");
           }

	   // data from BME280:
 	   float cTemp, humidity, pressure;
	   BME280(&cTemp, &humidity,&pressure);

	   //Write_Shell(cTemp, humidity, pressure);

	   // Output data to screen
	   printf("Temperature in Celsius : %8.2f C \n", cTemp);
	   printf("Pressure               : %8.2f hPa \n", pressure);
	   printf("Relative Humidity      : %8.2f RH \n", humidity);

	   // write to file
 	   char *szFilename = "/home/pi/Chris/Logs/Temperature.log";
           FILE *fp = fopen(szFilename, "w");
           if (fp) {
              fprintf(fp,"Test\n");
              fprintf(fp,"%s, %.1f, %.1f, %.1f, %.1f, %s", szT, T2, H2, T3, H3, szTime);
              fprintf(fp,"Test\n");
              fclose(fp);
           } else
	      printf("Cannot open file %s\n", szFilename);

	   char *szFilename2 = "/home/pi/Chris/Logs/Upload_Temperature.sh";

           FILE *fp2 = fopen(szFilename2, "w");
           if (fp2) {
              fprintf(fp2,"/usr/bin/curl \"https://api.thingspeak.com/update?api_key=8G8DM5II9O7BC9EG");
              fprintf(fp2,"&field1=%.3f&field2=%.2f&field3=%.2f&field4=%.2f&field5=%.2f&field6=%.2f&field7=%.2f&field8=%.2f\"\n", T, T3, H3, T2, H2, cTemp, humidity, pressure);
              fclose(fp2);
           } else
	      printf("Cannot open file %s\n", szFilename2);
   	
	   sleep(14);
	}
	return 0;
}
/* ============================================================================

=================================================================================*/
int GetIP_address(char *szAddress, char *szInterface)

{
 int fd;
 struct ifreq ifr;
 //char sz[40];

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, szInterface, IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 /* display result */
 printf("%s:  %s\n", szInterface, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

 sprintf(szAddress, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));




/*
 strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
 ioctl(fd, SIOCGIFADDR, &ifr);
 printf("wlan0: %s\n", szAddress);
*/

 close(fd);

 return 0;
}

/* ===================================
   get temeprature from DS18B20
*/
float getTemperature(float *T1, float *T2)   
{
   FILE* fd;
   char szString1[50], szString2[50];

   fd = fopen("/sys/bus/w1/devices/28-021480c8caff/w1_slave", "r");
   
   fgets(szString1, 49, fd);
   fgets(szString2, 49, fd);
   fclose(fd);
   *T1 =atof(szString2+29)/1000;  


   fd = fopen("/sys/bus/w1/devices/28-3c01d607b280/w1_slave", "r");
   
   fgets(szString1, 49, fd);
   fgets(szString2, 49, fd);
   fclose(fd);
   *T2 =atof(szString2+29)/1000;  



   return(0);

}



int DHT_Read(int DhtPin, double *T, double *H, int SensorType)
// return -1 on error
//         0 on success
{
    uint8_t laststate   = HIGH;
    uint8_t counter     = 0;
    uint8_t j       = 0, i;


    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode( DhtPin, OUTPUT );
    digitalWrite( DhtPin, LOW );
    delay( 18 );

    /* then pull it up for 40 microseconds */
    digitalWrite( DhtPin, HIGH );
    delayMicroseconds( 40 );

    /* prepare to read the pin */
    pinMode( DhtPin, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DhtPin ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DhtPin );

        if ( counter == 255 )
            break;

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            dht11_dat[j / 8] <<= 1;
            if ( counter > 16 )
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }

   /*
    * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
    * print it out if data is good
    */
   if (j<40) {
//	fprintf(stderr, "did not read 40 bits\n");
	printf("did not read 40 bits\n");
	return(-1);     // T, H not changed
   }

   char message1[] = "Temp.= 00.0 C";
   char message2[] = "Hum. = 00.0 %";
 

   if ( (j >= 40) &&(dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) ) {
      
      if (SensorType==SENSOR_DHT11) {                 // DHT11
         *H = dht11_dat[0] + dht11_dat[1]/10.;
         *T = dht11_dat[2] + dht11_dat[3]/10.;

         printf("DHT11 Sux T = %d.%d, H = %d.%d\n", dht11_dat[2], dht11_dat[3], dht11_dat[0], dht11_dat[1]);
         printf("DHT11 Sub:T = %5.2f, H = %5.2f\n", *T, *H);

         message1[ 7] = dht11_dat[2] / 10 + 48;
         message1[ 8] = dht11_dat[2] % 10 + 48;
         message1[10] = dht11_dat[3] / 10 + 48;
//         message1[11] = 223;   // degree symbol

         message2[ 7] = dht11_dat[0] / 10 + 48;
         message2[ 8] = dht11_dat[0] % 10 + 48;
         message2[10] = dht11_dat[1] / 10 + 48;

      } else {    // DHT22   AM2302
      
         int Temp = dht11_dat[2];
         Temp = (Temp << 8) | dht11_dat[3];
         int Hum  = dht11_dat[0];
         Hum  = (Hum  << 8) | dht11_dat[1];

         *H = Hum/10. ;
         *T = Temp/10.;
//         printf("Sux T = %d.%d, H = %d.%d\n", dht11_dat[2], dht11_dat[3], dht11_dat[0], dht11_dat[1]);

         printf("DHT22 Sub:T = %5.2f, H = %5.2f\n", *T, *H);
         message1[ 7] = (Temp / 100) % 10 + 48;
         message1[ 8] = (Temp / 10 ) % 10 + 48;
         message1[10] =  Temp        % 10 + 48;
//         message1[11] = 223;   // degree symbol

         message2[ 7] = (Hum  / 100) % 10 + 48;
         message2[ 8] = (Hum  / 10 ) % 10 + 48;
         message2[10] =  Hum         % 10 + 48;
      }

      printf("%s\n", message1);
      printf("%s\n", message2);


      return (0);

   } else {
      printf("Reading error\n");
   }
    
   return(-1);    // T, H not changed
}

   
