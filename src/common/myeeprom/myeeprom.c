#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "myeeprom.h"

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
#if 0
	if(size==2){
		printf("%x ",command);
		printf("%04x\n",data->byte);
	}
	if(size==3){
		printf("%x ",command);
		printf("%04x\n",data->word);
	}
#endif
	return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_read_byte(int file)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data)) {
		return -1;
	} else {
		return 0x0FF & data.byte;
	}
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &data);
}

static int i2c_write_2b(struct eeprom *e, __u8 buf[2])
{
	int r;
	r = i2c_smbus_write_byte_data(e->fd, buf[0], buf[1]);
	if(r < 0) {
		fprintf(stderr, "Error i2c_write_2b: %s\n", strerror(errno));
	}
	usleep(10);
	return r;
}

static int i2c_write_3b(struct eeprom *e, __u8 buf[3])
{
	int r;
	r = i2c_smbus_write_word_data(e->fd, buf[0], buf[2] << 8 | buf[1]);
	if(r < 0) {
		fprintf(stderr, "Error i2c_write_3b: %s\n", strerror(errno));
	}
	usleep(10);
	return r;
}


#define CHECK_I2C_FUNC( var, label ) do { if(0 == (var & label)) { fprintf(stderr, "\nError: " #label " function is required. Program halted.\n\n"); exit(1); } } while(0);

int eeprom_open(char *dev_fqn, int addr, int type, struct eeprom* e)
{
	int funcs, fd, r;
	e->fd = e->addr = 0;
	e->dev = 0;
	
	fd = open(dev_fqn, O_RDWR);
	if(fd <= 0) {
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}

	if((r = ioctl(fd, I2C_FUNCS, &funcs) < 0)) {
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}

	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

	if( ( r = ioctl(fd, I2C_SLAVE, addr)) < 0) {
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}
	e->fd = fd;
	e->addr = addr;
	e->dev = dev_fqn;
	e->type = type;
	return 0;
}

int eeprom_close(struct eeprom *e)
{
	close(e->fd);
	e->fd = -1;
	e->dev = 0;
	e->type = EEPROM_TYPE_UNKNOWN;
	return 0;
}

int eeprom_read_byte(struct eeprom* e, __u16 mem_addr)
{
	int r;
	ioctl(e->fd, BLKFLSBUF);
	if(e->type == EEPROM_TYPE_16BIT) {
		__u8 buf[2] = { (mem_addr >> 8) & 0x0ff, mem_addr & 0x0ff };
		r = i2c_write_2b(e, buf);
	} else {
		fprintf(stderr, "ERR: unknown eeprom type\n");
		return -1;
	}
	if (r < 0) {
		return r;
	}
	r = i2c_smbus_read_byte(e->fd);
	return r;
}

int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data)
{
	if(e->type == EEPROM_TYPE_16BIT) {
		__u8 buf[3] = { (mem_addr >> 8) & 0x00ff, mem_addr & 0x00ff, data };
		return i2c_write_3b(e, buf);
	} 
	fprintf(stderr, "ERR: unknown eeprom type\n");
	return -1;
}

#define die_if(a, msg) do { do_die_if( a , msg, __LINE__); } while(0);
void do_die_if(int b, char* msg, int line)
{
	if(!b) {
		return;
	}
	fprintf(stderr, "Error at line %d: %s\n", line, msg);
	fprintf(stderr, "	sysmsg: %s\n", strerror(errno));
	exit(1);
}

int myeeprom_read(int addr, char *buf, int size)
{
	int i,j;

	struct eeprom e;

//	die_if(eeprom_open("/dev/i2c/0", 0x50, EEPROM_TYPE_16BIT, &e) < 0, "unable to open eeprom device file (check that the file exists and that it's readable)");

	if (eeprom_open("/dev/i2c/0", 0x50, EEPROM_TYPE_16BIT, &e) < 0) {
		printf("unable to open eeprom device file (check that the file exists and that it's readable\n");
		return -1;
	}

	bzero(buf, size);

	for(i = 0; i < size; i++, addr++, buf++) {
		for (j = 0;j < 3;) {
			usleep(1);
			if ((*buf = eeprom_read_byte(&e, addr)) < 0) {
				j++;
			//	printf("read erro\n");
			//	return -1;
			}

			if ( j >= 3) {
				printf("read erro\n");
				return -1;
			}
			else {
				break;
			}
		}
	//	die_if((*buf = eeprom_read_byte(&e, addr)) < 0, "read error");
	}

	eeprom_close(&e);
	return 1;
}

int myeeprom_write(int addr, char *buf, int size)
{
	int i, j;

	struct eeprom e;

//	die_if(eeprom_open("/dev/i2c/0", 0x50, EEPROM_TYPE_16BIT, &e) < 0, "unable to open eeprom device file (check that the file exists and that it's readable)");

	if (eeprom_open("/dev/i2c/0", 0x50, EEPROM_TYPE_16BIT, &e) < 0) {
		printf("unable to open eeprom device file (check that the file exists and that it's readable\n");
		return -1;
	}

	for(i = 0; i < size; i++, addr++, buf++) {
		for (j = 0; j < 3; j++) {
			if (eeprom_write_byte(&e, addr, *buf) < 0) {
				printf("write erro\n");
				return -1;
			}
			usleep(4000);

            if (*buf == eeprom_read_byte(&e, addr)) {
                j = 0;
                break;
            }
        }

        if (j >= 3) {
            j = 0;
        }
	//	die_if(eeprom_write_byte(&e, addr, *buf), "write error");
	}

	eeprom_close(&e);
	return 1;
}
#if 0
int eeprom_read_code(char *buf)
{
	eeprom_read(64, buf, 64);
	return;
}

int eeprom_write_code(char *buf)
{
	eeprom_write(64, buf, 64);
	return;
}

int eeprom_read_time(time_t *buf, int addr)
{
	char time[4];
	eeprom_read(addr, time, 4);
	memcpy(buf, time, 4);
	return 0 ;
}

int eeprom_write_time(time_t buf, int *addr)
{
	char time[4];
	memcpy(time, &buf, 4);
	*addr = (*addr >= 0x1FFC) ? 128 : (*addr+4);
	eeprom_write(*addr, time, 4);
	return 0;
}
#endif 
