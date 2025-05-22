#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <errno.h>

#define VMCTL_INIT_VERBOSITY 0
#if VMCTL_INIT_VERBOSITY
#include <stdio.h>
#include <stdarg.h>

char const * const indent_str = "    ";
static void info(unsigned indent, char const *fmt, ...) {
    while(0 != indent--)
        fprintf(stdout, "%s", indent_str);

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fprintf(stdout, "\n");
}
static void prnt_args(int argc, char *argv[]) {
    info(1, "Arguments :\n");
    info(2, "argc = %d\n", argc);
    info(2, "argv:\n");
    for (int i = 0; i < argc; i++)
        info(3, "argv[%d] = %s\n", i, argv[i]);
}

#else
static inline void info(unsigned indent, char const *fmt, ...) {}
static inline void prnt_args(int argc, char *argv[]) {}
#endif

int ensure_dir(const char *path) {
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        info(1, "Error: creating the path (%s)", path);
        return -1;
    }
    return 0;
}

int mount_nix_store() {
    int ret;
    // This must match the tag used in virtiofsd
    const char *source = "nixstore";
    const char *target = "/nix/store";

    if (ensure_dir("/nix") || ensure_dir("/nix/store"))
        return 1;

    if (mount(source, target, "virtiofs", 0, "") == -1) {
        info(1, "Error: mounting source (%s) at target (%s)", source, target);
        return 1;
    }

    info(1, "Mounted %s at %s\n", source, target);

    return 0;
}

int main(int argc, char *argv[]) {
    int ret;
    char *init_path;

    info(1, "VMCTL image-less INIT started ... \n");
    prnt_args(argc, argv);

    ret = mount_nix_store();
    if(ret)
        return ret;

    if (argc > 2) {
        init_path = argv[2];
        info(1, "Attempting to exec: %s\n", init_path);

        execv(init_path, argv);
        info(1, "Error: execv failed");
    }

    info(1, "Error: Missing the path to init\n");
    return -1;
}
