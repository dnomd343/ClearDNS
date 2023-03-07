#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "logger.h"
#include "sundry.h"
#include "system.h"
#include "constant.h"

int run_command(const char *command) { // running command under system shell
    log_debug("Run command -> `%s`", command);
    int ret_code = system(command) / 256;
    if (ret_code != 0) {
        log_warn("Command `%s` return non-zero code %d", command, ret_code);
    }
    return ret_code;
}

void create_folder(const char *folder) { // create folder
    if (!access(folder, 0)) { // target is file or folder
        struct stat buf;
        stat(folder, &buf);
        if (!(S_IFDIR & buf.st_mode)) { // target is not folder
            log_error("Create folder `%s` failed -> target is file", folder);
        } else {
            log_debug("Create folder `%s` skip -> folder exist", folder);
        }
    } else if (mkdir(folder, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) { // folder -> 755
        log_perror("Create folder `%s` failed -> ", folder);
    } else {
        log_debug("Create folder `%s` success", folder);
    }
}

uint8_t is_file_exist(const char *file) { // whether file exist
    if (!access(file, 0)) { // target is file or folder
        struct stat buf;
        stat(file, &buf);
        if (S_IFREG & buf.st_mode) { // target is file
            return TRUE;
        }
    }
    return FALSE;
}

void save_file(const char *file, const char *content) { // save content into file
    log_debug("Write into `%s` ->\n%s", file, content);
    FILE* fp = fopen(file , "w");
    if (fp == NULL) {
        log_fatal("Fail to open file -> %s", file);
    }
    fputs(content, fp);
    fclose(fp);
    log_debug("Save `%s` success", file);
}

void file_append(const char *base_file, const char *append_file) { // append_file >> base_file
    char *append_cmd = string_load("cat %s >> %s", append_file, base_file);
    run_command(append_cmd);
    free(append_cmd);
}

char* read_file(const char *file) { // read file content
    log_debug("Read file -> %s", file);
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) { // file open failed
        log_fatal("File `%s` open failed", file);
    }
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp); // get file length
    char *content = (char*)malloc(length + 1); // malloc new memory
    if (content == NULL) {
        log_fatal("No enough memory for reading file"); // file too large
    }
    rewind(fp);
    fread(content, 1, length, fp); // read file stream
    content[length] = '\0'; // set end flag
    fclose(fp);
    log_debug("File `%s` read success ->\n%s", file, content);
    return content;
}

void save_string_list(const char *file, char **string_list) { // save string list into file
    log_debug("Write string list into `%s`", file);
    FILE* fp = fopen(file , "w");
    if (fp == NULL) {
        log_fatal("Fail to open file -> %s", file);
    }
    for (char **string = string_list; *string != NULL; ++string) {
        log_debug("File append -> `%s`", *string);
        fputs(*string, fp);
        fputs("\n", fp);
    }
    fclose(fp);
    log_debug("Save `%s` success", file);
}
