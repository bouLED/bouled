#include "stdint.h"
#include "fatfs.h"
#include "SEGGER_RTT.h"
#include "sd.h"

#define CHECKREAD(res, toread, read, lab) do { \
	if (res != FR_OK || read != toread) { \
		SEGGER_RTT_printf(0, "sd: f_read of size %d failed, err %d," \
				  "read %d\n", toread, res, readbytes); \
		goto lab; \
	} \
} while(0)

void sd_init(void)
{
	FRESULT res;
	/* Option 1: mount right now */
	res = f_mount(&SDFatFS, "", 1);
	if (res != FR_OK)
		SEGGER_RTT_printf(0, "sd: f_mount failed, err %d", res);
}

void readbinaryppm(const char *name, int width, int height, struct color *buf)
{
	uint8_t header[2];
	int w, h;
	unsigned int readbytes;
	FRESULT res;

	res = f_open(&SDFile, name, FA_READ);
	if (res != FR_OK) {
		SEGGER_RTT_printf(0, "sd: f_open failed, err %d\n", res);
		return;
	}

	res = f_read(&SDFile, header, 2, &readbytes);
	CHECKREAD(res, 2, readbytes, clean);

	if (header[0] != 'P' || header[1] != '6')
		goto error;

	res = f_read(&SDFile, &w, 1, &readbytes);
	CHECKREAD(res, 1, readbytes, clean);
	res = f_read(&SDFile, &h, 1, &readbytes);
	CHECKREAD(res, 1, readbytes, clean);

	if (w != width || h != height)
		goto error;

	res = f_read(&SDFile, (uint8_t*) buf, w*h, &readbytes);
	CHECKREAD(res, w*h, readbytes, clean);
	goto clean;

error:
	SEGGER_RTT_printf(0, "sd: attempted to read malformed file\n");
clean:
	res = f_close(&SDFile);
	if (res != FR_OK)
		SEGGER_RTT_printf(0, "sd; f_close() failed, err %d\n", res);
}
