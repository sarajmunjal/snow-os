#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2 || argc > 2) {
        char *err_msg = "ls must have exactly 2 argument\n";
        write(1, err_msg, strlen(err_msg));
        return -1;
    }
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        char *err_msg = "No directory found with name: ";
        write(1, err_msg, strlen(err_msg));
        write(1, argv[1], strlen(argv[1]));
        return -1;
    }
    struct dirent *dirent;
    write(1, " ", 1);
    while (1) {
        dirent = readdir(dir);
        write(1, (char *) dirent->d_name, strlen(dirent->d_name));
        write(1, " ", 1);
        if (dirent == NULL) break;
    }
    closedir(dir);
    return 0;
}