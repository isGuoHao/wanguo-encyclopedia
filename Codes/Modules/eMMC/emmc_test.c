#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/mmc/core.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <drivers/mmc/core/bus.h>

#define EMMC_TEST_DTS_MMC_NODE_NAME       "emmc0"

/*
   内核后期device.bus变量修改为const struct bus_type *bus类型,
   在Makefile中设置CFLAGS屏蔽告警 
*/
struct bus_type *g_pst_mmc_bus_type = NULL;
struct mmc_card *g_pst_mmc_card = NULL;

static int EMMC_TEST_find_mmc_card(void)
{
    int iRet = -1;
    struct device       *pstDev = NULL;
    struct device_node  *pstDevNode = NULL;
    struct mmc_driver   fake_mmc_driver = {
        .drv = {
            .name = "fake_emmc_drv",
        },
    };

    /* 注册伪造的mmc驱动到mmc bus */
    iRet = mmc_register_driver(&fake_mmc_driver);
    if (!iRet)
    {
        pr_err("mmc_register_driver failed\n");
        return -ENODEV;
    }

    /* 获取到mmc_bus_type的地址 */
    g_pst_mmc_bus_type = fake_mmc_driver.drv.bus;

    /* 查找emmc0节点 */
    pstDevNode = of_find_node_by_name(NULL, EMMC_TEST_DTS_MMC_NODE_NAME);
    if (!pstDevNode)
    {
        pr_err("of_find_node_by_name %s failed\n", EMMC_TEST_DTS_MMC_NODE_NAME);
        return -ENODEV;
    }

    /* 在mmc_bus_type上查找设备地址 */
    pstDev = bus_find_device_by_of_node(g_pst_mmc_bus_type, pstDevNode);
    if (!pstDev)
    {
        pr_err("bus_find_device_by_of_node failed\n");
        return -ENODEV;
    }

    /* 根据device地址获取到mmc_card地址 */
    g_pst_mmc_card = container_of(pstDev, struct mmc_card, dev);
    if (!g_pst_mmc_card)
    {
        pr_err("find spi_device failed\n");
        return -ENODEV;
    }

    /* 卸载伪造的mmc驱动 */
    mmc_unregister_driver(&fake_mmc_driver);
    return 0;
}


static int EMMC_TEST_module_init(void)
{
    int iRet = -1;
    pr_info("SPI_TEST_module_init\n");

    iRet = EMMC_TEST_find_mmc_card();
    if (!iRet)
    {
        pr_err("EMMC_TEST_find_mmc_card failed\n");
        return -ENODEV;
    }

    return 0;
}

static void EMMC_TEST_module_exit(void)
{
    pr_info("EMMC_TEST_module_exit\n");
    return;
}

module_init(EMMC_TEST_module_init);
module_exit(EMMC_TEST_module_exit);

MODULE_AUTHOR("GuoHao");
MODULE_DESCRIPTION("eMMC Driver Test Module");
MODULE_LICENSE("GPL");
