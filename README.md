下面是 **README.md 中文版**，结构清晰、专业，可直接用于 GitHub：

---

# 📘 **README.md — PortalD 认证系统 + LuCI 管理界面**

# PortalD – OpenWrt/MTK SDK 上的自助认证系统

# LuCI-App-PortalD – PortalD 的图形化配置界面

---

## 📌 项目简介

**PortalD** 是一个运行在 OpenWrt / MTK SDK 路由器上的轻量级、高性能 **门户认证服务（Captive Portal）**。
提供多种认证方式与灵活的白名单机制：

* 用户名/密码认证
* 短信认证（支持自定义脚本）
* 微信 OAuth 登录
* IP 白名单 / MAC 白名单
* 会话保持与超时清理
* 基于 SQLite 的认证记录
* 基于 iptables/ipset 的强制跳转

**LuCI-App-PortalD** 为 PortalD 提供一个图形化管理界面（LuCI），便于在 Web 中直接配置。

本仓库包含 **两个 OpenWrt package**，可直接用于：

* MTK SDK（如 MT7981、MT7986 开发板与商用路由器固件）
* 官方 OpenWrt 源码
* 各类定制固件二次开发项目

---

## 📁 仓库结构

```
├─ portald/
│  ├─ src/               # PortalD 后端源码（C）
│  ├─ files/             # 运行时文件（init.d、www、UCI 配置等）
│  ├─ Makefile           # PortalD 的 OpenWrt 编译脚本
│  └─ README.md
│
└─ luci-app-portald/
   ├─ luasrc/            # LuCI 控制器 + CBI 配置界面
   ├─ htdocs/            # 前端界面（HTML/CSS/JS）
   ├─ root/etc/config/   # 默认 /etc/config/portald
   ├─ Makefile           # LuCI package 编译脚本
   └─ README.md
```

---

## 🚀 功能特性

### 🔒 支持的认证方式

| 功能                          | 支持情况 |
| --------------------------- | ---- |
| 密码认证                        | ✔    |
| 短信认证                        | ✔    |
| 微信 OAuth 登录                 | ✔    |
| IP 白名单                      | ✔    |
| MAC 白名单                     | ✔    |
| session 超时管理                | ✔    |
| SQLite 数据库                  | ✔    |
| 使用 iptables + ipset 捕获未认证用户 | ✔    |

---

## 📦 安装方法

### 1️⃣ 克隆仓库

```bash
git clone https://github.com/yourname/portald-openwrt.git
cd portald-openwrt
```

### 2️⃣ 将本地 package 加入 OpenWrt/MTK SDK

编辑 SDK 根目录的：

```
feeds.conf.default
```

添加：

```
src-link local_portald /path/to/portald-openwrt
```

然后：

```bash
./scripts/feeds update local_portald
./scripts/feeds install portald luci-app-portald
```

---

## 🔧 编译方式

运行 menuconfig：

```bash
make menuconfig
```

勾选：

```
Network → Captive Portal → portald
LuCI → 3. Applications → luci-app-portald
```

开始编译：

```bash
make package/portald/compile V=s
make package/luci-app-portald/compile V=s
```

成功后会生成 `.ipk` 文件，可直接安装到路由器。

---

## 🗂 文件安装路径

PortalD 在设备上生成如下目录：

```
/etc/init.d/portald
/etc/config/portald
/etc/portald/config
/usr/sbin/portald
/usr/sbin/portal-fw.sh
/www/portal/        # Portal 登录页面
```

LuCI Web 页面地址：

```
http://路由器IP/cgi-bin/luci/admin/services/portald
```

---

## ⚙️ UCI 配置示例

`/etc/config/portald`：

```bash
config portald 'main'
    option port '8080'
    option session_timeout '7200'

    option enable_passwd '1'
    option enable_sms '0'
    option enable_wechat '1'

    option wx_appid 'your_wechat_appid'
    option wx_secret 'your_wechat_secret'
    option wx_redirect 'http://192.168.1.1:8080/wx/callback'

    option success_redirect 'https://www.bing.com'

    option db_path '/etc/portald/portal.db'
    option ip_whitelist '192.168.1.10,192.168.1.20'
    option mac_whitelist 'AA:BB:CC:DD:EE:FF'
```

---

## 🌐 Portal 前端页面说明

PortalD 的用户登录页面存放在：

```
/www/portal/
```

不会与 LuCI 或默认 `/www/index.html` 冲突，可并存。

用户首次访问 HTTP 网站将自动跳转到：

```
http://路由器IP:8080/portal/login.html
```

---

## 🔥 iptables / ipset 管理流程

PortalD 使用 `portal-fw.sh` 编排认证流程：

* 未认证 → 拦截 → 强制跳转到 Portal 网页
* 认证成功 → 加入 ipset → 放行所有流量
* 会话超时 → 重新加入认证队列

管理脚本位置：

```
/usr/sbin/portal-fw.sh
```

---

## 🧪 调试指南

查看日志：

```bash
logread -f
```

重启服务：

```bash
/etc/init.d/portald restart
```

查看 daemon：

```bash
ps | grep portald
```

查看数据库内容：

```bash
sqlite3 /etc/portald/portal.db
```

---

## 🤝 贡献指南

欢迎提交：

* Bug Report
* 新功能建议
* Pull Request

---

## 📄 License

MIT（可按你需求修改）

---

