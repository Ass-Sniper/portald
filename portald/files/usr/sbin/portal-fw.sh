#!/bin/sh
# 简单 iptables/ipset 版 Portal 防火墙
# 依赖：ipset, iptables, kmod-ipt-ipset 已安装

CONFIG=/etc/portald/config
LAN_IF="br-lan"        # 视你的实际 LAN 口而定，可以 uci get network.lan.ifname
PORT=8080             # portal 监听端口，下面会从 config 里覆盖

# 从 /etc/portald/config 里提取简单 key=value
cfg_get() {
    # 用法：cfg_get key
    local key="$1"
    grep -E "^$key[[:space:]]*=" "$CONFIG" 2>/dev/null | \
        head -n1 | sed 's/^[^=]*=\s*//'
}

start_firewall() {
    [ -f "$CONFIG" ] || return 0

    # 读取配置
    PORT="$(cfg_get port)"
    [ -z "$PORT" ] && PORT=8080

    IP_WL="$(cfg_get ip_whitelist)"
    MAC_WL="$(cfg_get mac_whitelist)"

    # 创建 ipset
    ipset create portal_authed hash:ip,mac timeout 7200 -exist
    ipset create portal_ip_whitelist hash:ip -exist

    # 填充 IP 白名单
    for ip in $(echo "$IP_WL" | tr ',' ' '); do
        [ -n "$ip" ] && ipset add portal_ip_whitelist "$ip" -exist
    done

    # 自定义 chain
    iptables -t nat    -N PORTAL_PREROUTING 2>/dev/null
    iptables -t filter -N PORTAL_FORWARD    2>/dev/null

    # 确保跳转链存在（插在最前面）
    iptables -t nat    -D PREROUTING -i "$LAN_IF" -j PORTAL_PREROUTING 2>/dev/null
    iptables -t nat    -I PREROUTING 1 -i "$LAN_IF" -j PORTAL_PREROUTING

    iptables -t filter -D FORWARD -i "$LAN_IF" -j PORTAL_FORWARD 2>/dev/null
    iptables -t filter -I FORWARD 1 -i "$LAN_IF" -j PORTAL_FORWARD

    # 清空自定义链
    iptables -t nat    -F PORTAL_PREROUTING
    iptables -t filter -F PORTAL_FORWARD

    ######## NAT PREROUTING：重定向 HTTP / DNS ########

    # 1) IP 白名单直接返回
    iptables -t nat -A PORTAL_PREROUTING -m set --match-set portal_ip_whitelist src -j RETURN

    # 2) MAC 白名单（用 -m mac，就不建 mac 的 ipset 了）
    for mac in $(echo "$MAC_WL" | tr ',' ' '); do
        [ -n "$mac" ] && \
        iptables -t nat -A PORTAL_PREROUTING -m mac --mac-source "$mac" -j RETURN
    done

    # 3) 已认证用户：ip+mac 命中 portal_authed
    iptables -t nat -A PORTAL_PREROUTING -m set --match-set portal_authed src,src -j RETURN

    # 4) DNS 劫持到本机 53（假设 dnsmasq 在 53）
    iptables -t nat -A PORTAL_PREROUTING -p udp --dport 53 -j REDIRECT --to-ports 53
    iptables -t nat -A PORTAL_PREROUTING -p tcp --dport 53 -j REDIRECT --to-ports 53

    # 5) HTTP 重定向到 Portal（端口由配置决定）
    iptables -t nat -A PORTAL_PREROUTING -p tcp --dport 80 -j REDIRECT --to-ports "$PORT"

    ######## FORWARD：只放行白名单 + 已认证 ########

    # IP 白名单
    iptables -t filter -A PORTAL_FORWARD -m set --match-set portal_ip_whitelist src -j ACCEPT

    # MAC 白名单
    for mac in $(echo "$MAC_WL" | tr ',' ' '); do
        [ -n "$mac" ] && \
        iptables -t filter -A PORTAL_FORWARD -m mac --mac-source "$mac" -j ACCEPT
    done

    # 已认证用户
    iptables -t filter -A PORTAL_FORWARD -m set --match-set portal_authed src,src -j ACCEPT

    # 其他一律丢弃
    iptables -t filter -A PORTAL_FORWARD -j REJECT

    echo "[portal-fw] started (LAN_IF=$LAN_IF, PORT=$PORT)"
}

stop_firewall() {
    # 删除跳转链
    iptables -t nat    -D PREROUTING -i "$LAN_IF" -j PORTAL_PREROUTING 2>/dev/null
    iptables -t filter -D FORWARD   -i "$LAN_IF" -j PORTAL_FORWARD    2>/dev/null

    # 清空并删除自定义链
    iptables -t nat    -F PORTAL_PREROUTING 2>/dev/null
    iptables -t nat    -X PORTAL_PREROUTING 2>/dev/null

    iptables -t filter -F PORTAL_FORWARD    2>/dev/null
    iptables -t filter -X PORTAL_FORWARD    2>/dev/null

    # 不删除 ipset（session 交给 portald 管）
    echo "[portal-fw] stopped"
}

case "$1" in
    start)  start_firewall ;;
    stop)   stop_firewall  ;;
    restart|reload)
        stop_firewall
        start_firewall
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac
