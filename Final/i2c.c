#include "i2c.h"

void i2c_init() {
	TWSR = (0 << TWPS0) | (0 << TWPS1); // NOT USEFUL SET PRESCALER TO 1
	// SCL = F_CPU/(16 + 2 * PRESCALER * X) || SCL must be equal to the frequency of module with lowest operating frequency
	TWBR = 0x48;
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