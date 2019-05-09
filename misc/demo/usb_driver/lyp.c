#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/usb.h>
#include<linux/mod_devicetable.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/string.h>
#include <linux/kref.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

//#include"pe100_usb_driver.h"
#include"netlink_transfer.h"
//#include"cf_log.h"
#include"lyp.h"


#define DRIVER_INFO(fmt, arg...)	printk("[DR_INFO: func: %s line: %d]"fmt,__func__,__LINE__,##arg);
#define DRIVER_ERROR(fmt, arg...)	printk("[DR_ERROR: func: %s line: %d]"fmt, __func__, __LINE__,##arg);


#define MIN(a,b) ((a <= b ) ? a : b)





unsigned int recv_len = MAX_MSGSIZE;
struct usb_host_interface *iface_desc = NULL;
struct usb_endpoint_descriptor *endpoint = NULL;





struct usb_skel *dev;    


/*向应用层发送处理数据的异常状态信息*/
int send_netlink_status(nl_packet_st *data_info, unsigned int status)
{
	int ret = -1;
	if(NULL == data_info)
	{
		return ret;
	}
	data_info->flag = SEND_DATA_FLAG;
	data_info->task_status = status;
	ret = netlink_send((unsigned char *)data_info, sizeof(nl_packet_st),data_info->task_pid);
	return ret;
}


/*NetLink回调函数中对数据解析封USB包*/
int netlink_recv_manage(char *data_addr, unsigned int nlmsg_pid)
{
	unsigned int recv_data_len = 0;
	nl_packet_st * nl_recv_data_header = NULL;

	recv_data_len = ((nl_packet_st *)data_addr)->data_len;
	nl_recv_data_header = kmalloc(recv_data_len + sizeof(nl_packet_st), GFP_KERNEL);
	if(nl_recv_data_header == NULL)
	{
		DRIVER_ERROR("Malloc nl_recv_data_header failed..\n");
		return -1;
	}
	else
	{
		memset(nl_recv_data_header, 0, recv_data_len + sizeof(nl_packet_st));
		nl_recv_data_header->task_pid = nlmsg_pid;
		memcpy(nl_recv_data_header, data_addr, recv_data_len + sizeof(nl_packet_st));
		DRIVER_INFO("recv netlink data ssid:%d, cmd:%d\n",nlmsg_pid, nl_recv_data_header->task_cmd);
	}
	
	if(card_config_start_flag == CONFIG_START)
	{
		if(nl_recv_data_header->task_type != TYPE_CPU)
		{
			send_netlink_status(nl_recv_data_header, SDR_BEINGCONFIGURED);
			return -1;
		}
	}
	


	return 0;
}


/*usb 发送数据接口,最大一次发送512字节*/
static unsigned int  usb_data_send(unsigned char * pbuff,unsigned int length)
{
	int retval = -1;
	unsigned int datalen = MIN(length,MAX_BUFFER_SIZE); //如果大于8.5k则只传输8.5k
	unsigned int offset = 0;
	unsigned int sendlen = 0;

	if(!pbuff)
	{
		DRIVER_ERROR("send buff is NULL !\n");
		return -1;
	}
	DRIVER_INFO("start usb_bulk_msg send data\n");

	memcpy(dev->bulk_out_buffer, pbuff, datalen);  //复制发送的数据到usb发送buffer

	//if(copy_from_user(dev->bulk_out_buffer,pbuff,datalen))//不涉及用户空间指针  无需转换
	if(datalen > MAX_MSGSIZE)
	{
		offset = 0;
		sendlen = MAX_MSGSIZE;
		while(datalen>0)
		{
			retval = usb_bulk_msg(
					dev->udev,
					usb_sndbulkpipe(
						dev->udev,
						dev->bulk_out_endpointAddr
						),
					dev->bulk_out_buffer + offset,//一次把数据全部拷贝进内核空间 发送时添加偏移
					sendlen,
					&recv_len,
					2000			
				);
			if(retval == 0)
			{
				datalen -= sendlen;
				offset += sendlen;
				if(datalen >= MAX_MSGSIZE)
				{
					sendlen = MAX_MSGSIZE;
				}
				else
				{
					sendlen = datalen;					
				}
				
				DRIVER_INFO("usb_bulk_msg send_data ok !\n");
				DRIVER_INFO("send_data = %d\n",dev->bulk_out_buffer[offset-1]);
			}
			else
			{
				DRIVER_ERROR("usb_bulk_msg send_data err ! ret = %d\n",retval);
				return retval;
			}
		}

	}
	else
	{
		retval = usb_bulk_msg(
			dev->udev,
			usb_sndbulkpipe(
				dev->udev,
				dev->bulk_out_endpointAddr
				),
			dev->bulk_out_buffer,
			datalen,
			&recv_len,
			2000			
			);
		if(retval == 0)
		{
			DRIVER_INFO("usb_bulk_msg send_data ok !\n");
			DRIVER_INFO("send_data = %d\n",dev->bulk_out_buffer[1]);
		}
		else
		{
			DRIVER_ERROR("usb_bulk_msg send_data err ! ret = %d\n",retval);
			return retval;
		}
	}

	return recv_len;
	
	
}

/*USB 接收数据接口*/
static unsigned int usb_data_recv(unsigned char * pbuff,unsigned int length)
{
	int retval = -1;
	unsigned int recv_datalen = MIN(length,MAX_BUFFER_SIZE);//总的接收长度
	unsigned int offset = 0;//接收时放入buffer的偏移
	unsigned int recvlen = 0;//每次发送的长度
	if(!pbuff)
	{
		DRIVER_ERROR("recv buff is NULL !\n");
		return -1;
	}
	
	if(recv_datalen > MAX_MSGSIZE)
	{
		recvlen = MAX_MSGSIZE;
		while(recv_datalen > 0)
		{
			retval = usb_bulk_msg(
				dev->udev,
				usb_rcvbulkpipe(
					dev->udev,
					dev->bulk_in_endpointAddr
					),
				dev->bulk_in_buffer + offset,  //接收时候也一次放入内核空间，然后统一取出数据
				recvlen,
				&recv_len,
				2000			
				);
			if(!retval)
			{
				//if (copy_to_user(pbuff+offset, dev->bulk_in_buffer+offset, recvlen))
				recv_datalen -= recv_len;
				offset += recv_len;//实际接收的数据长度
				if(recv_datalen > MAX_MSGSIZE)
				{
					recv_len = MAX_MSGSIZE;
				}
				else
				{
					recv_len = recv_datalen;
				}
				DRIVER_INFO("==========recv_data ok !\n");
				DRIVER_INFO("==========recv len = %d\n",offset);
				DRIVER_INFO("==========recv_data = %d\n",dev->bulk_in_buffer[offset-1]);
					
			}
			else
			{
				DRIVER_ERROR("usb_bulk_msg recv data err !\n");
				return retval;
			}
		}
	}
	else
	{
		retval = usb_bulk_msg(
			dev->udev,
			usb_rcvbulkpipe(
				dev->udev,
				dev->bulk_in_endpointAddr
				),
			dev->bulk_in_buffer,
			recv_datalen,
			&recv_len,
			2000			
			);
		if(!retval)
		{
			if (copy_to_user(pbuff, dev->bulk_in_buffer,recv_len))
			{
				DRIVER_INFO("==========send_data ok !\n");
				DRIVER_INFO("==========recv len = %d\n",recv_len);
				DRIVER_INFO("==========recv_data = %d\n",pbuff[1]);
			}
			else
			{
				retval = -1;
				DRIVER_ERROR("data copy_to_user err !\n");
				return retval;
			}
		        
		}
		else
		{
			DRIVER_ERROR("usb_bulk_msg recv data err !\n");
			return retval;
		}

	}
	memcpy(pbuff, dev->bulk_in_buffer, MIN(length,MAX_BUFFER_SIZE));  //从接收buffer中取出数据
	
	return 0;
}


/*匹配设备，枚举端点，申请发送接受数据空间*/
int usb_plug_device(struct usb_interface *interface, const struct usb_device_id *id)
{

	int i = 0;

	DRIVER_INFO("USB (%04X:%04X) plugged\n", id->idVendor, id->idProduct);//打印出枚举到的设备PID VID

	
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);//申请空间
	if(!dev)
	{
		DRIVER_ERROR("Could not allocate dev\n");
		return -1;
	}
	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;
	iface_desc = interface->cur_altsetting;

	DRIVER_INFO("+++++Pen i/f %d now probed: (%04X:%04X)\n",iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	DRIVER_INFO("+++++ID->bNumEndpoints: %02X\n",iface_desc->desc.bNumEndpoints);
	DRIVER_INFO("+++++ID->bInterfaceClass: %02X\n",iface_desc->desc.bInterfaceClass);

	
	if(iface_desc!=NULL)
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
	{

		endpoint = &iface_desc->endpoint[i].desc;
		if(endpoint!=NULL)
		if (!dev->bulk_in_endpointAddr &&
						(endpoint->bEndpointAddress & USB_DIR_IN) &&
						((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
						== USB_ENDPOINT_XFER_BULK)) 
		{
			DRIVER_INFO("++++++MaxPacketSize = %d\n",endpoint->wMaxPacketSize);
			dev->bulk_data_size = MAX_MSGSIZE;//设置发送最大长度
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			DRIVER_INFO("++++++bulk_in_endpointaddr = %02x\n",dev->bulk_in_endpointAddr);

			
			/*
			dev->bulk_in_buffer = kmalloc(MAX_BUFFER_SIZE, GFP_KERNEL);//申请接收数据空间 8.5K
			if (!dev->bulk_in_buffer) 
			{
					DRIVER_ERROR("Could not allocate bulk_in_buffer\n");
					return 1;
			}
			else
					DRIVER_INFO("++++++allocate bulk_in_buffer\n");
			*/
			
		}

		if (!dev->bulk_out_endpointAddr &&
						!(endpoint->bEndpointAddress & USB_DIR_IN) &&
						((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
						== USB_ENDPOINT_XFER_BULK)) 
		{ 
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
			DRIVER_INFO("++++++bulk_out_endpointaddr = %02x\n",dev->bulk_in_endpointAddr);
			/*
			dev->bulk_out_buffer = kmalloc(MAX_BUFFER_SIZE, GFP_KERNEL);//申请发送数据空间 8.5K
			if (!dev->bulk_out_buffer) 
			{
					DRIVER_ERROR("Could not allocate bulk_out_buffer\n");
					return 1;
			}
			else
					DRIVER_INFO("++++++allocate bulk_out_buffer\n");
			*/
			
		}
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr))
	{

		DRIVER_ERROR("Could not find both bulk-in and bulk-out endpoints");
		return 1;
	}
	return 0;
}




static int probe_usb(struct usb_interface *interface, const struct usb_device_id *id){

	netlink_init(NETLINK_TEST);

	//unsigned char *data = (unsigned char *)kmalloc(1024, GFP_USER);	
	//kfree(data);

	
	
	unsigned char data[1024*8+512];
	memset(data, 6, 1024*8+512);


	usb_plug_device(interface, id);

	usb_data_send(data,1024*8+512);
	usb_data_recv(data,1024*8+512);

	
	return 0;

}

static void disconnect_usb(struct usb_interface *interface){


	//netlink_exit();

/*
	if(dev->bulk_in_buffer!=NULL)
		kfree(dev->bulk_in_buffer);
	if(dev->bulk_out_buffer!=NULL)
		kfree(dev->bulk_out_buffer);

	if(dev!=NULL)
		kzfree(dev);
*/
	printk("USB removed\n");

}



static struct usb_device_id usb_soc_table[] =

{
	{ USB_DEVICE(0x1234, 0xabcd) },
	//{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,USB_INTERFACE_PROTOCOL_MOUSE)},
	{}/* Terminating entry */

};
MODULE_DEVICE_TABLE (usb, usb_soc_table);

static struct usb_driver usb_driver =

{
	.name = "PE_USB_DRIVER",
	.id_table = usb_soc_table,
	.probe = probe_usb,
	.disconnect = disconnect_usb,
};






static int __init usb_init(void)

{	

	int result = usb_register(&usb_driver);
	if(result != 0){
		DRIVER_INFO("usb_init fialed!\n");
		usb_deregister(&usb_driver);
		return -ENODEV;
	}
	DRIVER_INFO("register the USB Drive OK!\n");
	return 0;

}

static void  __exit usb_exit(void)
{
/*
	if(dev->bulk_in_buffer!=NULL)
		kfree(dev->bulk_in_buffer);
	if(dev->bulk_out_buffer!=NULL)
		kfree(dev->bulk_out_buffer);
*/
	if(dev!=NULL)
		kzfree(dev);

	usb_deregister(&usb_driver);
	printk("++++++USB_EXIT : I am in USB_EXIT function\n");

}

module_init(usb_init);
module_exit(usb_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("liyapeng");
MODULE_DESCRIPTION("HX201901 Driver");
MODULE_VERSION("0.0.1");

