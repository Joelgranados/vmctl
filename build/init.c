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

int main(int argc, char *argv[]) {
    printf("Custom init started (PID 1)\n");
    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    mount_nix_store();

    if (argc > 2) {
        char exec_path[4096];
        snprintf(exec_path, sizeof(exec_path), "%s/init", argv[2]);
        printf("Attempting to exec: %s\n", exec_path);

        // Shift argv to forward all arguments starting from argv[2]
        char **new_argv = &argv[2];
        new_argv[0] = exec_path;

        execv(exec_path, new_argv);
        perror("execv failed");
    } else {
        fprintf(stderr, "Insufficient arguments to determine init binary path\n");
    }

    while (1) {
        pause(); // keep PID 1 alive
    }

    return 0;
}

