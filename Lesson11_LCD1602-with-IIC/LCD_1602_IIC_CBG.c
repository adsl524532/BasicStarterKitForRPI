/*
 * Created by keen
 * Modified by keen 
 * Compiling :gcc -Wall -o LCD_1602_IIC_CBG LCD_1602_IIC_CBG.c -lwiringPi -lwiringPiDev
 * Run : LCD_1602_IIC 
 * Date: 28/03/2017
 */
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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



int GetIP_address(char *szAddress);

int main(int argc, char**argv)
{
	int i;
	char szIP_address[40];
	wiringPiSetup();        //Initialise WiringPi

	printf( "Welcome to Elecrow\n");
	printf( "Raspberry Pi LCD1602 with IIC test program\n" );

	pcf8574Setup(AF_BASE,0x3F);
	lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0) ;

	if (lcdHandle < 0) {
   		fprintf (stderr, "lcdInit failed\n") ;
		exit (EXIT_FAILURE) ;
	}

	for(i=0;i<8;i++)
		pinMode(AF_BASE+i,OUTPUT); 	//Will expand the IO port as the output mode
	
	digitalWrite(AF_LED,1); 		//Open back light
	digitalWrite(AF_RW,0); 			//Set the R/Wall to a low level, LCD for the write state
	
	lcdClear(lcdHandle); 			//Clear display
	lcdPosition(lcdHandle,0,0);           	//Position cursor on the first line in the first column
	
        GetIP_address(szIP_address);
	lcdPuts(lcdHandle, "Raspberry PI");  	//Print the text on the LCD at the current cursor postion
	lcdPosition(lcdHandle,0,1);          

	lcdPuts(lcdHandle, szIP_address);

	return 0;
}
/* ============================================================================

=================================================================================*/
int GetIP_address(char *szAddress)

{
 int fd;
 struct ifreq ifr;
 //char sz[40];

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 /* display result */
 printf("eth0:  %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));


 strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
 ioctl(fd, SIOCGIFADDR, &ifr);
 sprintf(szAddress, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
 printf("wlan0: %s\n", szAddress);


 close(fd);

 return 0;
}