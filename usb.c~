/*
Authors: Ernesto Soltero, Nick Jenis, Max Roth
*/
#include "usb.h"
#include "common.h"
#define HP_USB_VENDORID 0x03f0 /*Hewlett Packard*/
#define HP_USB_PRODUCTID 0x3207 /* 4 gb device id (its actually 2gb but identifies as 4)*/
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x082
#define MAX_PKT_SIZE 512

static struct usb_device *device;
static struct usb_class_driver class;
static uint8_t data_transfer_buffer[MAX_PKT_SIZE];

/*for later use with file system to open file*/
static int usb_open(struct file *f){

}

/*For later use with file system to close file*/
static int usb_close(struct file *f){

}

/*
Reads the file from storage and passes back the data 
through a pipe putting it into the buffer
*/
static int32_t usb_read(struct file *f, uint8_t *buffer, int32_t count){
	int32_t retval;
	int32_t read_count;

	retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN), data_trasfer_buffer, MAX_PKT_SIZE, &read_count, 5000);
	if(retval){
		/*print out the data here stored in buffer*/
		
	}
}

/*
Writes to the storage using pipes returns 1 on success
-1 on fail
*/
static int32_t usb_write(void){
	int32_t retval;
	int32_t wrote_count;

	retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT), data_transfer_buffer, 
	
}

/*
This function should be called when a usb device is inserted into the hub 
and we should be able to identify the device info
*/
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id){
	/*This function should be called when we attach our usb
	printk(KERN_INFO "Usb drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
	*/
	return 0;
}

/*
Called when the usb device is disconnected and we clean up
*/
static void usb_disconnect(struct usb_interface *interface){
	/*This function should be called when we disconnect our usb
	
	printk(KERN_INFO "Usb drive has been removed\n");*/
}

/* This only contains the specific id for the HP usb drives in the lab*/
static struct usb_device_id device_table[] = {
	{USB_DEVICE(HP_USB_VENDORID, HP_USB_PRODUCTID)},
	{}
};
MODULE_DEVICE_TABLE(usb, device_table);

/*
Driver info for the hp flash drive
*/
static struct usb_driver hp_driver = {
	.name = "2gb USB driver",
	.id_table = device_table,
	.probe = usb_probe,
	.disconnect = usb_disconnect,
};

/*
Register this driver 
*/
static int __init usb_init(void){
	return usb_register(&hp_driver);
}

/*
Deregister the driver
*/
static void __exit usb_exit(void){
	usb_deregister(&hp_driver);
}
