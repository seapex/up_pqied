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

enum kUpSysFiles {kUpStartup, kUp_wer, kUp_fstab, kUp_ntpclient, kUp_sys_mngr, 
    kUp_deamon, kUp_run_mn, kUp_run_gui, kCommu4SCNet, kBusybox, kUp_ntpclient_sh, 
    kUp_ne_ftps_sh, kUp_sys4_23, kUpEnd};
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
    int m = 0;
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
            case kUp_fstab:
                src = "fstab";
                des = "/etc";
                break;
            case kUp_ntpclient:
                src = "ntpclient";
                des = "/usr/sbin";
                break;
            case kUp_sys_mngr:
                src = "sys_mngr";
                des = "/usr/local/bin";
                break;
            case kUp_deamon:
                src = "deamon_run.sh";
                des = "/usr/sbin";
                break;
            case kUp_run_mn:
                src = "run_mn.sh";
                des = "/home/boyuu";
                break;
            case kUp_run_gui:
                src = "run_gui.sh";
                des = "/home/boyuu/gui";
                break;
            case kCommu4SCNet:
                src = "commu4scnet";
                des = "/home/boyuu/tools";
                break;
            case kBusybox:
                src = "busybox";
                des = "/bin";
                break;
            case kUp_ntpclient_sh:
                src = "ntpclient.sh";
                des = "/home/boyuu/ntp";
                break;
            case kUp_ne_ftps_sh:
                src = "ne_ftps.sh";
                des = "/home/boyuu/tools";
                break;
            case kUp_sys4_23:
                src = "upsys4_23.sh";
                fprintf(fscrpt, "chmod +x %s\n", src);
                fprintf(fscrpt, "./%s\n", src);
                fprintf(flist, "%d:%s\n", m++, src);

                fprintf(flist, "%d:%s\n", m++, "start_ntpd.sh");
                fprintf(flist, "%d:%s\n", m++, "meas_frq_err.sh");
                fprintf(flist, "%d:%s\n", m++, "ntp_ipv4.lua");
                fprintf(flist, "%d:%s\n", m++, "ntp.conf");
                fprintf(flist, "%d:%s\n", m++, "lua");
                fprintf(flist, "%d:%s\n", m++, "set_ipv6.lua");
            default:
                src = NULL;
                des = NULL;
                break;
        }
        if (src && des) {
            fprintf(fscrpt, "chmod +x %s\n", src);
            fprintf(fscrpt, "echo mv %s %s\n", src, des);
            fprintf(fscrpt, "mv %s %s\n", src, des);
            fprintf(flist, "%d:%s\n", m++, src);
        }
    }

    fclose(flist);
    fclose(fscrpt);
    return 0;
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
            flags[kUp_fstab] = 1;
        case 3:
            flags[kUp_deamon] = 1;
            flags[kUp_run_mn] = 1;
            flags[kUp_run_gui] = 1;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            flags[kBusybox] = 1;
        case 10:
        case 11:
            flags[kUp_wer] = 1;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            flags[kUp_sys_mngr] = 1;
        case 20:
            flags[kUp_ne_ftps_sh] = 1;
        case 21:
        case 22:
            flags[kUp_sys4_23] = 1;
            flags[kUpStartup] = 1;
            flags[kCommu4SCNet] = 1;
        case 23:
        default:
            break;
    }
}

int main(int argc, char* argv[])
{
    //printf("argc=%d %s %s %s\n", argc, argv[0], argv[1], argv[2]);
    int ver = ParseOptn(argc, argv);
    if (ver<0) return ver;

    uint8_t up_flags[kUpEnd];
    memset(up_flags, 0, sizeof(up_flags));

    ScanUpfile(up_flags, ver);
    
    SaveUpFile(up_flags, argv[2]);
}

