#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/spi/spi.h>

#define SPI_TEST_DTS_SPIDEV_NODE_NAME       "spidev"
#define SPI_TEST_TEST_REG_ADDR              (0x02)
#define SPI_TEST_READ_FLAG                  (0x0B)
#define SPI_TEST_WRITE_FLAG                 (0x02)

struct spi_device *g_pstSpiDev = NULL;


static SPI_TEST_find_spi_device(void)
{
    struct device       *pstDev = NULL;
    struct device_node  *pstDevNode = NULL;

    pstDevNode = of_find_node_by_name(NULL, SPI_TEST_DTS_SPIDEV_NODE_NAME);
    if (!pstDevNode)
    {
        pr_err("of_find_node_by_name %s failed\n", SPI_TEST_DTS_SPIDEV_NODE_NAME);
        return -ENODEV;
    }

    pstDev = bus_find_device_by_of_node(&spi_bus_type, pstDevNode);
    if (!pstDevNode)
    {
        pr_err("bus_find_device_by_of_node failed\n");
        return -ENODEV;
    }

    g_pstSpiDev = container_of(pstDev, struct spi_device, dev);
    if (!pstDevNode)
    {
        pr_err("find spi_device failed\n");
        return -ENODEV;
    }

    return 0;
}


static SPI_TEST_read(unsigned char ucAddr, unsigned char *pucData)
{
    int iRet = -1;
    unsigned char tx_buf[3];
    struct spi_transfer xfers[3];
    struct spi_device *spi = NULL;
    struct spi_message msg;

    spi = spi_dev_get(g_pstSpiDev);
    if (!spi)
    {
        pr_err("spi_dev_get failed\n");
        return -ENODEV;
    }

    spi_message_init(&msg);
    memset(xfers, 0, sizeof(xfers));
    memset(tx_buf, 0, sizeof(tx_buf));

    tx_buf[0] = SPI_TEST_READ_FLAG;
    xfers[0].tx_buf = &tx_buf[0];
    xfers[0].len = 1;
    xfers[0].speed_hz = 500000;
    spi_message_add_tail(&xfers[0], &msg);

    tx_buf[1] = ucAddr;
    xfers[1].tx_buf = &tx_buf[1];
    xfers[1].len = 1;
    xfers[1].speed_hz = 500000;
    spi_message_add_tail(&xfers[1], &msg);


    xfers[2].rx_buf = pucData;
    xfers[2].len = 1;
    xfers[2].speed_hz = 500000;
    spi_message_add_tail(&xfers[2], &msg);

    iRet = spi_sync(spi, &msg);
    if (!iRet)
    {
        pr_err("spi_sync failed, iRet = %d\n", iRet);
        spi_dev_put(spi);
        return iRet;
    }

    spi_dev_put(spi);
    return 0;
}


static SPI_TEST_write(unsigned char ucAddr, unsigned char ucData)
{
    int iRet = -1;
    unsigned char tx_buf[3];
    struct spi_transfer xfers[3];
    struct spi_device *spi = NULL;
    struct spi_message msg;

    spi = spi_dev_get(g_pstSpiDev);
    if (!spi)
    {
        pr_err("spi_dev_get failed\n");
        return -ENODEV;
    }

    spi_message_init(&msg);
    memset(xfers, 0, sizeof(xfers));
    memset(tx_buf, 0, sizeof(tx_buf));

    tx_buf[0] = SPI_TEST_WRITE_FLAG;
    xfers[0].tx_buf = &tx_buf[0];
    xfers[0].len = 1;
    xfers[0].speed_hz = 500000;
    spi_message_add_tail(&xfers[0], &msg);

    tx_buf[1] = ucAddr;
    xfers[1].tx_buf = &tx_buf[1];
    xfers[1].len = 1;
    xfers[1].speed_hz = 500000;
    spi_message_add_tail(&xfers[1], &msg);

    tx_buf[2] = ucData;
    xfers[2].rx_buf = &tx_buf[2];
    xfers[2].len = 1;
    xfers[2].speed_hz = 500000;
    spi_message_add_tail(&xfers[2], &msg);

    iRet = spi_sync(spi, &msg);
    if (!iRet)
    {
        pr_err("spi_sync failed, iRet = %d\n", iRet);
        spi_dev_put(spi);
        return iRet;
    }

    spi_dev_put(spi);
    return 0;
}


static int SPI_TEST_module_init(void)
{
    pr_err("SPI_TEST_module_init\n");

    return 0;
}

static void SPI_TEST_module_exit(void)
{
    pr_err("SPI_TEST_module_exit\n");
    return;
}

module_init(SPI_TEST_module_init);
module_exit(SPI_TEST_module_exit);

MODULE_AUTHOR("GuoHao");
MODULE_DESCRIPTION("Spi Driver Test Module");
MODULE_LICENSE("GPL");
