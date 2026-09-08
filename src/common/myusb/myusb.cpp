#include "myusb.h"

myusb::myusb()
{
}

usb_dev_handle* myusb::myusbOpen()
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for(bus=usb_get_busses(); bus; bus=bus->next){
        for(dev=bus->devices; dev; dev=dev->next){
            if(dev->descriptor.idVendor==USB_MY_VID && dev->descriptor.idProduct==USB_MY_PID)
                return usb_open(dev);
        }
    }

    return NULL;
}

void myusb::myusbFree()
{
    if(myUsbDev != NULL){
        usb_release_interface(myUsbDev, 0);
        usb_close(myUsbDev);
        myUsbDev = NULL;
    }
}

void myusb::myusbInit()
{
    usb_init();         // initialize the library
    usb_find_busses();  // find all busses
    usb_find_devices(); // find all connected devices

    if ((myUsbDev = myusbOpen()) == NULL) {
        fprintf(stderr, "myusb_init: usb device not found\n");
        return;
    }
    else {
        if (usb_set_configuration(myUsbDev, 1) < 0) {
            fprintf(stderr, "myusb_init: usb setting config 1 failed\n");
            usb_close(myUsbDev);
            myUsbDev = NULL;
            return;
        }
        else {
            if (usb_claim_interface(myUsbDev, 0) < 0) {
                fprintf(stderr, "myusb_init: usb claiming interface 0 failed\n");
                usb_close(myUsbDev);
                myUsbDev = NULL;
                return;
            }
        }
    }
}

usb_dev_handle *myUsbDev = NULL;
myusb usb;
