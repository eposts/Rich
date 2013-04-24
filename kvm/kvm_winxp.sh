#!/bin/bash
kvm -m 1024 -drive file=/mnt/E/winxp.img,cache=writeback,if=virtio,boot=on -localtime -net nic,model=virtio -net user -smp 2 -redir tcp:3389::3389 -redir tcp:8580::8580 -nographic -soundhw es1370 >/dev/null 2>&1 & ####-vnc 127.0.0.1:0  ###-vnc :0
sleep 50 ;rdesktop localhost -u administrator -p 'pass!@#!!!9' -f -N -D -z -P -r sound:local -clipboard >/dev/null 2>&1 &
