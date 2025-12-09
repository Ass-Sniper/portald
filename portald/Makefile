include $(TOPDIR)/rules.mk

PKG_NAME:=portald
PKG_RELEASE:=1

#############################################################
# 禁用下载和校验
PKG_SOURCE:=
PKG_SOURCE_URL:=
PKG_HASH:=skip

# ❗禁用自动解包（必须为空，而不是 true）
PKG_UNPACK:=0
PKG_BUILD_PARALLEL:=0
#############################################################

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_INSTALL:=0   # <—— 禁用默认 prepare/install，避免执行 PKG_BUILD_DIR 

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk  # 阻止默认 extract 函数

# ------- 关键：覆盖 MTK 的 Build/Unpack （否则目录被当成程序执行） -------
define Build/Unpack
	# nothing
endef
# -------------------------------------------------------------------------

define Package/portald
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Captive Portals
  TITLE:=High-performance captive portal daemon (iptables version)
  DEPENDS:=+libsqlite3 +curl +ipset +kmod-ipt-ipset
endef

define Package/portald/description
 A high-performance captive portal daemon using SQLite + ipset/iptables,
 supporting password, SMS and WeChat authentication.
endef


#############################################################
# 准备构建目录 —— 必须，否则会出现“是目录，无法执行”错误
#############################################################
define Build/Prepare
	$(RM) -r $(PKG_BUILD_DIR)
	$(MKDIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef


#############################################################
# 编译主程序
#############################################################
# 加上 curl 库，否则编译链接会失败
TARGET_LDFLAGS += -lsqlite3 -lcurl

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) \
		-I$(STAGING_DIR)/usr/include \
		$(PKG_BUILD_DIR)/portald.c \
		$(PKG_BUILD_DIR)/db.c \
		$(PKG_BUILD_DIR)/util.c \
		$(PKG_BUILD_DIR)/config.c \
		-o $(PKG_BUILD_DIR)/portald \
		$(TARGET_LDFLAGS)
endef


#############################################################
# 安装阶段
#############################################################
define Package/portald/install
	# 主程序
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/portald $(1)/usr/sbin/portald

	# init 脚本
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/portald $(1)/etc/init.d/portald

	# UCI 默认配置
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/portald $(1)/etc/config/portald

	# iptables 规则脚本
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/usr/sbin/portal-fw.sh $(1)/usr/sbin/portal-fw.sh

	# dnsmasq Portal 配置
	$(INSTALL_DIR) $(1)/etc/dnsmasq.d
	$(INSTALL_CONF) ./files/etc/dnsmasq.d/portal.conf $(1)/etc/dnsmasq.d/portal.conf

	# 外部认证脚本
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/usr/bin/send_sms.sh $(1)/usr/bin/send_sms.sh
	$(INSTALL_BIN) ./files/usr/bin/wx_oauth.sh $(1)/usr/bin/wx_oauth.sh

	# 数据库目录
	$(INSTALL_DIR) $(1)/etc/portald

	# Web 管理界面
	$(INSTALL_DIR) $(1)/www/portal
	$(INSTALL_DATA) ./files/www/admin/index.html $(1)/www/portal/index.html
endef

$(eval $(call BuildPackage,portald))
