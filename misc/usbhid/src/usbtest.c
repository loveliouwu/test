#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <string.h>
#include <sys/time.h>

#define MAX_STR 255

long long ustime(void) {
    struct timeval tv;
    long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust;
}

long long mstime(void) {
    return ustime()/1000;
}

int main()
{


	int res;
	unsigned char buf[512];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Initialize the hidapi library
	res = hid_init();
	if(res)
	{
		printf("hid init err!\n");
	}


	struct hid_device_info *devs, *cur_dev;
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage_page:	%04hx\n",cur_dev->usage_page);
		printf("  Usage:	%04hx\n",cur_dev->usage);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	//handle = hid_open(0x1ea7, 0x0064, NULL);
	handle = hid_open(0x8601, 0x8801, NULL);
	//handle = hid_open(0x1234, 0xabcd, NULL);


	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}
	else
	{
		printf("hid_open ok!\n");
	}


	wchar_t strr[64];
	size_t len = 32;
	//res = get_device_string(handle,DEVICE_STRING_MANUFACTURER,strr,32);
        //printf("get_device_string : %s\n",strr);

	/*写计时*/
	long long llStart = mstime();

	memset(buf,0x11,512);
	int count = 2048;//1M字节

	for(i = 0; i<count ;i++)
		res = hid_write(handle, buf, 512);

	long long llEnd=mstime();
	printf("use time: %lld ms\n",llEnd - llStart);
/*
*/
	/*读计时*/
	llStart = mstime();
	for(i = 0; i<count ;i++)
		res = hid_read(handle, buf, 65);


	llEnd=mstime();
	printf("use time: %lld ms\n",llEnd - llStart);

	for (i = 0; i < 65; i++)
		printf("%X ",buf[i]);



	res = hid_exit();
	while(1)
	{
		int i=0;
		while(i++<500000000);
		printf("over!\n");
		return 0;
	}


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x0e0f, 0x0003, NULL);
	//andle = hid_open(0x1ea7, 0x0064, NULL);

	printf("3333333333\n");
	// Read the Manufacturer String
	if(handle==NULL)
	{
		printf("open dev err! \n");
		return 0;
	}
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);
	printf("4444444444\n");
	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	for (i = 0; i < 4; i++)
		printf("buf[%d]: %d\n", i, buf[i]);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
