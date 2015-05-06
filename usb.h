/*
Usb driver 
Authors: Ernesto Soltero, Max Roth, Nicholas Jenis
*/

#ifndef __USB_H
#define __USB_H
#define MAXPACKETSIZE 512
#define USB_ENDPOINT_XFERTYPE_MASK
#define USB_ENDPOINT_XFER_ISOC
#define USB_ENDPOINT_XFER_BULK
#define USB_ENDPOINT_XFER_INT
#define USB_DIR_OUT
#define USB_DIR_IN
#include "types.h"

/*
Notes:
	-The naming scheme for USB sysfs device is root_hub-hub_port:config.interface
*/


/*
Host side endpoint descriptor and queue
*/
struct usb_host_endpoint{
	//endpoint information held here
	struct usb_endpoint_descriptor desc;
	//The USB address of this endpoint
	uint32_t bEndpointAddress;
	//The type of endpoint
	uint32_t bmAttributes;
	//Maximum size of packet transfer
	uint16_t wMaxPacketSize;
	//If endpoint is interrupt this is the interval setting in ms
	uint16_t bInterval;
};

struct usb_interface {
	//Array of interfaces containing alternate settings 
	struct usb_host_interface *altsetting;
	//The number of alternate settings pointed to by altsettings
	uint16_t num_altsetting;
	//A pointer into the array altsetting, this is the current setting for the interface
	struct usb_host_interface *cur_altsetting;
	//The minor number assigned by the USB core ex 2.1 minor num is 1
	uint8_t minor;
}

struct usb_host_config{
	struct usb_config_descriptor desc;	

	//List of interface association descriptions in this configuration 
	struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];
	//The interfaces associated with this config
	struct usb_interface *interface[USB_MAXINTERFACES];
}

struct usb_device{

}

static inline struct usb_device *interface_to_usbdev(struct usb_interface *intf){
	return to_usb_device(intf->dev.parent);
}


struct urb{
	//Pointer to the device which the urb is sent to 
	struct usb_device *dev;
	//Endpoint information for the specific usb device that this urb is going to be sent to
	uint32_t pipe;
	//Settings for transfer
	uint32_t transfer_flags;
	//Pointer to the buffer to be used when sending/receiving data
	void *transfer_buffer;

}


