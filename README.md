# AGPIO Switch - Kernel Module for disabling MT7628 Ethernet PHY[1,2,3,4] Switch

MT7628, an SoC used in routers and MecMeter, has 5-ports Ethernet switch.
These are enabled by default.

This module disables
* the Ethernet switch by modifying `AGPIO_CFG` resistor value.
* LEDs for each port by modifying `GPIO2_CFG` resistor value.

Tested on OpenWrt 23, with kernel version 5.15

## Usage

### Use a custom package collection.

[TowerWrt Packages (OpenWrt Custom Packages Collection)](https://github.com/dumtux/towerwrt-packages)

### (alternative way) Hard-copying

* Clone this into your customizing OpenWRT root, e.g. into `/package/somewhere/agpioSwitch`
* comment out node sections in device tree that are occupying Ethernet ports 1, 2, 3 and 4. enable gpiostate_defaults for those pins
* Run `make menuconfig` and search `agpio` to see if the `kmod-agpioSwitch` is present, then select it.
* After building, flashing, and booting the built OpenWrt image, run `dmesg | grep -i agpio` to see if it's applied correctly.

```
root@Openwrt:~# dmesg | grep -i agpio
[   60.140149] AGPIOSwitch: Disabling MT7628 PHY ports 1,2,3 and 4. enable gpio mode for PHY LEDs...
root@Openwrt:~#
```
