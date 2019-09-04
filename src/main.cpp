#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "config.h"
#include "up_pqied.h"
#include "lua_api.h"
#include "ssh_api.h"

struct InputPara {
    int type;
    int force;
    int prog;
    int cst61850;
    char ip[32];
    char port[8];
    int debug;
    int reboot;
};

/*!
Convert time_t to tm <local>
    Input:  src -- source
    Output: des -- destination
*/
void LocalTime(struct tm *des, const time_t *src)
{
    if (!src||!des) return;
    time_t tmt = *src;

    memcpy(des, localtime(&tmt), sizeof(tm));
}

char *NowTime()
{
    static char strtim[32];
    time_t time1 = time(NULL);
    tm tmi;
    LocalTime(&tmi, &time1);
    strftime(strtim, 32, "%Y-%m-%d %H:%M:%S", &tmi);
    return strtim;
}

enum kLuaType {kPreprocess, kDPost};
/*!
    Input:  type -- lua script type
            mnum -- map number
            stp -- step
            para --
    Return: 0=success, -1=failure, 1=needn't update
*/
int CallLua(kLuaType type, int mnum, int stp, InputPara *para)
{
    LuaApi *lua_api = new LuaApi;
    char filename[80];
    int ret = 0;
    if (kPreprocess == type) {
        if(!lua_api->LoadFile(".sys/preprocess")) {
            lua_api->Call("preprocess", "ii>i", para->type, para->debug, &ret);
        } else {
            ret = -1;
        }
    } else if (kDPost == type) {
        switch (stp) {
            case 1:
                sprintf(filename, "%sscript/%02d/a_post", WORK_PATH, mnum);
                if(!lua_api->LoadFile(filename)) {
                    lua_api->Call("reset_syscfg", "i", para->cst61850);
                }
                break;
            case 4:
                sprintf(filename, "%sscript/%02d/d_post", WORK_PATH, mnum);
                if(!lua_api->LoadFile(filename)) {
                    lua_api->Call("get_mac", "ss", para->ip, para->port);
                }
                break;
            default:
                break;
        }
    }
    delete lua_api;
    return ret;
}

/*!
    Return: 0=success, -1=can't open file; -2=parse param error
*/
int ReadParaFile(InputPara *para)
{
    FILE *fp;
    const char *file_name = kCfgFile;
    if ((fp = fopen(file_name, "r"))==NULL) {
        return -1;
    }

    char stri[64];
    char name[32], val[32];
    unsigned long scan = 0;
    while(fgets(stri, 64, fp)!=NULL) {  //Scan param in cfg file
        do {
            sscanf(stri, "%[a-zA-Z0-9_]%*[ =]%s", name, val);
            //printf("name=%s, val=%s\n", name, val);
            if (!(scan&1)) {
                if ( !strcmp(name, "ip") ) {
                    strcpy(para->ip, val); scan |= 1; break;
                }
            }
            if (!(scan&(1<<1))) {
                if ( !strcmp(name, "port") ) {
                    strcpy(para->port, val); scan |= (1<<1); break;
                }
            }
            if (!(scan&(1<<2))) {
                if ( !strcmp(name, "type") ) {
                    para->type = atoi(val); scan |= (1<<2); break;
                }
            }
            if (!(scan&(1<<3))) {
                if ( !strcmp(name, "prog") ) {
                    para->prog = atoi(val); scan |= (1<<3); break;
                }
            }
            if (!(scan&(1<<4))) {
                if ( !strcmp(name, "force") ) {
                    para->force = atoi(val); scan |= (1<<4); break;
                }
            }
            if (!(scan&(1<<5))) {
                if ( !strcmp(name, "cst61850") ) {
                    para->cst61850 = atoi(val); scan |= (1<<5); break;
                }
            }
            if (!(scan&(1<<6))) {
                if ( !strcmp(name, "reboot") ) {
                    para->reboot = atoi(val); scan |= (1<<6); break;
                }
            }
        } while(0);
        if (scan==0x7f) break;
    }
    fclose(fp);
    if (scan==0x7f) return 0;
    else return -2;
}

static const char * help_info =
    "Usage: upactuator.exe ip [port] [dbg]\n";
/*!
Parse command parameters

    Input:  argc --
            argv --
    Output: para
    Return: <0=exception, 0=no debug, 1=debug, 100=version
*/
int ParseOptn(InputPara *para, int argc, char* argv[])
{
    //for (int i=0; i<argc; i++) printf("%s ", argv[i]);
    //printf("\n");
    
    if (argc < 2) {
        puts(help_info);
        return -1;
    }
    if (!strcmp(argv[1], "-v")) return 100;
    
    if (ReadParaFile(para)<0) return -2;
    if (para->type<1) {
        printf("Invalid type:%d\n", para->type);
        return -3;
    }
    strcpy(para->ip, argv[1]);
    int dbg = 0;
    for (int i=2; i<argc; i++) {
        if (!strcmp(argv[i], "dbg")) {
            dbg = 1;
        } else {
            strcpy(para->port, argv[i]);
        }
    }

    return dbg;
}

/*!
    Return: 0=success, -1=failure, 1=needn't update, 2=System too old, need to update by serial
*/
int main(int argc, char* argv[])
{
    //printf("V%d.%d.%d Copyright(C) 2019 boyuu\n", _VERSION_MAJOR, _VERSION_MINOR, _VERSION_PATCH);
    
    InputPara para;
    para.debug = ParseOptn(&para, argc, argv);
    if (para.debug<0) {
        printf("_debug_:%d@%s. para.debug=%d\n", __LINE__, __FUNCTION__, para.debug);
        return para.debug;
    }
    if (para.debug==100) {
        printf("up_pqied %d.%d.%d\n", _VERSION_MAJOR, _VERSION_MINOR, _VERSION_PATCH);
        uint32_t ver = (_VERSION_MAJOR<<22) + (_VERSION_MINOR<<10) + _VERSION_PATCH;
        return ver;
    }

    printf("Update start %s\n", NowTime());
    SshApi *ssh_api = new SshApi(0);
    ssh_api->set_ip(para.ip);
    ssh_api->set_port(para.port);
    ssh_api->set_debug(para.debug);

    ssh_api->Run("uname -srm", 0, 1);
    char stri[128];
    sprintf(stri, "-m %sscript/init.scr", WORK_PATH);
    ssh_api->Run(stri, 1);
    
    int suc = 0;
    int ret, step;
    if (para.type<=10) {  //update device software
        sprintf(stri, "-b %sscript/getver.scr", WORK_PATH);
        ssh_api->FileTransfer(stri, 1);
        suc = CallLua(kPreprocess, para.type, 0, &para);
        if (0==suc) step = 0xf;
        else step = 0;
    } else if (para.type<100) {   //other command
        step = 0xf;
        para.reboot = 0;
    } else {    //disable command
        step = 0;
        suc = -1;
    }

    const char script_name[4][16] = {"a_pre.scr", "b_upload.scr", "c_update.scr", "d_post.scr"};
    char filename[128];
    FILE *fp;
    for (int i=0; i<4; i++) {
        if ((step>>i&1)==0) continue;
        sprintf(filename, "%sscript/%02d/%s", WORK_PATH, para.type, script_name[i]);
        fp = fopen(filename, "r");
        if(fp) {
            fclose(fp);
            printf("%d...\n", i+1);
            switch (i) {
                case 0:
                    sprintf(stri, "-m %s", filename);
                    ssh_api->Run(stri, 1);
                    break;
                case 2:
                    sprintf(stri, "-m %s", filename);
                    ret = ssh_api->Run(stri, 1);
                    if (ret) suc = -1; 
                    break;
                case 1:
                case 3:
                    sprintf(stri, "-b %s", filename);
                    ret = ssh_api->FileTransfer(stri, 1);
                    if (ret) suc = -1;
                    break;
                default:
                    break;
            }
        }
        CallLua(kDPost, para.type, i+1, &para);
        if (i==3) {
            sprintf(filename, "%sup_result", WORK_PATH);
            fp = fopen(filename, "r");
            if (fp) {
                char fail_inf[64];
                fgets(fail_inf, 64, fp);
                while (fgets(fail_inf, 64, fp)) {
                    printf(fail_inf);
                    suc = -1;
                }
                fclose(fp);
                remove(filename);
            }
        }
    }

    if (suc==0) {
        printf("update succeeded!\n");
        if (para.reboot && !para.debug) {
            sprintf(stri, "-m %sscript/%02d/reboot.scr", WORK_PATH, para.type);
            ssh_api->Run(stri, 1);
        }
    } else if (suc==1) printf("Don't need to update!\n");
    else printf("update failed!\n");
    printf("Update end %s\n", NowTime());
    if (para.debug) system("pause");
    if (suc != 1) system("rmdir .sys\\up_tmp /Q /S");
    return suc;
}

