#!/bin/bash
#set -x

logout()
{
curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" -d "language=English&userip=null&serialNo=$SN&basip=" -e "http://192.168.252.251:8080/portal/online_funcButton.jsp?language=English&userip=null&basip=&serialNo=$SN" "http://192.168.252.251:8080/portal/logout.jsp" --stderr /dev/null -o /dev/null
rm /tmp/cookie.txt
rm /tmp/tmpfile_sch
rm /tmp/hbstatus
exit
}

trap "logout" 1 2 3 9 15

while true
do
TIME=`date +%s%N|cut -c1-13`

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -L -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" "http://192.168.252.251:8080/portal/index_default.jsp" --stderr /dev/null -o /dev/null

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" -d "userName=$USERNAME&userPwd=$PASSWORD&isQuickAuth=false&language=Chinese&browserFinalUrl=&userip=null" -e "http://192.168.252.251:8080/portal/index_default.jsp" "http://192.168.252.251:8080/portal/login.jsp" --stderr /dev/null -o /tmp/tmpfile_sch

SN=`grep serialNo /tmp/tmpfile_sch |awk -F "=" '{print $4}' |awk -F "\"" '{print $2}'`

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" -d "language=Chinese&heartbeatCyc=240000&heartBeatTimeoutMaxTime=3&userDevPort=IAG_5000-vlan-02-0000@vlan&userStatus=99&userip=null&serialNo=$SN&basip="  -e "http://192.168.252.251:8080/portal/login.jsp" "http://192.168.252.251:8080/portal/online.jsp" --stderr /dev/null -o /dev/null

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" "http://192.168.252.251:8080/portal/online_heartBeat.jsp?heartbeatCyc=240000&heartBeatTimeoutMaxTime=3&language=Chinese&userDevPort=IAG_5000-vlan-02-0000@vlan&userStatus=99&userip=null&basip=&serialNo=$SN" -e "http://192.168.252.251:8080/portal/online.jsp" --stderr /dev/null -o /dev/null

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" "http://192.168.252.251:8080/portal/online_showTimer.jsp?language=Chinese&startTime=$TIME" -e "http://192.168.252.251:8080/portal/online.jsp" --stderr /dev/null -o /dev/null

curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" "http://192.168.252.251:8080/portal/online_funcButton.jsp?language=Chinese&userip=null&basip=&serialNo=$SN" -e "http://192.168.252.251:8080/portal/online.jsp" --stderr /dev/null -o /dev/null


while true
do
NSN=`grep serialNo /tmp/tmpfile_sch |awk -F "=" '{print $4}' |awk -F "\"" '{print $2}'`
curl -A "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)" --cookie-jar /tmp/cookie.txt --cookie /tmp/cookie.txt -H "Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*" -H "Accept-Language: zh-cn" -H "Accept-Encoding: gzip, deflate" -H "Connection: Keep-Alive" "http://192.168.252.251:8080/portal/doHeartBeat.jsp?heartBeatTimeoutMaxTime=3&language=Chinese&userDevPort=IAG_5000-vlan-02-0000@vlan&userip=null&serialNo=$NSN&basip=&userStatus=99" -e "http://192.168.252.251:8080/portal/online_heartBeat.jsp?heartbeatCyc=240000&heartBeatTimeoutMaxTime=3&language=Chinese&userDevPort=IAG_5000-vlan-02-0000@vlan&userStatus=99&userip=null&basip=&serialNo=$NSN" --stderr /dev/null -o /tmp/hbstatus

grep -q clearTimer /tmp/hbstatus
if [ $? -eq 0 ]
then
    break
fi

sleep 240
done
done
