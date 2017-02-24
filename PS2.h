/*
************************************

Library for interfacing with PS2 Mouse
Orange - GROUND
White - CLOCK
Green - DATA
Blue - VCC

************************************
*/

#ifndef PS2_h
#define PS2_h

#define RESET 0xFF
#define READ_DATA 0xEB
#define BAT_SUCCESSFUL 0xAA
#define ERROR 0xFC
#define SET_SAMPLE_RATE 0xF3
#define ENABLE_DATA_REPORTING 0xF4
#define SET_STREAM_MODE 0xEA
#define REMOTE_MODE 0xF0

#include <Arduino.h>

class MousePS2{
	
	uint8_t clk_pin;
	uint8_t data_pin;
	
public:
	
	MousePS2(uint8_t clk_pin, uint8_t data_pin){
		
		this->clk_pin = clk_pin;
		this->data_pin = data_pin;
		
	}
	
	/* Manipulate pins */
	
	void set_input_high(uint8_t);
	void set_output_low(uint8_t);
	
	/* Send & Receive bytes to/from the PS2 device */
	
	void send_byte(const uint8_t);
	uint8_t receive_byte();
	
	/* Initialize and communicate with the device */
	
	void init(uint8_t);
	uint8_t get_result();
	void send_command(const uint8_t);
	
	/* Modes of operation */
	
	void remote_mode_request();
	void stream_mode_receive();
	
};

#endif
