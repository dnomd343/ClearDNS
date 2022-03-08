#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

char **crond_command = NULL;
char **adguard_command = NULL;
char **overture_command = NULL;
char **domestic_dnsproxy_command = NULL;
char **foreign_dnsproxy_command = NULL;

void error_exit(char *message);
char* read_file(char *file_name);
char** command_add_field(char **command_list, char *field);
char** dnsproxy_config(char *port, cJSON *json, int is_debug);
void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug);

void error_exit(char *message) { // exit with code 1
    fprintf(stderr, "[ClearDNS] %s\n", message);
    exit(1);
}

char* read_file(char *file_name) { // read file content
    FILE *pfile = fopen(file_name, "rb");
    if (pfile == NULL) { // open failed
        error_exit("File open failed.");
    }
    fseek(pfile, 0, SEEK_END);
    long file_length = ftell(pfile); // get file length
    char *file_content = (char*)malloc(file_length + 1); // malloc new memory
    if (file_content == NULL) {
        error_exit("No enough memory."); // file too large
    }
    rewind(pfile);
    fread(file_content, 1, file_length, pfile); // read file stream
    file_content[file_length] = '\0'; // set end flag
    fclose(pfile);
    return file_content;
}

char** command_add_field(char **command_list, char *field) { // add field into command
    int num = 0;
    while(command_list[num++] != NULL); // get options number
    command_list = (char**)realloc(command_list, sizeof(char**) * (num + 1));
    command_list[num - 1] = strcpy((char*)malloc(strlen(field) + 1), field);
    command_list[num] = NULL; // end sign
    return command_list;
}

char** dnsproxy_config(char *port, cJSON *json, int is_debug) { // generate dnsproxy command
    char *bootstrap_dns = NULL;
    cJSON *primary_dns = NULL;
    cJSON *fallback_dns = NULL;
    char **command_list = (char**)malloc(sizeof(char*) * 9);

    command_list[0] = "dnsproxy";
    command_list[1] = "--port";
    command_list[2] = port; // listen port
    command_list[3] = "--all-servers"; // parallel queries to all servers
    command_list[4] = "--cache"; // enable DNS cache
    command_list[5] = "--cache-size";
    command_list[6] = "4194304"; // cache size -> 4MiB
    command_list[7] = "--cache-optimistic"; // optimistic DNS cache
    command_list[8] = NULL; // end sign

    while (json != NULL) {
        if (!strcmp(json->string, "bootstrap")) { // bootstrap DNS server
            if (!cJSON_IsString(json)) {
                error_exit("`bootstrap` must be a string.");
            }
            bootstrap_dns = json->valuestring;
        } else if (!strcmp(json->string, "primary")) { // primary DNS server
            if (!cJSON_IsArray(json)) {
                error_exit("`primary` must be a array.");
            }
            primary_dns = json->child;
        } else if (!strcmp(json->string, "fallback")) { // fallback DNS server
            if (!cJSON_IsArray(json)) {
                error_exit("`fallback` must be a array.");
            }
            fallback_dns = json->child;
        }
        json = json->next; // next field
    }

    if (bootstrap_dns != NULL) { // add bootstrap DNS server
        command_list = command_add_field(command_list, "--bootstrap");
        command_list = command_add_field(command_list, bootstrap_dns);
    }

    if (primary_dns != NULL) { // add primary DNS server
        while (primary_dns != NULL) { // iterate over server list
            if (!cJSON_IsString(primary_dns)) {
                error_exit("DNS Server should be a string.");
            }
            command_list = command_add_field(command_list, "--upstream");
            command_list = command_add_field(command_list, primary_dns->valuestring);
            primary_dns = primary_dns->next;
        }
    } else {
        error_exit("Miss primary DNS server."); // primary DNS server required
    }

    if (fallback_dns != NULL) { // add fallback DNS server
        while (fallback_dns != NULL) { // iterate over server list
            if (!cJSON_IsString(fallback_dns)) {
                error_exit("DNS Server should be a string.");
            }
            command_list = command_add_field(command_list, "--fallback");
            command_list = command_add_field(command_list, fallback_dns->valuestring);
            fallback_dns = fallback_dns->next;
        }
    }

    if (is_debug) { // debug mode
        command_list = command_add_field(command_list, "--verbose");
    }

    return command_list;
}

void load_start_command(char *adguard_workdir, char *overture_config, char *upstream_config, int is_debug) {
    // crond command
    crond_command = (char**)malloc(sizeof(char*) * 2);
    crond_command[0] = "crond";
    crond_command[1] = NULL; // end sign
    if (is_debug) { // debug mode
        crond_command = command_add_field(crond_command, "-f"); // run in foreground
        crond_command = command_add_field(crond_command, "-l"); // log-level
        crond_command = command_add_field(crond_command, "0"); // 0 -> verbose
        crond_command = command_add_field(crond_command, "-L");
        crond_command = command_add_field(crond_command, "/dev/stdout");
    }

    // AdGuardHome command
    adguard_command = (char**)malloc(sizeof(char*) * 7);
    adguard_command[0] = "AdGuardHome";
    adguard_command[1] = "-w";
    adguard_command[2] = adguard_workdir; // workdir for AdGuardHome
    adguard_command[3] = "-p";
    adguard_command[4] = "80"; // port for web manage
    adguard_command[5] = "--no-check-update"; // skip check update (invalid in docker)
    adguard_command[6] = NULL; // end sign
    if (is_debug) { // debug mode
        adguard_command = command_add_field(adguard_command, "--verbose");
    }

    // overture command
    overture_command = (char**)malloc(sizeof(char*) * 4);
    overture_command[0] = "overture";
    overture_command[1] = "-c";
    overture_command[2] = overture_config;
    overture_command[3] = NULL; // end sign
    if (is_debug) { // debug mode
        overture_command = command_add_field(overture_command, "-v");
    }

    // dnsproxy command
    cJSON *json = NULL;
    cJSON *json_root = cJSON_Parse(read_file(upstream_config));
    if (json_root == NULL) {
        error_exit("JSON format error.");
    } else {
        json = json_root->child;
    }
    while (json != NULL) {
        if (!strcmp(json->string, "domestic")) { // domestic dnsproxy config
            if (!cJSON_IsObject(json)) {
                error_exit("`domestic` must be a object.");
            }
            domestic_dnsproxy_command = dnsproxy_config("4053", json->child, is_debug);
        } else if (!strcmp(json->string, "foreign")) { // foreign dnsproxy config
            if (!cJSON_IsObject(json)) {
                error_exit("`foreign` must be a object.");
            }
            foreign_dnsproxy_command = dnsproxy_config("6053", json->child, is_debug);
        }
        json = json->next; // next field
    }
    if (domestic_dnsproxy_command == NULL) {
        error_exit("Miss domestic DNS settings.");
    }
    if (foreign_dnsproxy_command == NULL) {
        error_exit("Miss foreign DNS settings.");
    }
    cJSON_free(json_root); // free cJSON object
}
