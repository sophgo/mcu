========================
串口使用演示
========================

烧录器状态打印：
----------------

烧录器上电后，串口出现打印如下：

.. code-block:: shell

    Hello WOLFCLAW User
    firmware build time:xx-xx

若未插入SD卡，串口每秒重复打印：

.. code-block:: shell

    please insert sdcard

插入SD卡后：

.. code-block:: shell

    /* 挂载与初始化成功 */
    SDcard inserted
    In SDcard mode...
    SDcard init success!
    SDcard mount success!
    /* 挂载或初始化失败 */
    SDcard init failed!
    SDcard mount failed!

按下按键成功烧录时：

.. code-block:: shell

    infosize=配置文件字节数
    programming...1%-->100%
    burn successfully!

烧录器串口命令功能实现：
----------------

键入"help"

.. code-block:: shell

    # help
    mcuinfo
        get mcu's info
    deviceinfo
        get device's info
    led
        led [led_num] [led_status]
        control led on/off
    addr
        addr <the address>
        set the address as default address
    device
        device {ISL68224} 'or' {ISL68127}
        set the device as default device
    filename
        filename <0:otherfilename.hex>
        set otherfilename as default filename
        default filename = '0:firmware.hex' at first
    burn
        burn from sd card
    flashburn
        flashburn <file address in flash>
        burn from flash

串口命令提供以上诸多功能。
