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
	int32_t pipe;
	//status of transfer
	int32_t status;
	//Settings for transfer
	int32_t transfer_flags;
	//Pointer to the buffer to be used when sending/receiving data
	void *transfer_buffer;
	//data transfer buffer when DMA is used
	dma_addr_t transfer_dma;
	//transfer buffer length
	int32_t transfer_buffer_length;
	//actual length sent or recieved by urb
	int32_t actual_length;
	//completion handler called when transfer complete
	usb_complete_t complete;
	//data blob used when the completion handler is called
	void *context;
	//Setup packet transferred before the data in transfer buffer (control urbs)
	uint8_t *setup_packet;
	//Same as above but with dma
	dma_addr_t setup_dma;
	//Urb polling interval (iso and interrupt urb)
	int32_t interval;
	//Number of iso transfers that reported an error
	int32_t error_count;
	//Sets or returns the intial frame to use (iso urb)
	int32_t start_frame;
	//Number of iso transfer buffers to use (iso urb)
	int32_t number_of_packets;
	//Allows a single urb to define multiple iso transfers at once (iso urb)
	struct usb_iso_packet_descriptor iso_frame_desc[0];	
}

//Functions used to initialize the pipe of a urb structure
uint32_t usb_sndctrlpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_rcvctrlpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_sndbulkpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_rcvbulkpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_sndintpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_rcvintpipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_sndisopipe(struct usb_device *dev, uint32_t endpoint);
uint32_t usb_rcvisopipe(struct usb_device *dev, uint32_t endpoint);

//used to allocate urb's
//iso_packets should be 0 for anything other than iso transfers
struct urb *usb_alloc_urb(int32_t iso_packets, gfp_t mem_flags);

//used to free urbs
void usb_free_urb(struct urb *urb);

//used to fill in interrupt urb
void usb_fill_int_urb(struct urb *urb, struct usb_device *dev, uint32_t pipe, void *transfer_buffer, int32_t buffer_length, usb_complete_t complete, void *context, int32_t interval);

//used to fill in bulk urb
void usb_fill_bulk_urb(struct urb *urb, struct usb_device *dev, uint32_t pipe, void *transfer_buffer, int32_t buffer_length, usb_complete_t complete, void *context);

//used to fill in a control urb
void usb_fill_control_urb(struct urb *urb, struct usb_device *dev, uint32_t pipe, uint8_t *setup_packet, void *transfer_buffer, int32_t buffer_length, usb_complete_t complete, void *context);

//allocate a dma buffer
void *usb_buffer_alloc(struct usb_device *dev, int32_t size, gfp_t mem_flags, dma_addr_t *dma);

//free the dma buffers
void usb_buffer_free(struct usb_device *dev, int32_t size, void *addr, dma_addr_t dma);

//Submit the urb
int32_t usb_submit_urb(struct urb *urb, int32_t mem_flags);

//Cancel a urb async
int32_t usb_unlink_urb(struct urb *urb);

//Canels a urb sync
void usb_kill_urb(struct urb *urb);

//urb completion handler
void (*usb_complete_t)(struct urb *, struct pt_regs *);

static struct usb_device_id catc_id_table [] = {
{USB_DEVICE() } };
