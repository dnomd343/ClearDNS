#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "logger.h"
#include "sundry.h"
#include "constant.h"

int run_command(const char *command) { // running command with system shell
    log_debug("Run command -> `%s`", command);
    int ret = system(command) / 256;
    if (ret != 0) {
        log_warn("Command `%s` return non-zero code -> %d", command, ret);
    }
    return ret;
}

void remove_file(const char *file) {
    char *remove_cmd = string_join("rm -f ", file);
    run_command(remove_cmd);
    free(remove_cmd);
}

void create_folder(const char *folder) { // create folder
    if (!access(folder, 0)) { // target is file or folder
        struct stat buf;
        stat(folder, &buf);
        if (!(S_IFDIR & buf.st_mode)) { // target is not folder
            log_error("Create folder failed -> target is file");
        }
        return;
    }
    log_debug("Create folder -> %s", folder);
    char *command = string_join("mkdir -p ", folder);
    system(command);
    free(command);
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
