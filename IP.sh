#!/bin/bash
while true
do
wget http://iframe.ip138.com/ic.asp -O /tmp/ip
sendemail -t somebody@gmail.com -f somebody@gmail.com -s smtp.gmail.com -u `egrep "[0-9]{1,3}\." /tmp/ip |awk -F "[" '{print $2}'|awk -F "]" '{print $1}'` -a /tmp/ip -m `cat /tmp/ip` -xu somebody -xp password
sleep 600
done
