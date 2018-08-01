/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Evgeny Zinoviev <me@ch1p.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/io.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "pmh7tool.h"

uint8_t pmh7_register_read(uint16_t reg)
{
	outb(reg & 0xff, EC_LENOVO_PMH7_ADDR_L);
	outb((reg & 0xff00) >> 8, EC_LENOVO_PMH7_ADDR_H);
	return inb(EC_LENOVO_PMH7_DATA);
}

void pmh7_register_write(uint16_t reg, uint8_t val)
{
	outb(reg & 0xff, EC_LENOVO_PMH7_ADDR_L);
	outb((reg & 0xff00) >> 8, EC_LENOVO_PMH7_ADDR_H);
	outb(val, EC_LENOVO_PMH7_DATA);
}

void pmh7_register_set_bit(uint16_t reg, uint8_t bit)
{
	uint8_t val;

	val = pmh7_register_read(reg);
	pmh7_register_write(reg, val | (1 << bit));
}

void pmh7_register_clear_bit(uint16_t reg, uint8_t bit)
{
	uint8_t val;

	val = pmh7_register_read(reg);
	pmh7_register_write(reg, val & ~(1 << bit));
}

void print_usage(const char *name)
{
	printf("usage: %s\n", name);
	printf("\n"
		   "	-h, --help:                 print this help\n"
		   "	-d, --dump:                 print registers\n"
		   "	-w, --write <addr> <data>:  write to register\n"
		   "	-r, --read <addr>:          read from register\n"
		   "	-c, --clear-bit <addr> <bit>\n"
		   "	-s, --set-bit <addr> <bit>\n"
		   "\n"
		   "Attention! Writing to PMH7 registers is very dangerous, as you\n"
		   "           directly manipulate the power rails, enable lines,\n"
		   "           interrupt lines or something else of the device.\n"
		   "           Proceed with caution."
		   "\n");
}

enum action {HELP, DUMP, WRITE, READ, CLEAR, SET};

int main(int argc, char *argv[])
{
	enum action act = HELP;
	int opt, option_index = 0;
	long input_addr = 0, input_data = 0;

	static struct option long_options[] = {
		{"help",        0, 0, 'h'},
		{"dump",        0, 0, 'd'},
		{"write",       1, 0, 'w'},
		{"read",        1, 0, 'r'},
		{"clear-bit",   1, 0, 'c'},
		{"set-bit",     1, 0, 's'},
		{0, 0, 0, 0}
	};

	if (argv[1] == NULL) {
		print_usage(argv[0]);
		exit(0);
	}

	while ((opt = getopt_long(argc, argv, "hdw:r:c:s:",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'd':
			act = DUMP;
			break;

		case 'r':
			input_addr = strtoul(optarg, NULL, 16);
			act = READ;
			break;

		case 'w':
		case 'c':
		case 's':
			input_addr = strtoul(optarg, NULL, 16);

			if (optind < argc && *argv[optind] != '-') {
				input_data = strtoul(argv[optind], NULL, 16);
				optind++;
			} else {
				fprintf(stderr,
					"Error: -%c option requires two arguments\n",
					opt);
				exit(1);
			}

			switch (opt) {
			case 'w':
				act = WRITE;
				break;
			case 'c':
				act = CLEAR;
				break;
			case 's':
				act = SET;
				break;
			}
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Error: Extra parameter found.\n");
		print_usage(argv[0]);
		exit(1);
	}

	if (act == HELP) {
		print_usage(argv[0]);
		exit(0);
	}

	if (input_addr > 0x1ff) {
		fprintf(stderr,
			"Error: <addr> cannot be greater than 9 bits long.\n");
		exit(1);
	}

	if (act == SET || act == CLEAR) {
		if (input_data > 7) {
			fprintf(stderr,
				"Error: <bit> cannot be greater than 7.\n");
			exit(1);
		}
	} else {
		if (input_data > 0xff) {
			fprintf(stderr,
				"Error: <data> cannot be greater than 8 bits long.\n");
			exit(1);
		}
	}

	if (geteuid() != 0) {
		fprintf(stderr, "You must be root.\n");
		exit(1);
	}

	if (ioperm(EC_LENOVO_PMH7_BASE, 0x10, 1)) {
		fprintf(stderr, "ioperm: %s\n", strerror(errno));
		exit(1);
	}

	switch (act) {
	case DUMP:
		for (int i = 0; i < 0x200; i++) {
			if ((i % 0x10) == 0) {
				if (i != 0)
					printf("\n");
				printf("%04x: ", i);
			}
			printf("%02x ", pmh7_register_read(i));
		}
		printf("\n");
		break;

	case READ:
		printf("%02x\n", pmh7_register_read(input_addr));
		break;

	case WRITE:
		pmh7_register_write(input_addr, input_data);
		break;

	case CLEAR:
		pmh7_register_clear_bit(input_addr, input_data);
		break;

	case SET:
		pmh7_register_set_bit(input_addr, input_data);
		break;

	default:
		break;
	}

	return 0;
}
