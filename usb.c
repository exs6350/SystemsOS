/*
Authors: Ernesto Soltero, Nick Jenis, Max Roth
*/
#include "usb.h"
#include "types.h"
#include "klib.h"

// Host endpoint 
struct usb_host_endpoint{
	struct usb_endpoint_descriptor desc;
	struct usb_ss_ep_comp_descriptor ss_ep_comp;
	struct list_head urb_list;
	void *hcpriv;
	struct ep_device *ep_dev;
	//Extra descriptors 8 bits
	uint8_t *extra;
	//32 bit flags
	int32_t enabled;
	int32_t streams;
}
