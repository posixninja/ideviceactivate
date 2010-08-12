/*
 * cache.c
 * Load data from/add data to the activation info cache.
 *
 * Copyright (c) 2010 boxingsquirrel. All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <plist/plist.h>
#include <libimobiledevice/lockdown.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "util.h"

int write_file(const char *filename, char data[BUFSIZE])
{
	FILE *f=fopen(filename, "w");

	if (f==NULL)
	{
		printf("ERROR: Could not open %s for writing\n", filename);
		fclose(f);
		return -1;
	}

	else {
		fwrite(data, strlen(data), 1, f);
		fclose(f);

		return 0;
	}
}

int read_file(const char *filename, char data[BUFSIZE])
{
	FILE *f=fopen(filename, "r");

	if (f==NULL)
	{
		printf("ERROR: Could not open %s for reading\n", filename);
		fclose(f);
		return -1;
	}

	else {
		uint32_t read=fread(data, 1, BUFSIZE, f);

		while (read>0)
		{
			read=fread(data, 1, BUFSIZE, f);
			printf(".");
		}

		printf("\nINFO: Read sucessfully from %s\n", filename);

		return 0;
	}
}

int cache(const char *fname, const char *what)
{
	if (backup_to_cache==1)
	{
		char data[BUFSIZE];
		snprintf(data, BUFSIZE, "%s", what);

		char f_name[512];
		snprintf(f_name, BUFSIZE, "%s/%s", cachedir, fname);

		if (write_file((const char *)f_name, data)!=0)
		{
			return -1;
		}

		return 0;
	}

	else {
		return -1;
	}
}

int cache_plist(const char *fname, plist_t plist)
{
	if (backup_to_cache==1)
	{
		uint32_t len=0;
		char **xml=NULL;

		plist_to_xml(plist, &xml, &len);

		return cache(fname, (const char *)xml);
	}

	else {
		return -1;
	}
}

char* get_from_cache(const char *what)
{
	char fname[512];
	snprintf(fname, 512, "%s/%s", cachedir, what);

	char *d[BUFSIZE];
	read_file((const char *)fname, &d);

	return (char *)d;
}

/* Just prints a little notice about what caching actually does... */
void cache_warning()
{
	printf("The process of creating a cache is simple: perform a legit activation, storing all the required data. That way, you can borrow (or, I guess, steal (don't do that, though)) a sim for the carrier your iPhone is locked to, and be able to reactivate without having to get that sim back.\n\nThis data is stored in a folder where you want it (hence the folder passed with -c/-r). It does not get sent to me (boxingsquirrel), p0sixninja, or anyone else. Plus, we really have better things to do than look at your activation data.\n\nThis really isn't needed for iPod Touches or Wi-Fi only iPads (and I don't know if 3G iPad users need this, but be safe and do it).\n\nPress any key to continue or CONTROL-C to abort...\n\n");

	getchar();
}

/* Validates the cache to make sure it really is the cache for the connected device... */
int check_cache(lockdownd_client_t c)
{
	char* uuid_from_cache=get_from_cache("UUID");
	char* uuid_from_device=(char *)lockdownd_get_string_value(c, "UniqueDeviceID");

	return strcmp(uuid_from_cache, uuid_from_device);
}
