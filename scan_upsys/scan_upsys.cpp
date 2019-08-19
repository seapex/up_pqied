#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>

bool IsInt(const char *str)
{
    for (int i=0; i<strlen(str); i++) {
        if(!isdigit(str[i])) return false;
    }
    return true;
}

static const char * help_info =
    "Usage: scan_upsys.exe system_ver out_path\n";
/*!
Parse command parameters

    Input:  argc --
            argv --
    Return: >0=system version, <0=invalid version.
*/
int ParseOptn(int argc, char* argv[])
{
    if (argc < 3) {
        puts(help_info);
        return -1;
    }
    if (IsInt(argv[1])) return atoi(argv[1]);
    printf("system_ver must be number!\n");
    return -2;
}

enum kUpSysFiles {kUpStartup, kUp_wer=7, kUpEnd};
/*!
Save update files information

    Input:  flags --
            path --
    Return: 0=success, -1=can't open file; -2=parse param error
*/
int SaveUpFile(const uint8_t *flags, const char *path)
{
    char file_name[128];

    sprintf(file_name, "%s/upsys_files.lst", path);
    FILE *flist = fopen(file_name, "wb");
    if (flist==NULL) {
        printf("open %s failure\n", file_name);
        return -1;
    }
    sprintf(file_name, "%s/upsys.sh", path);
    FILE *fscrpt = fopen(file_name, "wb");
    if (fscrpt==NULL) {
        printf("open %s failure\n", file_name);
        fclose(flist);
        return -1;
    }
    fprintf(fscrpt, "#!/bin/sh\n");
    fprintf(fscrpt, "echo $0 run...\n");

    const char *src, *des;
    for (int i=0; i<kUpEnd; i++) {
        if (!flags[i]) continue;
        switch (i) {
            case kUpStartup:
                src = "startup.sh";
                des = "/home/boyuu";
                break;
            case kUp_wer:
                src = "wer";
                des = "/home/boyuu";
                break;
            default:
                src = NULL;
                des = NULL;
                break;
        }
        if (src && des) {
            fprintf(fscrpt, "chmod +x %s\n", src);
            fprintf(fscrpt, "echo mv %s %s\n", src, des);
            fprintf(fscrpt, "mv %s %s\n", src, des);
            fprintf(flist, "%d:%s\n", i, src);
        }
    }

    fclose(flist);
    fclose(fscrpt);
}

/*!
Scan system files to update

    Input: ver -- version
    Output: flags
*/
void ScanUpfile(uint8_t *flags, int ver)
{
    if (ver<2) {
        memset(flags, 1, kUpEnd);
        return;
    }
    switch (ver) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        default:
            break;
    }
}

int main(int argc, char* argv[])
{
    int ver = ParseOptn(argc, argv);
    if (ver<0) return ver;

    uint8_t up_flags[kUpEnd];
    memset(up_flags, 0, sizeof(up_flags));
    ScanUpfile(up_flags, ver);
    
    SaveUpFile(up_flags, argv[2]);
}

