/*
	simduino.c

	Copyright 2008, 2009 Michel Pollet <buserror@gmail.com>

 	This file is part of simavr.

	simavr is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	simavr is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <pthread.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "uart_pty.h"
#include "sim_vcd_file.h"

uart_pty_t uart_pty;
avr_t * avr = NULL;
avr_vcd_t vcd_file;

struct avr_flash {
	char avr_flash_path[1024];
	int avr_flash_fd;
};

// avr special flash initalization
// here: open and map a file to enable a persistent storage for the flash memory
void avr_special_init( avr_t * avr, void * data)
{
	struct avr_flash *flash_data = (struct avr_flash *)data;

	printf("%s\n", __func__);
	// open the file
	flash_data->avr_flash_fd = open(flash_data->avr_flash_path,
									O_RDWR|O_CREAT, 0644);
	if (flash_data->avr_flash_fd < 0) {
		perror(flash_data->avr_flash_path);
		exit(1);
	}
	// resize and map the file the file
	(void)ftruncate(flash_data->avr_flash_fd, avr->flashend + 1);
	ssize_t r = read(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
	if (r != avr->flashend + 1) {
		fprintf(stderr, "unable to load flash memory\n");
		perror(flash_data->avr_flash_path);
		exit(1);
	}
}

// avr special flash deinitalization
// here: cleanup the persistent storage
void avr_special_deinit( avr_t* avr, void * data)
{
	struct avr_flash *flash_data = (struct avr_flash *)data;

	printf("%s\n", __func__);
	lseek(flash_data->avr_flash_fd, SEEK_SET, 0);
	ssize_t r = write(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
	if (r != avr->flashend + 1) {
		fprintf(stderr, "unable to load flash memory\n");
		perror(flash_data->avr_flash_path);
	}
	close(flash_data->avr_flash_fd);
	uart_pty_stop(&uart_pty);
}

int simTxfd = 0;
int simRxfd = 0;
static void rlp_sim_fifo_init( uint8_t nodeId ) {
	char rxPath[80];
	char txPath[80];

	sprintf ( &rxPath[0], "/home/rpipkin/aprs/virtspoon/io/node%d-rx", nodeId );
	sprintf ( &txPath[0], "/home/rpipkin/aprs/virtspoon/io/node%d-tx", nodeId );

	struct stat tempbuffer;
	if ( stat( txPath, &tempbuffer ) != 0 ) {
		mknod( txPath, S_IFIFO | 0666, 0 );
	}

	if ( stat( rxPath, &tempbuffer ) != 0 ) {
		mknod( rxPath, S_IFIFO | 0666, 0 );
	}

	printf ( "opening %s\n",txPath );
	printf ( "opening %s\n",rxPath );
	simTxfd = open( txPath, O_RDWR | O_NONBLOCK );
	simRxfd = open( rxPath, O_RDWR | O_NONBLOCK );
}

static void rlp_sim_fifo_wr(avr_t * avr, avr_io_addr_t addr, uint8_t v, void * param)
{
	printf ( "Simulated fifo received 0x%02x : %c | 0x%02x : %c\n", v, v, v>>1,v>>1 );
	write ( simTxfd, &v, 1 );
}


int mybuffer = 0;
static const uint8_t msg1[] =
{
		0x7e, 0x82, 0xa0, 0xb4, 0x9a, 0x88, 0x9a, 0x60, 0x9c, 0x9e, 0x86, 0x82, 0x98,
		0x98, 0x60, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0x62, 0xae, 0x92, 0x88, 0x8a,
		0x64, 0x40, 0x65, 0x03, 0xf0, 0x74, 0x65, 0x73, 0x74, 0x69, 0xc0, 0xf6, 0x7e
};

static uint8_t rlp_sim_fifo_rd(avr_t * avr, avr_io_addr_t addr, void * param)
{

	uint8_t c = 0;
	if ( mybuffer < sizeof(msg1) ) {
		c = msg1[mybuffer++];
		printf ( "Simulated fifo returned 0x%02x\n", c );
	}

	return ( c );
}


int main(int argc, char *argv[])
{
	struct avr_flash flash_data;
	char boot_path[1024] = "ATmegaBOOT_168_atmega328.ihex";
	uint32_t boot_base, boot_size;
	char * mmcu = "atmega328p";
	uint32_t freq = 16000000;
	int debug = 0;
	int verbose = 0;
	int node = 1;


#if 0
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i] + strlen(argv[i]) - 4, ".hex"))
			strncpy(boot_path, argv[i], sizeof(boot_path));
		else if (!strcmp(argv[i], "-d"))
			debug++;
		else if (!strcmp(argv[i], "-v"))
			verbose++;
		else {
			fprintf(stderr, "%s: invalid argument %s\n", argv[0], argv[i]);
			exit(1);
		}
	}
#else
	int c;
	while ( (c = getopt( argc, argv, "dvn:f:")) != -1) {
		switch (c) {
		case 'f':
			printf ( "args: %s\n", optarg );
			if (!strcmp(optarg + strlen(optarg) - 4, ".hex"))
				strncpy(boot_path, optarg, sizeof(boot_path));
			break;
		case 'd':
			debug++;
			break;
		case 'v':
			verbose++;
			break;
		case 'n':
			node = atoi(optarg);
			break;
		case '?':
		default:
			fprintf ( stderr, "%s: invalid argument %c\n", argv[0], (char) c );
			exit(1);
		}
	}
#endif

	avr = avr_make_mcu_by_name(mmcu);
	if (!avr) {
		fprintf(stderr, "%s: Error creating the AVR core\n", argv[0]);
		exit(1);
	}

	uint8_t * boot = read_ihex_file(boot_path, &boot_size, &boot_base);
	if (!boot) {
		fprintf(stderr, "%s: Unable to load %s\n", argv[0], boot_path);
		exit(1);
	}
	if (boot_base > 32*1024*1024) {
		mmcu = "atmega2560";
		freq = 20000000;
	}
	printf("%s booloader 0x%05x: %d bytes\n", mmcu, boot_base, boot_size);

	snprintf(flash_data.avr_flash_path, sizeof(flash_data.avr_flash_path),
			"simduino_%s_flash.bin", mmcu);
	flash_data.avr_flash_fd = 0;
	// register our own functions
	avr->custom.init = avr_special_init;
	avr->custom.deinit = avr_special_deinit;
	avr->custom.data = &flash_data;
	avr_init(avr);
	avr->frequency = freq;

	memcpy(avr->flash + boot_base, boot, boot_size);
	free(boot);
	avr->pc = boot_base;
	/* end of flash, remember we are writing /code/ */
	avr->codeend = avr->flashend;
	avr->log = 1 + verbose;


	//... handle simulator
	rlp_sim_fifo_init( node );
	avr_register_io_write(avr, 0xff, rlp_sim_fifo_wr, NULL);
	avr_register_io_read(avr, 0xfe, rlp_sim_fifo_rd, NULL);


	// even if not setup at startup, activate gdb if crashing
	avr->gdb_port = 1234;
	if (debug) {
		avr->state = cpu_Stopped;
		avr_gdb_init(avr);
	}

	uart_pty_init(avr, &uart_pty);
	uart_pty_connect(&uart_pty, '0');

	while (1) {
		int state = avr_run(avr);
		if ( state == cpu_Done || state == cpu_Crashed)
			break;
	}

}
