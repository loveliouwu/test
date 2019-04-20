#include<linux/module.h>

#include<linux/kernel.h>

#include<linux/usb.h>

#include<linux/mod_devicetable.h>



static int probe_usb(struct usb_interface *interface, const struct usb_device_id *id){

  printk("USB (%04X:%04X) plugged\n", id->idVendor, id->idProduct);

  return 0;

}

static void disconnect_usb(struct usb_interface *interface){

  printk("USB removed\n");

}



static struct usb_device_id usb_pen_table[] =

{

      { USB_DEVICE(0x1234, 0xabcd) },

      {} /* Terminating entry */

};

MODULE_DEVICE_TABLE (usb, usb_pen_table);

static struct usb_driver usb_driver =

{

      .name = "usb_driver",

      .id_table = usb_pen_table,

      .probe = probe_usb,

      .disconnect = disconnect_usb,

};






static int __init usb_init(void)

{	

	int result = usb_register(&usb_driver);

	if(result == 0){

		printk("MY_USB_INIT : I am in USB_INIT function\n");

		return 0;

	}

	printk("ERROR : Couldn't register the USB Drive\n");

	return 1;

}

static void __exit usb_exit(void)

{

	usb_deregister(&usb_driver);

	printk("MY_USB_EXIT : I am in USB_EXIT function\n");

	return 0;

}



static int usb_probe(struct usb_interface *intf, const struct usb_device_id *id)

{

	printk("MY_USB_PROBE : Device Detected vendor_id [0x%x] , Product_id [0x%x]\n",id->idVendor,id->idProduct);

	return 0;

}



static void usb_disconnect(struct usb_interface *interface)

{

	printk("MY_USB_DISCONNECT : Device Removed\n");

} 





module_init(usb_init);

module_exit(usb_exit);




MODULE_LICENSE("GPL");

MODULE_AUTHOR("Alex Guerriero");

MODULE_DESCRIPTION("Template Character Driver");
