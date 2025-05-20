#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void read_cmdline() {
    FILE *f = fopen("/proc/cmdline", "r");
    if (!f) die("fopen /proc/cmdline");

    char line[4096];
    if (fgets(line, sizeof(line), f)) {
        printf("Kernel cmdline: %s\n", line);
    } else {
        die("fgets /proc/cmdline");
    }

    fclose(f);
}

void ensure_dir(const char *path) {
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        die("mkdir");
    }
}

void mount_nix_store() {
    const char *source = "nixstore";  // This must match the tag used in virtiofsd
    const char *target = "/nix/store";

    ensure_dir("/nix");
    ensure_dir("/nix/store");

    if (mount(source, target, "virtiofs", 0, "") == -1) {
        die("mount /nix/store");
    }

    printf("Mounted %s at %s\n", source, target);
}

int main() {
    printf("Custom init started (PID 1)\n");
    exit (1);

    ensure_dir("/proc");
    if (mount("proc", "/proc", "proc", 0, "") == -1) {
        die("mount /proc");
    }

    read_cmdline();
    mount_nix_store();

    // Enter a simple shell or just sleep
    printf("Init complete. Starting shell or sleeping...\n");
    execl("/bin/sh", "/bin/sh", NULL);
    perror("execl /bin/sh failed, falling back to pause");

    while (1) {
        pause(); // keep PID 1 alive
    }

    return 0;
}
