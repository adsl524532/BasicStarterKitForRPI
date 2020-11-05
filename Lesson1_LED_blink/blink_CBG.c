/*
 * Created by keen
 * Modified by keen 
 * Compiling: gcc -Wall -o blink_CBG blink_CBG.c -lwiringPi
 * Run: sudo ./blink
 * Date: 28/03/2017
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#define led_pin 0    // == GPIO pin 17

int main(int argc, char *argv[]){

int myLed_pin;

  printf( "Welcome to Elecrow...\n");
  printf( "Raspberry Pi blink program...\n" );
  printf( "Usage: %s [pinNr]\n", argv[0] );
  printf( "   default pin = 0\n");

  printf("argc = %d\n\n", argc);
  if (argc>1) {
    myLed_pin = atoi(argv[1]);
  } else {
    myLed_pin = led_pin;
  }

  printf( "Using pin %d\n", myLed_pin);
  printf( "Press Ctrl+C to exit\n..." );


  wiringPiSetup();
  pinMode(myLed_pin,OUTPUT); 
  while(1){
    digitalWrite(myLed_pin,HIGH);
    printf( "LED on...\n");
    delay (1000);
    digitalWrite(myLed_pin,  LOW);
    printf( "LED off...\n");
    delay (1000);
  }
}

