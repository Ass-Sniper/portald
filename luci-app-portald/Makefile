include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-portald
PKG_VERSION:=1.0.2
PKG_RELEASE:=1

PKG_SOURCE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/Ass-Sniper/portald/archive/refs/tags
PKG_HASH:=skip

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

LUCI_TITLE:=LuCI interface for portald
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+portald

include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/luci/luci.mk


########################################################
# 解包
########################################################
define Build/Prepare
	$(RM) -r $(PKG_BUILD_DIR)
	$(MKDIR) $(PKG_BUILD_DIR)
	$(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xzf $(DL_DIR)/$(PKG_SOURCE)
endef


########################################################
# 安装 LuCI 页面
########################################################
define Package/luci-app-portald/install
	# Controller
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/luci-app-portald/luasrc/controller/portald.lua \
		$(1)/usr/lib/lua/luci/controller/

	# CBI model
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/portald
	$(CP) $(PKG_BUILD_DIR)/luci-app-portald/luasrc/model/cbi/portald/* \
		$(1)/usr/lib/lua/luci/model/cbi/portald/

	# Views
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/portald
	$(CP) $(PKG_BUILD_DIR)/luci-app-portald/luasrc/view/portald/* \
		$(1)/usr/lib/lua/luci/view/portald/
endef


$(eval $(call BuildPackage,luci-app-portald))
