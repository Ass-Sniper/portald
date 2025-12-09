local m, s, o

m = Map("portald", translate("Portal 配置"))

s = m:section(TypedSection, "portald", translate("主配置"))
s.anonymous = true
s.addremove = false

-- WeChat
o = s:option(Value, "wx_appid", translate("微信 AppID"))
o.rmempty = true

o = s:option(Value, "wx_secret", translate("微信 AppSecret"))
o.password = true
o.rmempty = true

o = s:option(Value, "wx_redirect", translate("微信回调地址"))
o.placeholder = "http://192.168.16.254:8080/wx/callback"

-- SMS
o = s:option(Value, "sms_script", translate("短信脚本路径"))
o.placeholder = "/usr/bin/send_sms.sh"

-- DB
o = s:option(Value, "db_path", translate("数据库路径"))
o.placeholder = "/etc/portald/portal.db"

-- Port / Timeout
o = s:option(Value, "port", translate("监听端口"))
o.datatype = "port"
o.placeholder = "8080"

o = s:option(Value, "session_timeout", translate("会话超时(秒)"))
o.datatype = "uinteger"
o.placeholder = "7200"

-- Auth switches
o = s:option(Flag, "enable_passwd", translate("启用账号密码认证"))
o.default = o.enabled

o = s:option(Flag, "enable_sms", translate("启用短信认证"))
o.default = o.enabled

o = s:option(Flag, "enable_wechat", translate("启用微信认证"))
o.default = o.enabled

-- Redirect
o = s:option(Value, "success_redirect", translate("登录成功跳转 URL"))
o.placeholder = "https://www.bing.com/"

-- Whitelist
o = s:option(Value, "ip_whitelist", translate("IP 白名单(逗号分隔)"))
o.placeholder = "192.168.16.119,192.168.16.168,192.168.16.248,192.168.16.128"

o = s:option(Value, "mac_whitelist", translate("MAC 白名单(逗号分隔)"))
o.placeholder = "70:4d:7b:64:3b:da,00:0c:29:16:9d:62,00:0c:29:7d:06:3e,74:cf:00:fe:1d:08"

return m