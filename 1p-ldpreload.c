#include <string.h>
#include <dlfcn.h>
#include <stdint.h>
#include <sys/inotify.h>

//#include <stdio.h>

#define ONEPASSWORD_BINARY_SUFFIX "/1password"
static int (*original_inotify_add_watch)(int fd, const char *pathname, uint32_t mask) = NULL;

int suffix_cmp(const char *str, const char *suffix)
{
    if (str == NULL || suffix == NULL) {
        return 0;
    }

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len >  str_len) {
        return 0;
    }

    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

int inotify_add_watch(int fd, const char *pathname, uint32_t mask)
{
    if (original_inotify_add_watch == NULL) {
        original_inotify_add_watch = dlsym(RTLD_NEXT, "inotify_add_watch");
    }

    if (suffix_cmp(pathname, ONEPASSWORD_BINARY_SUFFIX)) {
        //FILE *LOG_OUTPUT;
        //LOG_OUTPUT = fopen("/tmp/1password-inotify.log", "a");
        //fprintf(LOG_OUTPUT, "pathname: %s, mask: 0x%x\n", pathname, mask);
        //fclose(LOG_OUTPUT);

        return (*original_inotify_add_watch)(fd, pathname, mask & ~(IN_ATTRIB));
    }

    return (*original_inotify_add_watch)(fd, pathname, mask);
}
