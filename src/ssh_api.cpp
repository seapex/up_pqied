#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssh_api.h"
#include "up_pqied.h"

/*!
    Input: type -- 0=x86, 1=linux
*/
SshApi::SshApi(int type)
{
    if (type) { //linux
        
        
    } else {    //x86
        sprintf(run_cmd_, "%splink", WORK_PATH);
        sprintf(ftp_cmd_, "%spsftp", WORK_PATH);
    }
    debug_ = 0;
}

SshApi::~SshApi()
{

}

static const char * kPasswd = "Boyuu;PQNet01";

/*!
Remote run command

    Input:  cmd -- Command to run on remote terminal
            type -- 0=single command, 1=batch command
            yn -- Automatic answer. 0=no answer, 1=answer yes, 2=answer no
*/
int SshApi::Run(const char *cmd, int type, int yn)
{
    char stri[128];
    if (type) { //batch
        sprintf(stri, "-batch %s", cmd);
    } else {    //single
        strcpy(stri, cmd);
    }
    if (yn) {
        sprintf(&stri[strlen(stri)], " < %s", yn==1?kRespYesFile:kRespNoFile);
    }
    sprintf(cmd_buf_, "%s -i %s -P %s root@%s %s", run_cmd_, kKeyFile, port_, ip_, stri);
    //sprintf(cmd_buf_, "%s -pw %s -P %s root@%s %s", run_cmd_, kPasswd, port_, ip_, stri);
    if (debug_) printf("%s\n", cmd_buf_);
    return system(cmd_buf_);
}

/*!
File transfer

    Input:  cmd -- ftp command
            type -- 0=single command, 1=batch command
*/
int SshApi::FileTransfer(const char *cmd, int type)
{
    char stri[128];
    if (type) { //batch
        sprintf(stri, "-batch -be %s", cmd);
    } else {    //single
        strcpy(stri, cmd);
    }
    sprintf(cmd_buf_, "%s -i %s -P %s root@%s %s", ftp_cmd_, kKeyFile, port_, ip_, stri);
    if (debug_) printf("%s\n", cmd_buf_);
    return system(cmd_buf_);
}

