#!/bin/sh
# /usr/bin/wx_oauth.sh
# Usage: wx_oauth.sh CODE
# Output: "openid nickname"

APPID="你的公众号APPID"
APPSECRET="你的公众号APPSECRET"

CODE="$1"
[ -z "$CODE" ] && exit 1

TMP=$(mktemp /tmp/wx_oauth.XXXXXX)

# Step1: 获取 access_token 和 openid
curl -s \
"https://api.weixin.qq.com/sns/oauth2/access_token?appid=${APPID}&secret=${APPSECRET}&code=${CODE}&grant_type=authorization_code" \
> "$TMP"

ERRCODE=$(grep -o '"errcode":[0-9]*' "$TMP" | head -n1 | sed 's/.*://')
if [ -n "$ERRCODE" ] && [ "$ERRCODE" != "0" ]; then
    rm -f "$TMP"
    exit 2
fi

OPENID=$(grep -o '"openid":"[^"]*' "$TMP" | head -n1 | sed 's/.*"openid":"//')
ACCESS_TOKEN=$(grep -o '"access_token":"[^"]*' "$TMP" | head -n1 | sed 's/.*"access_token":"//')

if [ -z "$OPENID" ] || [ -z "$ACCESS_TOKEN" ]; then
    rm -f "$TMP"
    exit 3
fi

# Step2: 获取用户信息（昵称）
curl -s \
"https://api.weixin.qq.com/sns/userinfo?access_token=${ACCESS_TOKEN}&openid=${OPENID}" \
> "$TMP"

NICK=$(grep -o '"nickname":"[^"]*' "$TMP" | head -n1 | sed 's/.*"nickname":"//' | sed 's/\\"/"/g')

rm -f "$TMP"

if [ -n "$NICK" ]; then
    echo "$OPENID $NICK"
else
    echo "$OPENID"
fi

exit 0