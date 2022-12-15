==============
客户移植指导
==============

系统需求
----------

 - x86 或 AMD64 系统 PC 机
 - 基于 Linux 的系统，推荐 ubuntu 18.04 或更高
 - GNU Make 4 或更高
 - GNU GCC9 工具链或更高
 - ARM GCC9 工具链或更高
 - Python3 或更高
 - Open office 或其他兼容 Microsoft office excel 格式的软件
 - libopencm3, 主页 https://libopencm3.org/
 - stlink v1.5.1, 主页 https://github.com/stlink-org/stlink

    * BM1684xEVB MCU型号:STM32L071C8

    * 工具链:gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux

代码结构
----------

顶层代码结构如表格所示：

=========== ===========================
目录           用途
=========== ===========================
BM1684xEVB      BM1684xEVB固件源码
----------- ---------------------------
tools           编译、打包工具集合
----------- ---------------------------
upgrader        基于 I2C 协议的升级器源码
----------- ---------------------------
build           构建系统目录
----------- ---------------------------
libopencm3      libopencm3 源码和构建后的库文件
=========== ===========================

编译方式
----------

在linux shell控制台执行

.. code-block:: shell

    #选择
    #pwd=../mcu
     make -C build PROJ=bm1684xevb
    #pwd=../mcu/build
     make PROJ=bm1684xevb
    #pwd=../mcu/bm1684xevb 文件生成于该目录build下
     make

关键代码位置
--------------

system.c:

- uart、i2c、adc、gpio、clock、timer等初始化调用
- 自写print函数

power.c:

- 上下电函数实现
- 通过由config.xlsx自动生成的节点进行上下电
- power_init 初始化电源框架
- power_on 系统上电
- power_off 系统下电

i2c_master.c:

- i2c设备初始化函数实现
- 使MCU 能够发出满足 I2C 协议的传输, 包括单个byte、word的传输
- 最主要是与PMIC电源管理芯片MP5475通信,配置输出电压
- 温度传感器tmp451温度信息获取以及配置设备
- gpio拓展板tca6416通过I2C协议透传
- i2c_master_init 将 I2C 控制器初始化为主模式,主要是时钟配置 100kHz
- i2c_master_destroy 关闭i2c
- i2c_master_trans --->  i2c_transfer7_timeout 符合i2c协议,任意字节进行读写
- i2c_master_smbus_read_byte 等相关函数实现

i2c_slave.c:

- 将i2c作为从设备的配置
- 采用了中断响应,异步工作的模式,任何需要使用mcu从设备的模块,都需要注册回调函数
- 包含i2c作为从设备时的多地址设置

chip.c:

- chip_init 芯片框架初始化
- chip_enable 解复位芯片
- chip_disable 复位芯片
- chip_reset 通过复位-解复位操作重置芯片
- chip_popd_reset_early + chip_popd_reset_end 通过下电-上电操作重置芯片
- chip_uptime 返回芯片自解复位开始的时间，以秒计
- chip_reset_times 返回芯片自系统电源通电算起的复位次数

loop.c:

- loop_add 在主循环框架中增加一个入口
- loop_start 启动主循环，此函数从不返回

tick.c:

- tick_init 初始化 STM32 的 sys timer,超时事件 1 毫秒
- mdelay 利用 sys timer 完成 1 毫秒延迟
- tick_get 获取当前 sys timer 驱动的计数器值
- tick_register_task 生成一个计数器实例
- tick_set_task_interval 修改一个计数器的超时时间

timer.c:

- timer_setup 设置一个计数器，此计数器使用 STM32 中的 TIMERx,不使用 sys timer
- timer_counter 获取当前计数器值
- timer_stop 停止计数器
- timer_test  测试计数器时间是否准确
- timer_mdelay 利用 STM32 的 TIMERx 完成毫秒级延迟
- timer_udelay 利用 STM32 的 TIMERx 完成微秒级延迟

i2c-slaves文件夹下

- 包含着MCU 模拟的各种从设备
- 实现PC通过smbus到从设备的信息透传

移植通常需要修改的部分
-------------------------

修改上电时序
~~~~~~~~~~~~~~~

在config.xlsx的第二个sheet中进行更改,更改之后在BM1684xEVB目录下执行

.. code-block:: shell

    make code

自动生成board_power.c等文件,如若有Function类型的节点,还需要对该功能类型节点函数进行定义

PMIC电压输出修改
~~~~~~~~~~~~~~~~~

如需对多项电源MP5475输出电压进行修改,在mp5475.c的mp5475_init()函数中

修改mp5475_voltage_config(0, xxxx)传入的第二个参数即可。

硬件型号以及产品型号更改
~~~~~~~~~~~~~~~~~~~~~~~~~~

mon.c 中通过AD采样得到整版12V电压通道的电流值、硬件版本号、产品版本号和ntc板温。
其中硬件型号以及产品型号是根据adc采样区分不同阻值的采样电阻,从而区分不同型号。ad管脚端能够采集到的电压最大值
等于MCU的供电电压,一般是3.3V或1.8V。我们采用的是3.3V供电,因此ad采样电压阈值限制在0~3.3V区间,当模拟信号为3.3V时,
转化成12bit数字信号,正好每一位都是 1。硬件上通过使用不同的采样电阻使得adc管脚分到不同电压,不同板卡类型
焊接不同的采样电阻。adc采样得到的ad值 = (分到的电压 / 3.3V) * 2^12 。

客户移植后若希望改变硬件型号或者产品型号, 可以通过以下方式：

- 更换硬件上的采样电阻,改变ad传感器分到的电压
- 更改软件上version_table[]区间分割值,重新设计一套自己的分类表
- 若沿用了硬件上的设计以及软件上区间的划分,还可以通过更改adc_ver()函数返回值
- 若单纯只为从SMBUS读取到想要的值, 直接在mcu.c中强制修改寄存器值。

tca6416寄存器透传
~~~~~~~~~~~~~~~~~~~~

目前BM1684xEVB固件中只能写out寄存器,而in, config, pol寄存器无法写,四类寄存器对读操作开放

通常客户移植后想要对所有寄存器进行操作,需要在tca6416a_i2c_slave_write()函数中进行透传的开放。

需要谨慎修改的部分
--------------------

上电时序的sys_rst
~~~~~~~~~~~~~~~~~~~~~~~

BM1684xEVBMCU 上电过程的末程将sys_rst拉高30ms后拉低,又30ms后检测板卡运行模式,如果为soc,主动将sys_rst拉高
如果为PCIe模式,等到获取到p_rst信号后,将sys_rst拉高。

修改时不建议去除sys_rst这样的操作逻辑。

mp5475输出电压修改注意
~~~~~~~~~~~~~~~~~~~~~~

修改前注意与硬件确认修改的通道以及修改后电压,否则若有修改问题可能导致某些模块上电出现问题,
间接导致主芯片工作不正常







