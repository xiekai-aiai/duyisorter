#ifndef MYUSB_H
#define MYUSB_H

#include <QtGui>
#include "3rdparty/libusb/usb.h"

/* the device's vendor and product id */
#define USB_MY_VID 0x0754
#define USB_MY_PID 0x1002

/* the device's endpoints */
#define USB_EP_IN       0x82
#define USB_EP_OUT      0x02
#define USB_EP_IN_AI    0x86
#define USB_EP_OUT_AI   0x08
#define USB_EP_TEST_IN  0x88

class myusb
{
public:
    myusb();

    usb_dev_handle* myusbOpen(void);
    void myusbFree(void);
    void myusbInit(void);
};

extern usb_dev_handle *myUsbDev;
extern myusb usb;

#endif // MYUSB_H
