**Programer manual of up_pqied**

## Summary
```
Usage: up_pqied ip [port] [dbg]
       up_pqied -v | -luav
```
其他参数从配置文件读取

### main_flow
1. Run the .sys/script/init.scr, Get system information from device & prepare the running environment.
```
if (update program) {
    2. Get version information.
    3. Run the preproccess.lua for update program.
}
```
4. Run the .sys/script/[a_pre.scr, b_upload.scr, c_update.scr, d_post.scr...] in sequence, if it is exists.

## ConfigFile
### general update.cfg
```bash
ip = 192.168.1.89
port = 12307
typeidx = 0 # type index, only used for up_pqgui.exe
type = 1    # command type, i.e. script number. refer to .sys/type_map.lst & .sys/script for detail
force = 0   # force update. 0=disable, 1=enable
prog = 0x1  # program list to force an update. bit0-bit31, 0=no, 1=yes. refer to .sys/force_up.lst or detail
cst61850 = 2    # 61850 customer. refer to .sys/cst61850.lst for detail
reboot = 1  # Whether to reboot after the update. 0=no, 1=yes
debug = 0   # debug mode. 0=no, 1=yes
kernel = 0  # Whether update kernel, 0=no, 1=yes
vendor = 1  # vendor number. refer to vendors.lst for detail
```

### type_map sys/script/type_map.lst
```bash
# num:name
# length of name not over 60 char
# total number of items not over 32
# >100 hide menu
1:PQNet300D
2:_PQAny316
11:Get debug information
12:Get sysconfig
13:Get event_info
14:Get cycle10
15:_Get MAC address
...
```

### cst61850 upfile/61850/cst61850.lst
```bash
#num:name
#length of name not over 60 char
1:Fujian 福建电网
2:Hebei 河北电网
3:Henan 河南电网
4:Shanxi 山西电网
5:StateGrid 国网
...
```

## VerInfoFile
### system version.inf
```bash
ied_model="PQNet300D"
pqnet_mn="0.4.0"
pqnet_gui="0.3.1"
system="1"
kernel="1"
vendor="1"
svx="0.0.1"
```

### s61850 ver61850.inf
```bash
custom=Fujian
server="2.0.19"
model="1.0.17"
config=9
db=6
```

## FAQ
#### Q1 执行 plink 命令后，系统提示"Unable to open connection: Opening '\\.\COM1': Error?"
plink 命令的运行基于putty保存在注册表中的参数. 如果putty的 "Default settings" 设为了 串口COM1就会出现该问题. 把 "Default settings" 设为SSH 即可.
