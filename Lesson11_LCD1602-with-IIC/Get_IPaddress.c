#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int GetIP_address(char *szAddress);

int main()
{
char szIP_address[40];

	GetIP_address(szIP_address);
        printf("%s\n", szIP_address);

	return 0;
}


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