#!/bin/sh
# /usr/bin/send_sms.sh PHONE CODE
PHONE="$1"
CODE="$2"

LOG=/var/log/portald-sms.log
mkdir -p /var/log 2>/dev/null

echo "$(date '+%F %T') SEND SMS to ${PHONE}: code=${CODE}" >> "$LOG"

# TODO: 在这里接入真实短信平台：
# 例如：
# curl -s 'https://sms-provider.example.com/send' \
#   -d "phone=${PHONE}" -d "content=Your code is ${CODE}"

exit 0