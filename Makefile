#Kernel module mt7688-agpio-switch

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt7628-agpio-switch
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk


MAKE_OPTS:=$(KERNEL_MAKE_FLAGS) \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	CONFIG_AGPIO=m

define KernelPackage/mt7628-agpio-switch
	SECTION:=kernel
	CATEGORY:=TowerSoftware
	SUBMENU:=Kernel modules
	TITLE:=Switch AGPIO_CFG to disable PHY[1,2,3,4] on MecMeter
	FILES:=$(PKG_BUILD_DIR)/mt7628-agpio-switch.ko
	AUTOLOAD:=$(call AutoLoad,81,mt7628-agpio-switch)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) -R ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,mt7628-agpio-switch))
