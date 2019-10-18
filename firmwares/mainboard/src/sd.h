#ifndef SD_H
#define SD_H

struct color {
	uint8_t r, g, b;
};

void sd_init(void);
void readbinaryppm(const char* name, int width, int height, struct color* buf);

#endif
