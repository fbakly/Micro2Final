/*
 * Final.c
 *
 * Created: 19/03/2020 17:14:19
 * Author : Fouad Elbakly
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define MYUBRR (F_CPU/16/BAUD_RATE - 1)
#define BH1750_ID 0x40
#define BH1750_ADDRESS 0x06
#define MCP23008_ID 0x40
#define MCP23008_ADDRESS 0x00
#define MCP23008_IODIR 0x00
#define MCP23008_OLAT 0x0A
#define MAX_TRIES 50
#define I2C_START 0
#define I2C_DATA 1
#define I2C_DATA_ACK 2
#define I2C_STOP 3
#define I2C_DATA_NACK 4
#define ACK 1
#define NACK 0
#define READ 1
#define WRITE 0

#include <util/delay.h>
#include <avr/io.h>
#include <compat/twi.h>
#include <stdio.h>

void USART_init() {
	UBRR0H = (MYUBRR >> 8);
	UBRR0L = (MYUBRR);
	UCSR0B |= (1 << TXEN0);
	UCSR0C |= (1 << USBS0) | (3 << UCSZ00);
}

void USART_transmit(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0))) {}
	UDR0 = data;
}


uint8_t i2c_transmit(uint8_t type) {
	switch(type) {
		case I2C_START:
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;
		case I2C_DATA:
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_DATA_ACK:
			TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_DATA_NACK:
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case  I2C_STOP:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			return 0;
		default:
			return -1;
	}
	while (!(TWCR & (1 << TWINT)));
	return (TWSR & 0xF8);
}

int8_t i2c_start(uint8_t device_id, uint8_t device_address, uint8_t rw) {
	uint8_t i2c_status;
	uint8_t num_retry = -1;
	do {
		i2c_transmit(I2C_START);
		TWDR = (device_id & 0xF0) | (device_address & 0x0E) | rw;
		i2c_status = i2c_transmit(I2C_DATA);
		if (num_retry++ >= MAX_TRIES) {
			return -1;
		}
		//USART_transmit(i2c_status);
	} while (i2c_status == TW_MT_ARB_LOST);
	return 0;
}

void bh1750_reset() {
	i2c_start(BH1750_ID, BH1750_ADDRESS, WRITE);
	TWDR = 0x07;
	i2c_transmit(I2C_DATA);
	i2c_transmit(I2C_STOP);
}

void bh1750_init() {
	i2c_start(BH1750_ID, BH1750_ADDRESS, WRITE);
	bh1750_reset();
}

void bh1750_sleep() {
	i2c_start(BH1750_ID, BH1750_ADDRESS, WRITE);
	TWDR = 0x00;
	i2c_transmit(I2C_DATA);
	i2c_transmit(I2C_STOP);
}

void i2c_init() {
	TWSR = (0 << TWPS0) | (0 << TWPS1); // NOT USEFUL SET PRESCALER TO 1
	// SCL = F_CPU/(16 + 2 * PRESCALER * X) || SCL must be equal to the frequency of module with lowest operating frequency
	TWBR = 0x48;
}


void init() {
	USART_init();
	i2c_init();
	bh1750_init();
}

void mcp_iodir(uint8_t direction) {
	i2c_start(MCP23008_ID, MCP23008_ADDRESS, WRITE);
	TWDR = 0x00;
	i2c_transmit(I2C_DATA);
	TWDR = WRITE;
	i2c_transmit(I2C_DATA);
	i2c_transmit(I2C_STOP);
}

void mcp_write(uint8_t data) {
	i2c_start(MCP23008_ID, MCP23008_ADDRESS, WRITE);
	TWDR = MCP23008_OLAT;
	i2c_transmit(I2C_DATA);
	TWDR = data;
	i2c_transmit(I2C_DATA);
	i2c_transmit(I2C_STOP);
}

uint16_t read_lux() {
	uint16_t lux_level;
	
	// reset the data register
	bh1750_reset();
	
	i2c_start(BH1750_ID, BH1750_ADDRESS, WRITE);

	TWDR = 0x20;
	i2c_transmit(I2C_DATA);
	i2c_transmit(I2C_STOP);

	_delay_ms(180);

	i2c_start(BH1750_ID, BH1750_ADDRESS, READ);
	i2c_transmit(I2C_DATA_ACK);
	lux_level = (TWDR << 8);
	i2c_transmit(I2C_DATA_NACK);
	lux_level |= TWDR;
	i2c_transmit(I2C_STOP);
	return (lux_level/1.2);
}

void display_brightness(uint16_t lux) {
	if (lux >= 10000) {
		mcp_write(0x00);
	} else if (lux >= 1000) {
		mcp_write(0x01);
	} else if (lux >= 200) {
		mcp_write(0x03);
	} else if (lux >= 200) {
		mcp_write(0x07);
	} else if (lux >= 100) {
		mcp_write(0x0F);
	} else if (lux >= 50) {
		mcp_write(0x1F);
	} else if (lux >= 25) {
		mcp_write(0x3F);
	} else if (lux >= 12) {
		mcp_write(0x7F);
	} else {
		mcp_write(0xFF);
	}
}

int main(void) {
    init();
	uint16_t lux;
	mcp_iodir(WRITE);
	while (1) {
		bh1750_reset();
		lux = read_lux();
		USART_transmit(lux >> 8);
		USART_transmit(lux & 0x00FF);
		display_brightness(lux);
		_delay_ms(100);
	}
}