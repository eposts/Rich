#!/bin/bash
Agent="Firefox"

for username in $(cat USER_FILE)
    do
curl -A "$Agent" --cookie-jar cookie.txt "http://passport.sohu.com/web/dispatchAction.action?appid=1000&ru=http://mail.sohu.com/reg/signup_success.jsp" -e http://mail.sohu.com/ -o /dev/null --stderr /dev/null

curl -A "$Agent" --cookie cookie.txt "http://passport.sohu.com/servlet/Captcha?time=a" -o verify.jpg -e "http://passport.sohu.com/web/dispatchAction.action?appid=1000&ru=http://mail.sohu.com/reg/signup_success.jsp" --stderr /dev/null

gpicview verify.jpg &
echo "Input verifycode"
read verify
verifycode=`echo -n $verify |xxd|awk -F ":" '{print $2}'|tr -d " "|tr -d "."|tr [a-z] [A-Z]|sed ':a;s/\(.*[0-9A-Z]\)\([0-9A-Z]\{2\}\)/\1%\2/;ta'`

curl -A "$Agent" --cookie cookie.txt "http://passport.sohu.com/web/Passportregister.action" -d "shortname=$username&domainName=sohu.com&user.password=$USERNAME&password2=$PASSWORD&validate=%$verifycode&uuidCode=&app_para=appid%3D1000&ot_registerid=&registerid=&appid=1000&autologin=1&ru=http%3A%2F%2Fmail.sohu.com%2Freg%2Fsignup_success.jsp&registerType=Passport&showAllType=0" -e "http://passport.sohu.com/web/dispatchAction.action?appid=1000&ru=http://mail.sohu.com/reg/signup_success.jsp" -o result.html --stderr /dev/null

grep -q "The URL has moved" result.html

if [ $? -eq 0 ]
then
echo $username >> username.txt
echo $username >> ../username.txt
echo SUCCESS
fi
killall gpicview >/dev/null 2>&1
done
