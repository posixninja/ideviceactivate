/*
 * util.c
 * Just utility functions ;)
 *
 * Copyright (c) 2010 Joshua Hill and boxingsquirrel. All Rights Reserved.
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
#include <stdint.h>
#include <string.h>
#include <plist/plist.h>
#include <libimobiledevice/lockdown.h>

#include "util.h"

int buffer_read_from_filename(const char *filename, char **buffer, uint32_t *length) {
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

int plist_read_from_filename(plist_t *plist, const char *filename) {
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

/* This one is a wrapper for p0sixninja's rather repetetive code... */
void lockdownd_get_string_value(lockdownd_client_t client, const char *what, char *val)
{
	plist_t val_node=NULL;

	lockdownd_get_value(client, NULL, what, &val_node);
	if (!val_node || plist_get_node_type(val_node) != PLIST_STRING) {
		fprintf(stderr, "Unable to get %s from lockdownd\n", what);
		val=NULL;
	}
	plist_get_string_val(val_node, &val);
	plist_free(val_node);
}

/* This is really just a function to allow some hooking into Gtk stuff in iDeviceActivator... */
void info(const char *m)
{
	printf("INFO: %s\n", m);
}

void error(const char *m)
{
	fprintf(stderr, "%s\n", m);
}

void task(const char *m)
{
	printf("%s\n", m);	
}
