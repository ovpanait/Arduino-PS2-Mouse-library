#include "PS2.h"
#include <Arduino.h>

void MousePS2::set_input_high(uint8_t pin){
  pinMode(pin,INPUT_PULLUP);
}

void MousePS2::set_output_low(uint8_t pin)
{
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
}

void MousePS2::send_command(uint8_t data){

/* "Request-to-send" state  */
set_output_low(clk_pin);
delayMicroseconds(150);
set_output_low(data_pin);
delayMicroseconds(5);

/*  Release clock line  */
set_input_high(clk_pin);

/* PS2 protocol implies that bytes are sent only when the Clock line is low */

while(digitalRead(clk_pin)==HIGH);

/* Send the data */

send_byte(data);

/* release the Data line */

set_input_high(data_pin);

/* Wait for the acknowledge bit */

while(digitalRead(data_pin)==HIGH);
while(digitalRead(data_pin)==LOW);

/* Wait for the device to release clock line */

while(digitalRead(clk_pin)==LOW);

}

uint8_t MousePS2::get_result (){

  /* PS2 uses a serial protocol with 11 bit frames :
  
		1 start bit - always 0
		8 data bits - least significat bit first
		1 parity bit - odd parity
		1 stop bit - always 1
	
	*/
	
	/* Start bit */
	
	while(digitalRead(clk_pin)==HIGH);
	while(digitalRead(clk_pin)==LOW);
	
	/* Data bits */
	
	uint8_t value = receive_byte();
	
	/* Stop bit */
	
	while(digitalRead(clk_pin)==LOW);

	return value;
}

/* Send 8 bits data + 1 parity bit */

void MousePS2::send_byte(uint8_t data){

uint8_t parity=1;

while(digitalRead(clk_pin)==HIGH);

/* Send data bits */

for(uint8_t i=0;i<8;i++){
  if(data&0x01)
    set_input_high(data_pin);
  else 
    set_output_low(data_pin);
    
  while(digitalRead(clk_pin)==LOW);
  while(digitalRead(clk_pin)==HIGH);
    
  parity^=(data&0x01);
  data>>=1;
}

/* Send parity bit */

if(parity)
  set_input_high(data_pin);
else 
	set_output_low(data_pin);

while(digitalRead(clk_pin)==LOW);
while(digitalRead(clk_pin)==HIGH);
  
}

/* Receive 8 data + 1 parity */
uint8_t MousePS2::receive_byte(){
	
	uint8_t received=0x00;
	uint8_t mask =0x01;
  
	while(digitalRead(clk_pin)==HIGH);
	
	for(uint8_t i=0;i<8;i++)
    {
		if(digitalRead(data_pin)==HIGH)
			received^=mask;

		while(digitalRead(clk_pin)==LOW);
		while(digitalRead(clk_pin)==HIGH);
		mask<<=1;
		
    }
	
/* Receive parity bit */

 uint8_t parity = digitalRead(data_pin);
 while(digitalRead(clk_pin)==LOW);
 while(digitalRead(clk_pin)==HIGH);
 
 return received;
}

void MousePS2::init(uint8_t mode){
		
	set_input_high(clk_pin);
	set_input_high(data_pin);
	
	/* Eneter reset mode */
	
	send_command(RESET);
	get_result(); //ACKNOWLEDGE RESET MODE
	get_result(); //BAT SUCCESSFUL --- AA ----
	get_result();// IDDev -0x00
	
	/* Enter a specific mode of operation*/
	
	send_command(mode);
	get_result();
}

/*

3 bytes are sent each time:
byte 1: flag byte
byte 2: x movement
byte 3: y movement

*/

/* Mouse will send data on request */

void MousePS2::remote_mode_request(){
	send_command(READ_DATA);
	get_result(); // Acknowledge request
	Serial.println((int8_t)get_result()); 
	Serial.println((int8_t)get_result());
	Serial.println((int8_t)get_result());
}

/* Mouse will send data each time it registers movement */

void MousePS2::stream_mode_receive(){
	while(digitalRead(clk_pin)==HIGH);
	get_result();
}
