#include <libimobiledevice/lockdown.h>

extern int plist_read_from_filename(plist_t *plist, const char *filename);
extern int buffer_read_from_filename(const char *filename, char **buffer, uint32_t *length);
extern char *lockdownd_get_string_value(lockdownd_client_t client, const char *what);
