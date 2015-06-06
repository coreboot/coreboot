#!/bin/bash
# $1: file containing text

. ~/.wikiaccount
WIKIAPI="http://www.coreboot.org/api.php"
TITLE="Supported_Motherboards"
cookie_jar="/tmp/wikicookiejar"
#Will store file in wikifile

#################login
#Login part 1
CR=$(curl -sS \
        --location \
        --retry 2 \
        --retry-delay 5\
        --cookie $cookie_jar \
        --cookie-jar $cookie_jar \
        --user-agent "Curl Shell Script" \
        --keepalive-time 60 \
        --header "Accept-Language: en-us" \
        --header "Connection: keep-alive" \
        --compressed \
        --data-urlencode "lgname=${USERNAME}" \
        --data-urlencode "lgpassword=${USERPASS}" \
        --request "POST" "${WIKIAPI}?action=login&format=json")

TOKEN=`echo $CR| sed -e 's,^.*"token":"\([^"]*\)".*$,\1,'`
if [ -z "$TOKEN" ]; then
        exit
fi

#Login part 2
CR=$(curl -sS \
        --location \
        --cookie $cookie_jar \
    --cookie-jar $cookie_jar \
        --user-agent "Curl Shell Script" \
        --keepalive-time 60 \
        --header "Accept-Language: en-us" \
        --header "Connection: keep-alive" \
        --compressed \
        --data-urlencode "lgname=${USERNAME}" \
        --data-urlencode "lgpassword=${USERPASS}" \
        --data-urlencode "lgtoken=${TOKEN}" \
        --request "POST" "${WIKIAPI}?action=login&format=json")

###############
#Get edit token
CR=$(curl -sS \
        --location \
        --cookie $cookie_jar \
        --cookie-jar $cookie_jar \
        --user-agent "Curl Shell Script" \
        --keepalive-time 60 \
        --header "Accept-Language: en-us" \
        --header "Connection: keep-alive" \
        --compressed \
        --request "POST" "${WIKIAPI}?action=query&meta=tokens&format=json")

EDITTOKEN=`echo $CR| sed -e 's,^.*"csrftoken":"\([^"]*\)".*$,\1,'`
EDITTOKEN=`printf "$EDITTOKEN"`
if [ ${#EDITTOKEN} != 34 ]; then
        exit
fi
#########################

CR=$(curl -sS \
        --location \
        --cookie $cookie_jar \
        --cookie-jar $cookie_jar \
        --user-agent "Curl Shell Script" \
        --keepalive-time 60 \
        --header "Accept-Language: en-us" \
        --header "Connection: keep-alive" \
        --header "Expect:" \
        --form "token=${EDITTOKEN}" \
        --form "title=${TITLE}" \
        --form "text=<$1" \
        --request "POST" "${WIKIAPI}?action=edit&")
