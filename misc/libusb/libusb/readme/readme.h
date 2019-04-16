/*
 *
 *
 *
 *
 *
 *
 1 libusb_init
函数原型：int libusb_init(libusb_context **ctx);
功能说明：该函数进行libusb的初始化，必须最先调用。
参数说明：ctx通常设置NULL
返回值：0成功，非0 失败


2 libusb_exit
函数原型：void  libusb_exit(libusb_context *ctx);
功能说明：和libusb_init成对使用，释放相应的资源。
参数说明：ctx通常设置NULL


3 libusb_has_capability
函数原型：int libusb_has_capability(uint32_t capability);
功能说明：判断当前的库是否支持某项功能
参数说明：capability的取值范围在 enum libusb_capability中定义。
          LIBUSB_CAP_HAS_CAPABILITY libus库的        API是否有效，该项通常总是返回1
          LIBUSB_CAP_HAS_HOTPLUG    是否支持热插拔
          LIBUSB_CAP_HAS_HID_ACCESS 是否支持访问HID设备，而不需要用户干预
          LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER 是否支持在内核中释放默认的驱动也就是可以调用libusb_detach_kernel_driver来释放内核驱动
返回值：非0 支持 ，0 不支持


4 libusb_hotplug_register_callback
函数原型：int LIBUSB_CALL libusb_hotplug_register_callback(libusb_context *ctx,        libusb_hotplug_event events,libusb_hotplug_flag flags,        int vendor_id, int product_id,
                                                int dev_class,        libusb_hotplug_callback_fn cb_fn,        void *user_data,libusb_hotplug_callback_handle *handle);
功能说明：注册回调函数，响应热插拔事件。
参数说明：ctx 通常为NULL
          events 要响应的事件，参数为LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED设备插入事件 LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT 设备拔出事件，也可以LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT表示同时响应插拔事件
          flags 如果为0，只有发生插拔的时候才会调用注册的回调函数，如果为LIBUSB_HOTPLUG_ENUMERATE，则在初始化前设备已经插入，也会调用注册的毁掉函数。
          vendor_id 需要监控的VID，只有指定的VID的设备插拔，才会调用回调函数 。设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断VID
          product_id 需要监控的PID，如果设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断PID
          dev_class  需要监控的设备class,如果设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断class。注意这里的class是与libusb_device_descriptor的class匹配，而不是 libusb_interface_descriptor的class
          cb_fn 回调函数的指针  回调函数的定义为int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
          user_data 用户数据的指针 对应回调函数中的user_data
          handle    句柄
返回值：0成功，非0 失败
注意：不要在回调函数中调用可能阻塞的操作，否则可能造成libusb的其他函数执行失败，不要在回调函数中调用libusb_claim_interface等操作，有可能会失败


5 libusb_hotplug_deregister_callback
函数原型：void LIBUSB_CALL libusb_hotplug_deregister_callback(libusb_context *ctx,libusb_hotplug_callback_handle handle);
参数说明：注销libusb_hotplug_register_callback函数注册的回调函数
参数说明：ctx 通常为NULL
          handle   libusb_hotplug_register_callback返回的句柄
返回值：无


6 libusb_handle_events
函数原型： int LIBUSB_CALL libusb_handle_events(libusb_context *ctx);
功能说明：在阻塞模式中处理任何挂起的事件。
参数说明：ctx 通常为NULL
返回值：0成功，非0 失败
注意：1调用libusb_handle_events的线程，不要执行阻塞的操作，否则可能造成libusb其他函数执行失败
2 如果注册了热插拔事件，必须要在循环中调用这个函数 例如下面的代码如论怎么插拔USB设备，都不会打印“device insert”
static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
        printf("device insert  \n");
}
int main(int argc, char **argv)
{
        libusb_hotplug_callback_handle hp;
        libusb_init (NULL);
        libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY,
                LIBUSB_HOTPLUG_MATCH_ANY, 0, hotplug_callback, NULL, &hp);
        while(1);
        libusb_hotplug_deregister_callback(hp);
}
必须要改为下面的代码,插拔USB才会有“device insert” 的信息
static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
        printf("device insert  \n");
}
int main(int argc, char **argv)
{
        libusb_hotplug_callback_handle hp;
        libusb_init (NULL);
        libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY,
                LIBUSB_HOTPLUG_MATCH_ANY, 0, hotplug_callback, NULL, &hp);
        while(1)
        {
                libusb_handle_events(NULL);
        }
        libusb_hotplug_deregister_callback(hp);
}

7   libusb_open_device_with_vid_pid
函数原型：libusb_device_handle * LIBUSB_CALL libusb_open_device_with_vid_pid(        libusb_context *ctx, uint16_t vendor_id, uint16_t product_id);
函数功能：通过VID和PID打开一个USB 设备，并返回设备句柄libusb_device_handle的指针
参数说明：ctx 通常为NULL
          vendor_id 设备的VID
          product_id 设备的PID
返回值：成功返回libusb_device_handle的指针 ，失败返回NULL


8 libusb_open
函数原型：int LIBUSB_CALL libusb_open(libusb_device *dev, libusb_device_handle **handle);
函数功能：通过libusb_device的指针打开一个USB设备，并返回设备句柄libusb_device_handle的指针
参数说明：dev libusb_device的指针
          handle 用来返回设备句柄libusb_device_handle的指针
返回值：0成功，非0 失败


9 libusb_close
函数原型：void LIBUSB_CALL libusb_close(libusb_device_handle *dev_handle);
函数功能：关闭 libusb_open或者libusb_open_device_with_vid_pid打开的设备
参数说明：dev_handle  调用libusb_open或者libusb_open_device_with_vid_pid返回的设备句柄libusb_device_handle的指针
返回值： 无


10 libusb_get_device_list
函数原型：ssize_t LIBUSB_CALL libusb_get_device_list(libusb_context *ctx,        libusb_device ***list);
函数功能：获取当前的设备列表
函数功能：ctx 通常为NULL
          list USB设备列表
返回值：0成功，非0 失败


11 libusb_free_device_list
函数原型：void LIBUSB_CALL libusb_free_device_list(libusb_device **list,        int unref_devices);
函数功能：释放以前使用的设备列表
参数说明：list 要释放的设备列表的指针
          unref_devices 如果该参数置1 列表中的每个设备的引用计数减1
返回值：无
注：下面是示例代码：
        libusb_device **devs;
        ssize_t cnt;
        int i;
        libusb_init (NULL);
        cnt = libusb_get_device_list(NULL, &devs);
        for(i=0;i<cnt;i++)
        {
                PrintUsbDec(devs[i]);
        }
        libusb_free_device_list(devs, 1);

12 libusb_get_device_descriptor
函数原型：int LIBUSB_CALL libusb_get_device_descriptor(libusb_device *dev,        struct libusb_device_descriptor *desc);
函数功能：获取USB设备的设备描述符
参数说明：dev libusb_device的指针，是要读取的设备
                  desc 设备描述符的指针，用来带回设备描述符的结构
返回值：0成功，非0 失败
注意：这个函数是将设备描述符的结构拷贝到desc指向的地址
所以,下面的用法是错误的
struct libusb_device_descriptor *desc;
libusb_get_device_descriptor(dev,desc);
正确的写法是
struct libusb_device_descriptor desc;
libusb_get_device_descriptor(dev,&desc);


13 libusb_get_config_descriptor
函数原型：int LIBUSB_CALL libusb_get_config_descriptor(libusb_device *dev,        uint8_t config_index, struct libusb_config_descriptor **config);
函数功能：获取指定设备的配置描述符
参数说明：dev libusb_device的指针，是要读取的设备
          config_index 配置描述符的索引（一个USB设备可能有多个配置）
          config 配置描述符的指针，用来带回设备描述符
返回值：0成功，非0 失败


14 libusb_free_config_descriptor
函数原型：void LIBUSB_CALL libusb_free_config_descriptor(        struct libusb_config_descriptor *config);
函数功能：释放配置描述符
参数说明：config 要释放的配置描述符
返回值：无
注意： 用libusb_get_config_descriptor获取配置描述符后必须要调用        libusb_free_config_descriptor释放，否则会造成内存泄漏。


15  libusb_control_transfer
函数原型：int LIBUSB_CALL libusb_control_transfer(libusb_device_handle *dev_handle,        uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
              unsigned char *data, uint16_t wLength, unsigned int timeout);
参数说明：dev_handle libusb_device_handle的指针
                  request_type      D7=0主机到设备， =1设备到主机；
                            D6D5 =00标准请求命令， 01 类请求命令，10用户定义的命令，11保留值
                            D4D3D2D1D0= 0表示接收者为设备，1表示接收者为接口，2表示接收者为端点，3表示接收者为其他，其他值保留
                            这个参数由 enum libusb_request_recipient 、enum libusb_request_type 、enum libusb_endpoint_direction 组合
          bRequest      命令的序号(其实就是命令)；所有的命令都是以不同编码值的方式传递给设备的，bRequest就表示USB命令的编码值。可以是USB标准命令，也可以用户自定义命令
                        标准请求命令定义在 enum libusb_standard_request
          Value         2个字节，用来传送当前请求的参数，随请求不同而变。
          Index    索引字段同样是2个字节，描述的是接口号
          data     要传输的数据的指针，不需要传输数据，设置为NULL
          wLength    数据的长度。当命令不需要传输数据时，此字段设为0
          timeout  设置超时的毫秒数，如果设置0，则永不超时
注：USB标准请求命令：
1.获取状态 Get Status (00H)
    A:[To Device]获取设备的状态:
   *.位0:自供电(0表示总线供电;1表示自供电).
   *.位1:远程唤醒(0表示不支持远程唤醒;1表示远程唤醒).
   *.位2~15:保留.
   *.一般选择总线供电,不支持远程唤醒,所以返回数据就是0x0000.
   B:[To Interface]获取接口的状态:
   *.接口状态的16位字节全部保留,所以返回数据就是0x0000.
   C:[To Endpoint]获取端点的状态:
   *.位0:Halt(0表示端点允许;1表示端点禁止).
   *.位1~15:保留(复位为0).
2.清除特性 Clear Feature (01H)
   A:[To Device]清除设备的远程唤醒功能,并返回一个空包.
   B:[To Endpoint]解禁端点.
3.设置特性 Set Feature (03H)
   A:[To Device]设置设备的远程唤醒功能,并返回一个空包.
   B:[To Endpoint]禁止端点.
4.设置地址 Set Address (05H)
   A:设置设备地址.
5.获取描述符 Get Descriptor (06H)
   A:[To Device]获取设备描述符:
   *.描述当前USB协议的版本号.设备端点0的FIFO大小.USB设备的ID号等.
   B:[To Configuration]获取配置描述符:
   *.描述USB设备接口个数及是否有自供电能力等.
   C:[To Interface]获取接口描述符:
   *.描述端点0以外的物理端点个数等信息.
   D:[To Endpoint]获取端点描述符:
   *.描述端点0各端点的传输类型和最大信息包大小和端点的传输方向(IN/OUT).
6.设置描述符(可选,无法更新) Set Descriptor (07H)
7.获取配置信息 Get Configuration (08H)
8.设置配置 Set Configuration (09H)
A:[To Configuration]设置配置描述符.
B:[To Interface]设置接口描述符.
C:[To Endpoint]设置端点描述符.
9.获取接口信息 Get Interface (0AH)
10.设置接口 Set Interface (0BH)
11.SYNCH_FRAME(0CH)
用于设备设置和报告一个端点的同步帧.


16 libusb_kernel_driver_active
函数原型：int LIBUSB_CALL libusb_kernel_driver_active(libusb_device_handle *dev,int interface_number);
函数功能：确定指定接口的内核驱动程序是否已经激活。如果一个内核驱动程序是激活的，libusb_claim_interface调用的会失败
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  interface_number 接口号，这个对应接口描述符的 bInterfaceNumber
返回值：1 已经激活，非1 没有激活


17 libusb_detach_kernel_driver
函数原型：int LIBUSB_CALL libusb_detach_kernel_driver(libusb_device_handle *dev,int interface_number);
函数功能：卸载指定接口的内核驱动程序。如果一个内核驱动程序是激活的，必须先调用这个函数，再调用libusb_claim_interface
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  interface_number 接口号，这个对应接口描述符的 bInterfaceNumber
返回值：0 成功，非0失败


18 libusb_claim_interface
函数原型：int LIBUSB_CALL libusb_claim_interface(libusb_device_handle *dev,        int interface_number);
函数功能：为指定的设备申请接口
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  interface_number 接口号，这个对应接口描述符的 bInterfaceNumber
返回值：0 成功，非0失败


19 libusb_release_interface
函数原型：int LIBUSB_CALL libusb_release_interface(libusb_device_handle *dev,        int interface_number);
函数功能：释放之前为指定的设备申请接口，注意这个函数只是释放接口，不会重新加载内核驱动
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  interface_number 接口号，这个对应接口描述符的 bInterfaceNumber
返回值：0 成功，非0失败


19 libusb_attach_kernel_driver
函数原型：int LIBUSB_CALL libusb_attach_kernel_driver(libusb_device_handle *dev,        int interface_number);
函数功能：加载指定接口的内核驱动
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  interface_number 接口号，这个对应接口描述符的 bInterfaceNumber
返回值：0 成功，非0失败


下面是示例代码：
        if(libusb_kernel_driver_active(handle, bInterfaceNumber) == 1) //判断内核驱动时候加载
        {

                if(libusb_detach_kernel_driver(handle, bInterfaceNumber) == 0) //卸载驱动，例如我们操作的是一个U盘，那么执行到这里设备文件里面的U盘将消失
                {
                         printf("Kernel Driver Detached!");
                }
         }
         libusb_claim_interface(handle, bInterfaceNumber);
         ..........对设备进行读写操作
         libusb_release_interface(handle, bInterfaceNumber); //释放请求的接口

         libusb_attach_kernel_driver(handle, bInterfaceNumber); //加载内核驱动，U盘将重新出现在设备文件里

20 libusb_set_auto_detach_kernel_driver
函数原型：int LIBUSB_CALL libusb_set_auto_detach_kernel_driver(        libusb_device_handle *dev, int enable);
函数功能：设置自动卸载内核驱动，注意这个函数调用时不会卸载内核驱动，只是做标记。在调用libusb_claim_interface的时候卸载内核驱动
          在调用libusb_release_interface的时候自动加载内核驱动
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  enable  1 使能自动卸载的功能，0关闭
返回值：0 成功，非0失败
例如 下面的代码和上面的代码效果等同
        libusb_set_auto_detach_kernel_driver(handle,1);
        libusb_claim_interface(handle, bInterfaceNumber);  //请求接口之前先卸载内核驱动
        ..........对设备进行读写操作
        libusb_release_interface(handle, bInterfaceNumber); //释放请求的接口后自动加载内核驱动

21 libusb_bulk_transfer
函数原型：int LIBUSB_CALL libusb_bulk_transfer(libusb_device_handle *dev_handle,unsigned char endpoint, unsigned char *data, int length,int *actual_length, unsigned int timeout);
函数功能：执行USB批量传输。该函数可以处理输入和输出，根据端点地址的方向位推断传输方向，该函数采用同步模式，数据传输完毕才返回
参数说明：dev_handle 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  endpoint 端点地址 最高位为1表示输入
                  data     发送或者接收缓冲区指针
                  length   缓冲区长度
                  actual_length 带回实际传输长度
                  timeout 超时的毫秒数，0 永不超时
返回值：0 成功，非0失败


22 libusb_clear_halt
函数原型：int LIBUSB_CALL libusb_clear_halt(libusb_device_handle *dev,        unsigned char endpoint);
函数功能：清除端点的halt/stall状态，libusb_bulk_transfer有可能返回LIBUSB_ERROR_PIPE，这是需要调用这个函数
参数说明：dev 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
          endpoint 出错的端点地址
返回值：0 成功，非0失败


23 libusb_interrupt_transfer
函数原型：int LIBUSB_CALL libusb_interrupt_transfer(libusb_device_handle *dev_handle,unsigned char endpoint, unsigned char *data, int length,int *actual_length, unsigned int timeout);
函数功能：执行USB中断传输。该函数可以处理输入和输出，根据端点地址的方向位推断传输方向，该函数采用同步模式，数据传输完毕才返回
参数说明：dev_handle 调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  endpoint 端点地址 最高位为1表示输入
                  data     发送或者接收缓冲区指针
                  length   缓冲区长度
                  actual_length 带回实际传输长度
                  timeout 超时的毫秒数，0 永不超时
返回值：0 成功，非0失败


24 libusb_set_configuration
函数原型：int LIBUSB_CALL libusb_set_configuration(libusb_device_handle *dev,int configuration);
函数功能：为设备设置一个配置参数，大部分设备只有一个配置这个函数通常不需要调用。当某个USB设备有多个配置的时候需要设置
参数说明：dev  调用 libusb_open或者libusb_open_device_with_vid_pid返回的libusb_device_handle的句柄
                  configuration 配置参数，这个对应的是配置描述符里面的bConfigurationValue
返回值：0 成功，非0失败
---------------------
作者：wince_lover
来源：CSDN
原文：https://blog.csdn.net/wince_lover/article/details/70337809
版权声明：本文为博主原创文章，转载请附上博文链接！

 */
