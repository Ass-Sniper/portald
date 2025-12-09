include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-portald
PKG_VERSION:=1.0
PKG_RELEASE:=1

LUCI_TITLE:=LuCI interface for portald
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+portald

include $(TOPDIR)/feeds/luci/luci.mk

# 必须最后一句
# call BuildPackage is already inside luci.mk
