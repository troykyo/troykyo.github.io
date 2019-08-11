{\rtf1\ansi\ansicpg1252\cocoartf1348\cocoasubrtf170
{\fonttbl\f0\fmodern\fcharset0 Courier;}
{\colortbl;\red255\green255\blue255;}
\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\deftab720
\pard\pardeftab720

\f0\fs26 \cf0 \expnd0\expndtw0\kerning0
// \expnd0\expndtw0\kerning0
 hello.mic.44.c\
//	Troy Nachtigall\expnd0\expndtw0\kerning0
\
// \
// based upon hello.mic.45.c\
//\
// electret microphone hello-world\
//    9600 baud FTDI interface\
//\
// Neil Gershenfeld 4/1/14\
// (c) Massachusetts Institute of Technology 2014\
//\
// This work may be reproduced, modified, distributed,\
// performed, and displayed for any purpose. Copyright is\
// retained and must be preserved. The work is provided\
// as is; no warranty is provided, and users accept all \
// liability.\
//\
\
#include <avr/io.h>\
#include <util/delay.h>\
\
#define output(directions,pin) (directions |= pin) // set port direction for output\
#define set(port,pin) (port |= pin) // set port pin\
#define clear(port,pin) (port &= (~pin)) // clear port pin\
#define pin_test(pins,pin) (pins & pin) // test for port pin\
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set\
#define bit_delay_time 102 // bit delay for 9600 with overhead\
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay\
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay\
#define char_delay() _delay_ms(10) // char delay\
#define serial_port PORTB\
#define serial_direction DDRB\
#define serial_pin_out (1 << PB2)\
#define NPTS 100 // points in buffer\
\
\
void put_char(volatile unsigned char *port, unsigned char pin, char txchar) \{\
   //\
   // send character in txchar on port pin\
   //    assumes line driver (inverts bits)\
   //\
   // start bit\
   //\
   clear(*port,pin);\
   bit_delay();\
   //\
   // unrolled loop to write data bits\
   //\
   if bit_test(txchar,0)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,1)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,2)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,3)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,4)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,5)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,6)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   if bit_test(txchar,7)\
      set(*port,pin);\
   else\
      clear(*port,pin);\
   bit_delay();\
   //\
   // stop bit\
   //\
   set(*port,pin);\
   bit_delay();\
   //\
   // char delay\
   //\
   bit_delay();\
   \}\
\
int main(void) \{\
   //\
   // main\
   //\
   static unsigned char i,array_lo[NPTS],array_hi[NPTS];\
   //\
   // set clock divider to /1\
   //\
   CLKPR = (1 << CLKPCE);\
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);\
   //\
   // initialize output pins\
   //\
   set(serial_port, serial_pin_out);\
   output(serial_direction, serial_pin_out);\
   //\
   // init A/D\
   //\
   ADMUX = (0 << REFS2) | (1 << REFS1) | (0 << REFS0) // 1.1V ref\
      | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // 20(PB4-PB3) \
   ADCSRA = (1 << ADEN) // enable\
      | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); // prescaler /64\
   ADCSRB = (1 << BIN); // bipolar mode\
   //\
   // main loop\
   //\
   while (1) \{\
      //\
      // send framing\
      //\
      put_char(&serial_port, serial_pin_out, 1);\
      char_delay();\
      put_char(&serial_port, serial_pin_out, 2);\
      char_delay();\
      put_char(&serial_port, serial_pin_out, 3);\
      char_delay();\
      put_char(&serial_port, serial_pin_out, 4);\
      char_delay();\
      //\
      // free-running sample loop\
      //\
      for (i = 0; i < NPTS; ++i) \{\
         //\
         // initiate conversion\
         //\
         ADCSRA |= (1 << ADSC);\
         //\
         // wait for completion\
         //\
         while (ADCSRA & (1 << ADSC))\
            ;\
         //\
         // save result\
         //\
         array_lo[i] = ADCL;\
         array_hi[i] = ADCH;\
         \}\
      for (i = 0; i < NPTS; ++i) \{\
         //\
         // send result\
         //\
         put_char(&serial_port, serial_pin_out, array_lo[i]);\
         put_char(&serial_port, serial_pin_out, array_hi[i]);\
         \}\
      \}\
   \}}