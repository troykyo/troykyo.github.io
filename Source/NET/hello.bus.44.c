{\rtf1\ansi\ansicpg1252\cocoartf1348\cocoasubrtf170
{\fonttbl\f0\fmodern\fcharset0 Courier;}
{\colortbl;\red255\green255\blue255;}
\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\deftab720
\pard\pardeftab720

\f0\fs26 \cf0 \expnd0\expndtw0\kerning0
//  hello.bus.44.c\
//  Troy Nachtigall\
// \
// based upon \expnd0\expndtw0\kerning0
hello.bus.45.SLIP.c\
//\
// 9600 baud serial bus SLIP hello-world\
//\
// Neil Gershenfeld 11/18/13\
//\
// (c) Massachusetts Institute of Technology 2013\
// This work may be reproduced, modified, distributed,\
// performed, and displayed for any purpose. Copyright is\
// retained and must be preserved. The work is provided\
// as is; no warranty is provided, and users accept all \
// liability.\
//\
\
#include <avr/io.h>\
#include <util/delay.h>\
#include <string.h>\
\
#define output(directions,pin) (directions |= pin) // set port direction for output\
#define input(directions,pin) (directions &= (~pin)) // set port direction for input\
#define set(port,pin) (port |= pin) // set port pin\
#define clear(port,pin) (port &= (~pin)) // clear port pin\
#define pin_test(pins,pin) (pins & pin) // test for port pin\
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set\
#define bit_delay_time 100 // bit delay for 9600 with overhead\
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay\
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay\
#define char_delay() _delay_ms(1) // character delay\
\
#define MAX_PACKET 50 // max packet size = 20 IP + 8 UDP + payload\
\
#define END 192 // packet end\
#define ESC 219 // escape char\
#define ESC_END 220 // end escape\
#define ESC_ESC 221 // escape escape\
\
#define ADDRESS_0 10\
#define ADDRESS_1 0\
#define ADDRESS_2 0\
#define ADDRESS_3 2\
\
#define WRITE_PORT 1234\
#define READ_PORT 1235\
\
#define SOURCE_ADDRESS 12\
#define DEST_ADDRESS 16\
#define SOURCE_PORT 20\
#define DEST_PORT 22\
#define PAYLOAD 28\
\
#define led_port PORTB\
#define led_direction DDRB\
#define led_pin (1 << PB0)\
\
#define serial_port PORTB\
#define serial_direction DDRB\
#define serial_pins PINB\
#define serial_pin_in (1 << PB3)\
#define serial_pin_out (1 << PB4)\
\
void get_char(volatile unsigned char *pins, unsigned char pin, unsigned char *rxbyte) \{\
   //\
   // read character into rxbyte on pins pin\
   //    assumes line driver (inverts bits)\
   //\
   *rxbyte = 0;\
   while (pin_test(*pins,pin))\
      //\
      // wait for start bit\
      //\
      ;\
   //\
   // delay to middle of first data bit\
   //\
   half_bit_delay();\
   bit_delay();\
   //\
   // unrolled loop to read data bits\
   //\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 0);\
   else\
      *rxbyte |= (0 << 0);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 1);\
   else\
      *rxbyte |= (0 << 1);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 2);\
   else\
      *rxbyte |= (0 << 2);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 3);\
   else\
      *rxbyte |= (0 << 3);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 4);\
   else\
      *rxbyte |= (0 << 4);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 5);\
   else\
      *rxbyte |= (0 << 5);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 6);\
   else\
      *rxbyte |= (0 << 6);\
   bit_delay();\
   if pin_test(*pins,pin)\
      *rxbyte |= (1 << 7);\
   else\
      *rxbyte |= (0 << 7);\
   //\
   // wait for stop bit\
   //\
   bit_delay();\
   half_bit_delay();\
   \}\
\
void put_char(volatile unsigned char *port, unsigned char pin, unsigned char txchar) \{\
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
unsigned char get_packet(unsigned char *buf) \{\
   //\
   // get SLIP packet\
   //\
   static unsigned char chr, ptr;\
   ptr = 0;\
   while (1) \{\
      get_char(&serial_pins, serial_pin_in, &chr);\
      switch(chr) \{\
         case END:\
            return ptr;\
         case ESC:\
            get_char(&serial_pins, serial_pin_in, &chr);\
            switch(chr) \{\
               case ESC_END:\
                  chr = END;\
                  break;\
               case ESC_ESC:\
                  chr = ESC;\
                  break;\
               \}\
         default:\
            if (ptr < MAX_PACKET)\
               buf[ptr++] = chr;\
            else\
               return 0;\
         \}\
      \}\
   \}\
\
void put_packet(unsigned char *buf, unsigned char len) \{\
   //\
   // put SLIP packet\
   //\
   static unsigned char ptr,chr;\
   //\
   // CSMA check\
   //\
   /*\
   todo:\
   print_packet_CSMA:\
      ldi temp, csma_count\
	   print_packet_CSMA_loop:\
         sbis PINA, click_pin\
            rjmp print_packet_CSMA_delay\
         dec temp\
	      brne print_packet_CSMA_loop\
            rjmp print_packet_CSMA_continue\
      print_packet_CSMA_delay:\
         ldi temp, csma_count\
	      print_packet_CSMA_delay_loop:\
            dec temp\
	         brne print_packet_CSMA_delay_loop\
         rjmp print_packet_CSMA\
      print_packet_CSMA_continue:\
   */\
   //\
   // turn on output\
   //\
   output(serial_direction, serial_pin_out);   \
   //\
   // send with SLIP mapping\
   //\
   put_char(&serial_port, serial_pin_out, END);\
   for (ptr = 0; ptr < len; ++ptr) \{\
      chr = buf[ptr];\
      switch(chr) \{\
         case END:\
            put_char(&serial_port, serial_pin_out, ESC);\
            put_char(&serial_port, serial_pin_out, ESC_END);\
            break;\
         case ESC:\
            put_char(&serial_port, serial_pin_out, ESC);\
            put_char(&serial_port, serial_pin_out, ESC_ESC);\
            break;\
         default:\
            put_char(&serial_port, serial_pin_out, chr);\
         \}\
      char_delay();\
      \}   \
   put_char(&serial_port, serial_pin_out, END);\
   //\
   // turn off output\
   //    \
   input(serial_direction, serial_pin_out);   \
   \}\
\
unsigned char assemble_packet(unsigned char *buf,\
   unsigned char source_address_0,\
   unsigned char source_address_1,\
   unsigned char source_address_2,\
   unsigned char source_address_3,\
   unsigned char dest_address_0,\
   unsigned char dest_address_1,\
   unsigned char dest_address_2,\
   unsigned char dest_address_3,\
   unsigned char source_port_0,\
   unsigned char source_port_1,\
   unsigned char dest_port_0,\
   unsigned char dest_port_1,\
   unsigned char *payload,\
   unsigned char payload_length) \{\
   //\
   // assemble packet\
   //\
   unsigned char i;\
   //\
   // IP\
   //\
   buf[0] = 0x45; // version = 4, header length = 5 32-bit words\
   buf[1] = 0; // type of service\
   buf[2] = 0; // packet length high byte\
   buf[3] = 28 + payload_length; // packet length low byte\
   buf[4] = 0; // identification high byte\
   buf[5] = 0; // identification low byte\
   buf[6] = 0; // flag, fragment offset high byte\
   buf[7] = 0; // flag, fragment offset low byte\
   buf[8] = 255; // time to live\
   buf[9] = 17; // protocol = 17 for UDP\
   buf[10] = 0; // header checksum (to be calculated)\
   buf[11] = 0; // header checksum (to be calculated)\
   buf[12] = source_address_0; // source address byte 1\
   buf[13] = source_address_1; // source address byte 2\
   buf[14] = source_address_2; // source address byte 3\
   buf[15] = source_address_3; // source address byte 4\
   buf[16] = dest_address_0; // dest address byte 1\
   buf[17] = dest_address_1; // dest address byte 2\
   buf[18] = dest_address_2; // dest address byte 3\
   buf[19] = dest_address_3; // dest address byte 4\
   //\
   // UDP\
   //\
   buf[20] = source_port_0; // source port high byte\
   buf[21] = source_port_1; // source port low byte\
   buf[22] = dest_port_0; // dest port high byte\
   buf[23] = dest_port_1; // dest port low byte\
   buf[24] = 0; // payload length high byte\
   buf[25] = payload_length; // payload length low byte\
   buf[26] = 0; // payload checksum (not used)\
   buf[27] = 0; // payload checksum (not used)\
   //\
   // payload\
   //\
   for (i = 0; i < payload_length; ++i)\
      buf[28+i] = payload[i];\
   //\
   // calculate checksum\
   //\
   /*\
   todo:\
   ldi zh, high(outgoing_ip_start)\
   ldi zl, low(outgoing_ip_start)\
   ldi count, ip_header_size\
   clr check_lo\
   clr check_hi\
   clr check_carry\
   ip_checksum_loop:\
      adiw zl, 1\
      ld temp, z\
      dec count\
      sbiw zl, 1\
      ld temp1, z\
      add check_lo, temp\
      adc check_hi, temp1\
      adc check_carry, zero\
      adiw zl, 2\
      dec count\
      brne ip_checksum_loop\
   add check_lo, check_carry\
   adc check_hi, zero\
   com check_lo\
   com check_hi\
   sts outgoing_ip_checksum, check_hi\
   sts outgoing_ip_checksum+1, check_lo\
   */\
   //\
   // return\
   //\
   return (28+payload_length);\
   \}\
\
void process_packet(unsigned char *buf) \{\
   //\
   // process the packet\
   //\
   static unsigned char packet_length, payload_length;\
   static unsigned char payload[3];\
   //\
   //\
   // does the address match?\
   //\
   if ((ADDRESS_0 != buf[DEST_ADDRESS])\
    || (ADDRESS_1 != buf[DEST_ADDRESS+1])\
    || (ADDRESS_2 != buf[DEST_ADDRESS+2])\
    || (ADDRESS_3 != buf[DEST_ADDRESS+3]))\
      //\
      // no, return\
      //\
      return;\
   //\
   // yes, switch on port\
   //\
   switch(buf[DEST_PORT]*256+buf[DEST_PORT+1]) \{\
      case WRITE_PORT:\
         //\
         // receive the LED state\
         //\
         if ((buf[PAYLOAD] == 'o') && (buf[PAYLOAD+1] == 'n'))\
            clear(led_port, led_pin);\
         else\
            set(led_port, led_pin);\
         return;\
      case READ_PORT:\
         //\
         // send the LED state\
         //\
         if (0 == pin_test(led_port, led_pin)) \{\
            payload[0] = 'o';\
            payload[1] = 'n';\
            payload_length = 2;\
            \}\
         else \{\
            payload[0] = 'o';\
            payload[1] = 'f';\
            payload[2] = 'f';\
            payload_length = 3;\
            \}\
         packet_length = assemble_packet(buf,\
            buf[DEST_ADDRESS],\
            buf[DEST_ADDRESS+1],\
            buf[DEST_ADDRESS+2],\
            buf[DEST_ADDRESS+3],\
            buf[SOURCE_ADDRESS],\
            buf[SOURCE_ADDRESS+1],\
            buf[SOURCE_ADDRESS+2],\
            buf[SOURCE_ADDRESS+3],\
            0,0,\
            buf[DEST_PORT],buf[DEST_PORT+1],\
            payload,payload_length);\
         put_packet(buf,packet_length);\
         return;\
      default:\
         //\
         // do nothing\
         //\
         return;\
      \}\
   \}\
\
int main(void) \{\
   //\
   // main\
   //\
   static unsigned char len,buf[MAX_PACKET];\
   //\
   // set clock divider to /1\
   //\
   CLKPR = (1 << CLKPCE);\
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);\
   //\
   // initialize output pins\
   //\
   set(serial_port, serial_pin_out); // turn on pull-up\
   input(serial_direction, serial_pin_out);\
   set(led_port, led_pin);\
   output(led_direction, led_pin);\
   //\
   // main loop\
   //\
   while (1) \{\
      //\
      // read packet\
      //\
      len = get_packet(buf);\
      //\
      // process packet\
      //\
      if (len > 0) process_packet(buf);\
      \}\
   \}}