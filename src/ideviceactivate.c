/*
 * ideviceactivate.c
 * A simple utility to handle the activation process for iPhones
 *
 * Copyright (c) 2010 Joshua Hill. All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <plist/plist.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/libimobiledevice.h>

#include "activate.h"

static int buffer_read_from_filename(const char *filename, char **buffer, uint32_t *length) {
	FILE *f;
	uint64_t size;

	f = fopen(filename, "rb");
	if(f == NULL) {
		printf("Unable to open file %s\n", filename);
		return -1;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);

	*buffer = (char*) malloc(sizeof(char) * size);
	if (fread(*buffer, sizeof(char), size, f) != size) {
		printf("Unable to read %llu bytes from '%s'.\n", size, filename);
		free(*buffer);
		*buffer = NULL;
	}
	fclose(f);

	*length = size;
	return 0;
}

static int plist_read_from_filename(plist_t *plist, const char *filename) {
	char *buffer = NULL;
	uint32_t length;

	if (filename == NULL) {
		printf("No filename specified\n");
		return -1;
	}

	if(buffer_read_from_filename(filename, &buffer, &length) < 0) {
		printf("Unable to read file\n");
		return -1;
	}

	if (buffer ==  NULL) {
		printf("Buffer returned null\n");
		return -1;
	}

	if (memcmp(buffer, "bplist00", 8) == 0) {
		plist_from_bin(buffer, length, plist);
	} else {
		plist_from_xml(buffer, length, plist);
	}

	free(buffer);

	return 0;
}

static void usage(int argc, char** argv) {
	char* name = strrchr(argv[0], '/');
	printf("Usage: %s [OPTIONS]\n", (name ? name + 1 : argv[0]));
	printf("Activate or Deactivate an iPhone device .\n\n");
	printf("options:\n");
	printf("  -x\t\tdeactivate the target device\n");
	printf("  -d\t\tenable communication debugging\n");
	printf("  -h\t\tprints usage information\n");
	printf("  -u UUID\ttarget specific device by its 40-digit device UUID\n");
	printf("  -f FILE\tactivates device with local activation record\n");
	printf("\n");
	printf("\n");
}

int main(int argc, char* argv[]) {
	int i = 0;
	int opt = 0;
	int debug = 0;
	char* uuid = NULL;
	char* file = NULL;
	int deactivate = 0;
	idevice_t device = NULL;
	lockdownd_client_t client = NULL;
	idevice_error_t device_error = IDEVICE_E_UNKNOWN_ERROR;
	lockdownd_error_t client_error = LOCKDOWN_E_UNKNOWN_ERROR;

	while ((opt = getopt(argc, argv, "dhxu:f:")) > 0) {
		switch (opt) {
		case 'h':
			usage(argc, argv);
			break;

		case 'd':
			debug = 1;
			break;

		case 'x':
			deactivate = 1;
			break;

		case 'u':
			uuid = optarg;
			break;

		case 'f':
			file = optarg;
			break;

		default:
			usage(argc, argv);
			return -1;
		}
	}

	argc -= optind;
	argv += optind;

	device_error = idevice_new(&device, uuid);
	if (device_error != IDEVICE_E_SUCCESS) {
		printf("No device found, is it plugged in?\n");
		return -1;
	}

	client_error = lockdownd_client_new_with_handshake(device, &client, "ideviceactivate");
	if (client_error != LOCKDOWN_E_SUCCESS) {
		printf("Unable to connect to lockdownd\n");
		idevice_free(device);
		return -1;
	}

	plist_t activation_record = NULL;
	if (deactivate) {
		printf("Deactivating device... ");
		client_error = lockdownd_deactivate(client);
		if (client_error == LOCKDOWN_E_SUCCESS) {
			printf("SUCCESS\n");
		} else {
			fprintf(stderr, "ERROR\nUnable to deactivate device: %d\n", client_error);
		}

	} else {
		if (file != NULL) {
			printf("Reading activation record from %s\n", file);
			if (plist_read_from_filename(&activation_record, file) < 0) {
				printf("Unable to find activation record\n");
				lockdownd_client_free(client);
				idevice_free(device);
				return -1;
			}

		} else {
			printf("Creating activation request\n");
			if(activate_fetch_record(client, &activation_record) < 0) {
				fprintf(stderr, "Unable to fetch activation request\n");
				lockdownd_client_free(client);
				idevice_free(device);
				return -1;
			}
		}

		printf("Activating device... ");
		client_error = lockdownd_activate(client, activation_record);
		if (client_error == LOCKDOWN_E_SUCCESS) {
			printf("SUCCESS\n");
		} else {
			fprintf(stderr, "ERROR\nUnable to activate device: %d\n", client_error);
		}

		//plist_free(activation_record);
		activation_record = NULL;
	}

	lockdownd_client_free(client);
	idevice_free(device);
	return 0;
}

