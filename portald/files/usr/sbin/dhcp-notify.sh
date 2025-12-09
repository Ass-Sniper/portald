#!/bin/sh
# dhcp-notify.sh
# Called by dnsmasq whenever DHCP lease changes

EVENT="$1"     # add / old / del
MAC="$2"
IP="$3"
HOSTNAME="$4"

PORTALD_URL="http://127.0.0.1:2060/api/dhcp"

logger -t portald "DHCP event=${EVENT} mac=${MAC} ip=${IP} hostname=${HOSTNAME}"

# 发送事件给 portald 守护进程
curl -s -X POST "${PORTALD_URL}" \
	-d "event=${EVENT}" \
	-d "mac=${MAC}" \
	-d "ip=${IP}" \
	-d "hostname=${HOSTNAME}" >/dev/null 2>&1

exit 0
