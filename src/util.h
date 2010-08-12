/*
 * util.h
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

#include <libimobiledevice/lockdown.h>

// These just wrap p0sixninja's original code, just trying to clean up...
extern int plist_read_from_filename(plist_t *plist, const char *filename);
extern int buffer_read_from_filename(const char *filename, char **buffer, uint32_t *length);
extern char *lockdownd_get_string_value(lockdownd_client_t client, const char *what);

// The main purpose of these two is to provide a way to mod the behavior, plus a bit of shorthand ;)
extern void info(const char *m);
extern void error(const char *m);
extern void task(const char *m);
