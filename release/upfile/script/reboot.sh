#!/bin/sh
if [ -f uImage -a -s uImage ]; then
    echo cold reboot
    sys_mngr forceboot
else
    echo reboot
    reboot
fi
