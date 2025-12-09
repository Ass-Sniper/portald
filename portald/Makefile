include $(TOPDIR)/rules.mk

PKG_NAME:=portald
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

# GitHub Release 源码包
PKG_SOURCE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/Ass-Sniper/portald/archive/refs/tags/
PKG_HASH:=18a6f6cc222f4f7bbd49fc1f7cf3a5f0701d494de9311044a910c7125070464c

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=MIT
PKG_MAINTAINER:=Ass-Sniper

include $(INCLUDE_DIR)/package.mk

define Package/portald
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Captive Portals
  TITLE:=High-performance captive portal daemon (SQLite + ipset)
  DEPENDS:=+libsqlite3 +curl +ipset +kmod-ipt-ipset
endef

define Package/portald/description
  A high-performance captive portal daemon using SQLite and iptables/ipset.
  Supports password, SMS authentication and WeChat OAuth.
endef


#############################################################
# 解压 GitHub tar.gz → $(PKG_BUILD_DIR)
#############################################################
define Build/Prepare
	$(RM) -r $(PKG_BUILD_DIR)
	$(MKDIR) $(PKG_BUILD_DIR)
	# GitHub Release 解压后会包含 portald/
	$(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xzf $(DL_DIR)/$(PKG_SOURCE)
endef


#############################################################
# 编译 portald（从 src 目录构建）
#############################################################
define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -D_LARGEFILE64_SOURCE \
		-I$(STAGING_DIR)/usr/include \
		-o $(PKG_BUILD_DIR)/portald \
		$(PKG_BUILD_DIR)/portald/src/portald.c \
		$(PKG_BUILD_DIR)/portald/src/db.c \
		$(PKG_BUILD_DIR)/portald/src/util.c \
		$(PKG_BUILD_DIR)/portald/src/config.c \
		-lsqlite3
endef


#############################################################
# 安装所有 runtime 文件
#############################################################
define Package/portald/install

	# 主程序
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald $(1)/usr/sbin/portald

	# init.d 脚本
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald/files/etc/init.d/portald \
		$(1)/etc/init.d/portald

	# UCI 配置
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/portald/files/etc/config/portald \
		$(1)/etc/config/portald

	# Firewall 脚本
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald/files/usr/sbin/portal-fw.sh \
		$(1)/usr/sbin/portal-fw.sh

	# dnsmasq portal.conf
	$(INSTALL_DIR) $(1)/etc/dnsmasq.d
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/portald/files/etc/dnsmasq.d/portal.conf \
		$(1)/etc/dnsmasq.d/portal.conf

	# SMS & WeChat 使用的脚本
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald/files/usr/bin/send_sms.sh \
		$(1)/usr/bin/send_sms.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald/files/usr/bin/wx_oauth.sh \
		$(1)/usr/bin/wx_oauth.sh

	# 数据库目录
	$(INSTALL_DIR) $(1)/etc/portald

	# Web 登录页面（避免 /www/index.html 冲突，使用 /www/portal）
	$(INSTALL_DIR) $(1)/www/portal
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/portald/files/www/admin/index.html \
		$(1)/www/portal/index.html
endef


$(eval $(call BuildPackage,portald))
