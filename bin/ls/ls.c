#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define PATH_LEN 1024

int main(int argc, char *argv[], char *envp[]) {
    if (argc > 2) {
        char *err_msg = "Incorrect number of arguments\n";
        write(1, err_msg, strlen(err_msg));
        while (1);
        return 0;
    }
    char dir_to_open[PATH_LEN];
    if (argc == 1) {
        getcwd(dir_to_open, PATH_LEN);
    }
    else {
        strcpy(dir_to_open, argv[1]);
    }
    DIR *dir = opendir(dir_to_open);
    if (dir == NULL) {
        char *err_msg = "No directory found with name: ";
        write(1, err_msg, strlen(err_msg));
        write(1, dir_to_open, strlen(dir_to_open));
        while (1);
        return 0;
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
    while (1);
    /*char buff[256];
    getcwd((char *) buff, 256);
    chdir("bin/test");
    getcwd((char *) buff, 256);*/
    return 0;
}