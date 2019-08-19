/*! \interpolate.h
    \brief ssh operation API.
    Copyright (c) 2019 Seapex
*/
#ifndef _SSH_API_H_
#define _SSH_API_H_

class SshApi
{
    char ip_[32];
    char port_[8];
    
    char run_cmd_[128];
    char ftp_cmd_[128];
    char cmd_buf_[256];
    int debug_;
  public:
    SshApi(int type);
    ~SshApi();

    int Run(const char *cmd, int type=0, int yn=0);
    int FileTransfer(const char *cmd, int type=0);
    
    //Mutators
    void set_ip(char *ip) { strcpy(ip_, ip); }
    void set_port(char *port) { strcpy(port_, port); }
    void set_debug(int val) { debug_ = val; }
};

enum kSshApiRunCmd {kGetSysInfo, };

#endif // _SSH_API_H_

