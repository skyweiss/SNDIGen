#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define USAGE_STRING "Usage: SNDIGen outfile [options] infiles...\n"

unsigned int reverse_endian_32(unsigned int number) {
	return ((number & 0xFF) << 24) | ((number & 0xFF00) << 8) | ((number & 0xFF0000) >> 8) | (number >> 24);
}

unsigned short reverse_endian_16(unsigned short number) {
	return (number << 8) | (number >> 8);
}

void write_flipped_int(int n, FILE* file) {
	n = reverse_endian_32(n);
	fwrite(&n, sizeof(int), 1, file);
}

void write_int(int n, FILE* file) {
	fwrite(&n, sizeof(int), 1, file);
}

void write_flipped_short(short n, FILE* file) {
	n = reverse_endian_16(n);
	fwrite(&n, sizeof(short), 1, file);
}

void write_short(short n, FILE* file) {
	fwrite(&n, sizeof(short), 1, file);
}

void write_byte(char n, FILE* file) {
	fwrite(&n, sizeof(char), 1, file);
}

unsigned int bkdr(char* str) {
	unsigned int out = 0;
	for (int i = 0; i < strlen(str); i++) {
		out = out * 131 + toupper(str[i]);
		while (out >= pow(2, 32)) {
			out -= pow(2, 32);
		}
	}
	return out;
}

char* basename(char* str) {
	char* newstr = str;

	for (int i = 0; i < strlen(str); i++) {
		char c = str[i];
		if (c == '\\' || c == '/') {
			newstr = str+i+1;
		}
	}

	for (int i = 0; i < strlen(newstr); i++) {
		if (newstr[i] == '.') {
			newstr[i] = '\0';
			break;
		}
	}

	return newstr;
}

int main(int argc, char* argv[]) {
	fprintf(stdout, "\n");

	if (argc < 3) {
		fprintf(stdout, USAGE_STRING);
		return 1;
	}

	bool fix_sample_rate = false;
	int num_fsb_files = 0;

	for (int i = 2; i < argc; i++) {
		char* c = argv[i];
		if (*c == '-') {
			if (*(++c) == 's') {
				fix_sample_rate = true;
			}
		}
		else {
			num_fsb_files++;
		}
	}

	if (num_fsb_files == 0) {
		fprintf(stdout, USAGE_STRING);
		return 1;
	}

	FILE* out = fopen(argv[1], "wb");
	if (NULL == out) {
		fprintf(stdout, "Could not open file %s for writing\n", argv[1]);
		return 1;
	}

	long* offsets = calloc(num_fsb_files, sizeof(long));
	if (NULL == offsets) {
		fprintf(stdout, "Not enough memory\n");
		return 1;
	}

	int* asset_ids = calloc(num_fsb_files, sizeof(int));
	if (NULL == asset_ids) {
		fprintf(stdout, "Not enough memory\n");
		return 1;
	}

	write_flipped_int(bkdr(basename(argv[1])), out);
	write_int(0, out);
	write_int(0, out);
	write_int(0, out);
	write_int(0, out);
	write_int(0, out);
	write_flipped_short(num_fsb_files+1, out);
	write_flipped_short(1, out);
	write_flipped_short(num_fsb_files, out);
	write_byte(num_fsb_files+1, out);
	write_byte(0, out);

	char silence[0xA0] = {
		0x46, 0x53, 0x42, 0x33, 0x01, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x73, 0x69, 0x6c, 0x65, 0x6e, 0x63,
		0x65, 0x2e, 0x77, 0x61, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x10, 0x00, 0x02, 0x44, 0xac, 0x00, 0x00,
		0xff, 0x00, 0x80, 0x00, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x24, 0x74, 0x49,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x2d, 0x00, 0x30, 0x00, 0x00, 0x00
	};
	fwrite(silence, 1, 0xA0, out);

	int n = 0;

	for (int i = 2; i < argc; i++) {
		if (*argv[i] == '-') {
			continue;
		}
		
		FILE* in = fopen(argv[i], "rb");
		if (NULL == in) {
			fprintf(stdout, "Could not open file %s for reading\n", argv[i]);
			return 1;
		}

		asset_ids[n] = bkdr(basename(argv[i]));
		fprintf(stdout, "Adding sound \"%s\" [%x]\n", basename(argv[i]), asset_ids[n]);
		
		offsets[n] = ftell(out);
		n++;

		int c = 0;
		while (fread(&c, sizeof(int), 1, in)) {
			if (fix_sample_rate && ftell(in) == 0x50) {
				float adjust = 1124.0f / 1125.0f;
				c *= adjust;
				fprintf(stdout, "  Changing sample rate to %d\n", c);
			}
			fwrite(&c, sizeof(int), 1, out);
		}

		fclose(in);
	}
	
	long footer = ftell(out) - 0x20;

	write_int(0, out);
	for (n = 0; n < num_fsb_files; n++) {
		write_flipped_int(offsets[n] - 0x20, out);
	}

	write_flipped_int(0xAAAAAAAA, out);
	write_byte(0x0, out);
	write_byte(0x0, out);
	write_byte(0x0, out);
	write_byte(0x0, out);

	for (n = 0; n < num_fsb_files; n++) {
		write_flipped_int(asset_ids[n], out);
		write_byte(0x2, out);
		write_byte(0x0, out);
		write_byte(n+1, out);
		write_byte(0, out);
	}

	fseek(out, 0x4, SEEK_SET);
	write_flipped_int(footer, out);

	fclose(out);

	return 0;
}