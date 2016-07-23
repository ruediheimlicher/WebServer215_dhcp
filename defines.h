//
//  defines.h
//  WebServer
//
//  Created by Ruedi Heimlicher on 23.07.2016.
//
//

#ifndef defines_h
#define defines_h

// set output to VCC, red LED off
#define LEDOFF PORTB|=(1<<PORTB1)
// set output to GND, red LED on
#define LEDON PORTB&=~(1<<PORTB1)
// to test the state of the LED
#define LEDISOFF PORTB&(1<<PORTB1)
// packet buffer
#define BUFFER_SIZE 650



//

// set output to VCC, red LED off
#define PD0LEDOFF PORTD|=(1<<PORTD0)
// set output to GND, red LED on
#define PD0LEDON PORTD&=~(1<<PORTD0)
// to test the state of the LED
#define PD0LEDISOFF PORTD&(1<<PORTD0)
//

static char* teststring = "pw=Pong&strom0=360\0";

// listen port for tcp/www:
#define MYWWWPORT 80


#endif /* defines_h */
