#ifndef _MYEEPROM_H_
#define _MYEEPROM_H_


#include <linux/i2c-dev.h>
#include <linux/i2c.h>


#define EEPROM_TYPE_UNKNOWN 0
#define EEPROM_TYPE_16BIT 1

#ifdef __cplusplus
extern "C"{
#endif
struct eeprom
{
	char *dev;	// device
	int addr;	// address
	int fd;		// file descriptor
	int type; 	// type
};

// API
int myeeprom_write(int addr, char *buf, int size);
int myeeprom_read(int addr, char *buf, int size);

#ifdef __cplusplus
}
#endif
#endif /* _MYEEPROM_H_ */

