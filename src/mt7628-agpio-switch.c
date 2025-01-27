/* 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kmod.h>
#include <linux/io.h>

#define DRV_NAME "AGPIOSwitch"

#define CHIPID0_3_LOC   0x10000000 //Location of CPU ID register (low word)
#define CHIPID4_7_LOC   0x10000004 //Location of CPU ID register (high word)
#define AGPIO_CFG_LOC   0x1000003C //Location of APGIO_CFG register on mt7628
#define GPIO1_MODE_LOC  0x10000060 //Location of GPIO1_MODE register on mt7628
#define GPIO2_MODE_LOC  0x10000064 //Location of GPIO2_MODE register on mt7628

#define CHIPID0_3_MAGIC 0x3637544D //ASCII value stored in CPU ID register MSB from datasheet (MT7628  )
#define CHIPID4_7_MAGIC 0x20203832

typedef volatile uint32_t*  mt7628_t;

mt7628_t CHIPID0_3;
mt7628_t CHIPID4_7;
mt7628_t AGPIO_CFG;
mt7628_t GPIO1_MODE;
mt7628_t GPIO2_MODE;

uint32_t backup_AGPIO_CFG;
uint32_t backup_GPIO1_MODE;
uint32_t backup_GPIO2_MODE;

static int is_mt7628(void){
    //Check the IO registers are equal to the magic values written in the datasheet for the mt7628
    return (*CHIPID0_3 == CHIPID0_3_MAGIC) && (*CHIPID4_7 == CHIPID4_7_MAGIC);
}

static int __init agpio_init(void)
{
    //Map CPU IO registers
    CHIPID0_3 = (mt7628_t)ioremap(CHIPID0_3_LOC, 4);
    CHIPID4_7 = (mt7628_t)ioremap(CHIPID4_7_LOC, 4);
    AGPIO_CFG = (mt7628_t)ioremap(AGPIO_CFG_LOC, 4);
    GPIO2_MODE = (mt7628_t)ioremap(GPIO2_MODE_LOC, 4);
    GPIO1_MODE = (mt7628_t)ioremap(GPIO1_MODE_LOC, 4);

    if(is_mt7628()){
        printk("AGPIOSwitch: Disabling MT7628 PHY ports 1,2,3 and 4. enable gpio mode for PHY LEDs...\n");

        backup_AGPIO_CFG = *AGPIO_CFG;
        backup_GPIO1_MODE = *GPIO1_MODE;
	backup_GPIO2_MODE = *GPIO2_MODE;
        *AGPIO_CFG |= (0xF << 17); //Set the EPHY_GPIO_AIO_EN bits of the CFG register to enable Digital PAD PHY mode
        *AGPIO_CFG |= 0xF;         //set the i2s pads to digital PAD

        // For enabling gpio on the interface pins for pwm0/1, uart2, i2c, refclk, i2s, spis, gpio_mode
        *GPIO1_MODE |= 0b01010100000101000000000001000101; //0x54140045

        // for enabling GPIO on EPHY_40_LED, EPHY_P3_LED, EPHY_P2_LED, EPHY_P1_LED and WLAN_LED
        *GPIO2_MODE |= 0b010101010001; //0x551
    }
    else{
        printk("AGPIOSwitch: Not running on mt7628, module will not do anything\n");
    }
	return 0;
}

static void __exit agpio_exit(void)
{
    if(is_mt7628()){
        printk("AGPIOSwitch: Reverting AGPIO_CFG\n");
        *AGPIO_CFG = backup_AGPIO_CFG;
        *GPIO2_MODE = backup_GPIO2_MODE;
	*GPIO1_MODE = backup_GPIO1_MODE;
    }
    iounmap(CHIPID0_3);
    iounmap(CHIPID4_7);
    iounmap(AGPIO_CFG);
    iounmap(GPIO2_MODE);
    iounmap(GPIO1_MODE);

    printk("AGPIOSwitch: module unloaded\n");
}

module_init(agpio_init);
module_exit(agpio_exit);

MODULE_AUTHOR("David Kitschker, Hotte S.");
MODULE_DESCRIPTION("Modify mt7628 AGPIO_CFG register to enable 5 port PHY");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);
