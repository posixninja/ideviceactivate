#include <plist/plist.h>

#define BUFSIZE 0x200000

extern char* cachedir;
extern int use_cache;
extern int backup_to_cache;

extern int write_file(const char *filename, char data[BUFSIZE]);
extern int cache(const char *fname, const char *what);
extern int cache_plist(const char *fname, plist_t plist);
extern char* get_from_cache(const char *what);

extern void cache_warning();

extern int check_cache(lockdownd_client_t c);
