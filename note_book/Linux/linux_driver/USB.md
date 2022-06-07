# USB设备驱动  
Linux系统实现了几类通用的USB设备驱动，划分为如下几个设备类：     
音频、通信、HID(人机接口)、显示、存储、电源、打印、集线器    
一般的通用的Linux设备如U盘，USB鼠标，USB键盘等都不需要工程师再编写驱动，需要编写的是特定厂商，特定芯片的驱动，而且往往也可以参考内核中已提供的驱动的模板。    
Linux内核为各类USB设备分配了相应的设备号，提供了USB设备文件系统usbdevfs或usbfs，它和/proc类似，都是动态产生的。  


### usb_druver结构体  
```C
struct usb_driver {
	const char *name;

	int (*probe) (struct usb_interface *intf,
		      const struct usb_device_id *id);

	void (*disconnect) (struct usb_interface *intf);

	int (*unlocked_ioctl) (struct usb_interface *intf, unsigned int code,
			void *buf);

	int (*suspend) (struct usb_interface *intf, pm_message_t message);
	int (*resume) (struct usb_interface *intf);
	int (*reset_resume)(struct usb_interface *intf);

	int (*pre_reset)(struct usb_interface *intf);
	int (*post_reset)(struct usb_interface *intf);

	const struct usb_device_id *id_table;

	struct usb_dynids dynids;
	struct usbdrv_wrap drvwrap;
	unsigned int no_dynamic_id:1;
	unsigned int supports_autosuspend:1;
	unsigned int disable_hub_initiated_lpm:1;
	unsigned int soft_unbind:1;
};
```
编写新的USB驱动时，主要应该完成的工作时probe和disconnect函数，即探测和断开函数，它们分别在设备插入和拔出时被调用，用于初始化和释放软硬件资源。   
对usb_driver的注册和注销通过下面两个函数完成；   
```C
int usb_register(struct usb_driver *new_driver);
void usb_deregister(struct usb_driver *driver);
```
usb_driver结构体中的id_table成员描述了这个usb驱动所支持的USB设备列表，它指向一个usb_device_id数组，usb_device_id结构体用于包含USB设备的制造商ID、产品ID、产品版本、设备类、接口类等信息机器要匹配标志成员。    
可以通过宏`USB_DEVICE(vid,pid)`进行生成：如   
```C
static struct usb_device_id mydev_table[] = {
    {USB_DEVICE(vendor,product)},
    {USB_DEVICE(vendor1,product1)}
};
MODULE_DEVICE_TABLE(usb,mydev_table);
``` 
这个宏有两个参数，第一个参数设备名，第二个参数该设备加入到模块中时对应产生的设备搜索符号，它的功能为：  
1、将设备假如到外设队列中     
2、告诉程序阅读者该设备是热插拔设备或者设备支持热插拔功能    

上述usb_driver结构体中的函数是USB设备驱动中USB相关的部分，而USB只是一个总线，真正的USB设备驱动的主体工作仍然是USB设备本身所属类型的驱动，如字符设备、tty设备、块设备、输入设备等，因此USB设备驱动包含其作为总线上挂在设备的驱动和本身所属设备类型的驱动两部分。   
与paltform_driver类似，usb_driver起到了"牵线"的作用，即在probe里注册相应的字符，tty设备等，在disconnect里注销相应的字符、tty设备。而原先对设备的注册和注销一般直接发生在模块加载和卸载函数中。   
由此可见，usb_drier本身只是起到了找到USB设备、管理USB设备连接和断开的作用，最终的通信还是通过URB来实现。  


### USB请求块 URB  
- urb结构体   
USB请求块是USB设备驱动中用来描述与USB设备通信所用的基本载体和核心数据结构，非常类似于网络设备驱动中的sk_buff结构体。  
```C
struct urb {
	/* private: usb core and host controller only fields in the urb */
	struct kref kref;		/* reference count of the URB */
	void *hcpriv;			/* private data for host controller */
	atomic_t use_count;		/* concurrent submissions counter */
	atomic_t reject;		/* submissions will fail */
	int unlinked;			/* unlink error code */

	/* public: documented fields in the urb that can be used by drivers */
	struct list_head urb_list;	/* list head for use by the urb's
					 * current owner */
	struct list_head anchor_list;	/* the URB may be anchored */
	struct usb_anchor *anchor;
	struct usb_device *dev;		/* (in) pointer to associated device */
	struct usb_host_endpoint *ep;	/* (internal) pointer to endpoint */
	unsigned int pipe;		/* (in) pipe information */
	unsigned int stream_id;		/* (in) stream ID */
	int status;			/* (return) non-ISO status */
	unsigned int transfer_flags;	/* (in) URB_SHORT_NOT_OK | ...*/
	void *transfer_buffer;		/* (in) associated data buffer */
	dma_addr_t transfer_dma;	/* (in) dma addr for transfer_buffer */
	struct scatterlist *sg;		/* (in) scatter gather buffer list */
	int num_mapped_sgs;		/* (internal) mapped sg entries */
	int num_sgs;			/* (in) number of entries in the sg list */
	u32 transfer_buffer_length;	/* (in) data buffer length */
	u32 actual_length;		/* (return) actual transfer length */
	unsigned char *setup_packet;	/* (in) setup packet (control only) */
	dma_addr_t setup_dma;		/* (in) dma addr for setup_packet */
	int start_frame;		/* (modify) start frame (ISO) */
	int number_of_packets;		/* (in) number of ISO packets */
	int interval;			/* (modify) transfer interval
					 * (INT/ISO) */
	int error_count;		/* (return) number of ISO errors */
	void *context;			/* (in) context for completion */
	usb_complete_t complete;	/* (in) completion routine */
	struct usb_iso_packet_descriptor iso_frame_desc[0];
					/* (in) ISO ONLY */
};
```  
- 创建一个urb结构体  
```C
struct urb *usb_alloc_urb(int iso_packets,int mem_flags);
``` 
iso_packets是这个urb应当包含的等时数据包的数目，若为0表示不创建等时数据包，mem_flags参数是分配内存的表示，和kmalloc函数的分配标志参数含义相同。如果分配成功，该函数返回一个urb结构体。否则返回0    urb结构体在驱动中**不能静态创建**，因为这可能破坏USB核心给urb使用的引用计数方法。 
释放urb`void usb_free_urb(struct urb *urb);`    

对urb进行成功赋值初始化后，需要提交urb给USB核心，直到结束处理例程函数被调用前，都不能访问urb结构的任何成员。  
通过`int usb_submit_urb(struct urb *urb,gfp_t mem_flags);`  
在该函数被成功调用后，对urb的控制权传递给USB核心，函数调用成功，urb被结束例程会被调用，表示usb核心完成了urb，并将控制权返回给驱动设备。  


- usb_set_intdata(interface,dev)   
将私有数据指针保存在设备结构中,即将dev保存在interface   

- usb_get_intfdata(interface);  
从设备结构中获取私有数据  
获取后如果需要清除则通过
`usb_set_intdata(interface,NULL)`