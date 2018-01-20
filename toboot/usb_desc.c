/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "usb_desc.h"
#include "webusb_defs.h"

// USB Descriptors are binary data which the USB host reads to
// automatically detect a USB device's capabilities.  The format
// and meaning of every field is documented in numerous USB
// standards.  When working with USB descriptors, despite the
// complexity of the standards and poor writing quality in many
// of those documents, remember descriptors are nothing more
// than constant binary data that tells the USB host what the
// device can do.  Computers will load drivers based on this data.
// Those drivers then communicate on the endpoints specified by
// the descriptors.

// To configure a new combination of interfaces or make minor
// changes to existing configuration (eg, change the name or ID
// numbers), usually you would edit "usb_desc.h".  This file
// is meant to be configured by the header, so generally it is
// only edited to add completely new USB interfaces or features.

// **************************************************************
//   USB Device
// **************************************************************

#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

#define USB_DT_BOS_SIZE 5
#define USB_DT_BOS 0xf
#define USB_DT_DEVICE_CAPABILITY 0x10
#define USB_DC_PLATFORM 5

struct usb_bos_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumDeviceCaps;
    /* Descriptor ends here.  The following are used internally: */
    const struct usb_device_capability_descriptor **capabilities;
} __attribute__((packed));

// USB Device Descriptor.  The USB host reads this first, to learn
// what type of device is connected.
static const uint8_t device_descriptor[] = {
        18,                                     // bLength
        1,                                      // bDescriptorType
        0x00, 0x02,                             // bcdUSB
        0x00,                                   // bDeviceClass
        0x00,                                   // bDeviceSubClass
        0x00,                                   // bDeviceProtocol
        EP0_SIZE,                               // bMaxPacketSize0
        LSB(VENDOR_ID), MSB(VENDOR_ID),         // idVendor
        LSB(PRODUCT_ID), MSB(PRODUCT_ID),       // idProduct
        LSB(DEVICE_VER), MSB(DEVICE_VER),       // bcdDevice
        1,                                      // iManufacturer
        2,                                      // iProduct
        0,                                      // iSerialNumber
        1                                       // bNumConfigurations
};

// These descriptors must NOT be "const", because the USB DMA
// has trouble accessing flash memory with enough bandwidth
// while the processor is executing from flash.


// **************************************************************
//   USB Configuration
// **************************************************************

// USB Configuration Descriptor.  This huge descriptor tells all
// of the devices capbilities.
static const uint8_t config_descriptor[CONFIG_DESC_SIZE] = {
        // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
        9,                                      // bLength;
        2,                                      // bDescriptorType;
        LSB(CONFIG_DESC_SIZE),                  // wTotalLength
        MSB(CONFIG_DESC_SIZE),
        NUM_INTERFACE,                          // bNumInterfaces
        1,                                      // bConfigurationValue
        2,                                      // iConfiguration
        0x80,                                   // bmAttributes
        50,                                     // bMaxPower

        // interface descriptor, DFU Mode (DFU spec Table 4.4)
        9,                                      // bLength
        4,                                      // bDescriptorType
        DFU_INTERFACE,                          // bInterfaceNumber
        0,                                      // bAlternateSetting
        0,                                      // bNumEndpoints
        0xFE,                                   // bInterfaceClass
        0x01,                                   // bInterfaceSubClass
        0x02,                                   // bInterfaceProtocol
        2,                                      // iInterface

        // DFU Functional Descriptor (DFU spec TAble 4.2)
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x0D,                                   // bmAttributes
        LSB(DFU_DETACH_TIMEOUT),                // wDetachTimeOut
        MSB(DFU_DETACH_TIMEOUT),
        LSB(DFU_TRANSFER_SIZE),                 // wTransferSize
        MSB(DFU_TRANSFER_SIZE),
        0x01,0x01,                              // bcdDFUVersion
};


// **************************************************************
//   String Descriptors
// **************************************************************

// The descriptors above can provide human readable strings,
// referenced by index numbers.  These descriptors are the
// actual string data

struct usb_string_descriptor_struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wString[];
};

extern const struct usb_string_descriptor_struct usb_string_manufacturer_name
        __attribute__ ((weak, alias("usb_string_manufacturer_name_default")));
extern const struct usb_string_descriptor_struct usb_string_product_name
        __attribute__ ((weak, alias("usb_string_product_name_default")));

static const struct usb_string_descriptor_struct string0 = {
    4,
    3,
    {0x0409}
};

// Microsoft OS String Descriptor. See: https://github.com/pbatard/libwdi/wiki/WCID-Devices
static const struct usb_string_descriptor_struct usb_string_microsoft = {
    18, 3,
    {'M','S','F','T','1','0','0', MSFT_VENDOR_CODE}
};
 
// Microsoft WCID
const uint8_t usb_microsoft_wcid[MSFT_WCID_LEN] = {
    MSFT_WCID_LEN, 0, 0, 0,         // Length
    0x00, 0x01,                     // Version
    0x04, 0x00,                     // Compatibility ID descriptor index
    0x01,                           // Number of sections
    0, 0, 0, 0, 0, 0, 0,            // Reserved (7 bytes)

    0,                              // Interface number
    0x01,                           // Reserved
    'W','I','N','U','S','B',0,0,    // Compatible ID
    0,0,0,0,0,0,0,0,                // Sub-compatible ID (unused)
    0,0,0,0,0,0,                    // Reserved
};

#ifndef LANDING_PAGE_URL
#define LANDING_PAGE_URL "dfu.tomu.im"
#endif

#define LANDING_PAGE_DESCRIPTOR_SIZE (WEBUSB_DT_URL_DESCRIPTOR_SIZE \
                                    + sizeof(LANDING_PAGE_URL) - 1)
#define WEBUSB_VENDOR_CODE 2

static const struct webusb_platform_descriptor webusb_platform = {
    .bLength = WEBUSB_PLATFORM_DESCRIPTOR_SIZE,
    .bDescriptorType = USB_DT_DEVICE_CAPABILITY,
    .bDevCapabilityType = USB_DC_PLATFORM,
    .bReserved = 0,
    .platformCapabilityUUID = WEBUSB_UUID,
    .bcdVersion = 0x0100,
    .bVendorCode = WEBUSB_VENDOR_CODE,
    .iLandingPage = 1
};

static const struct webusb_url_descriptor landing_url_descriptor = {
    .bLength = LANDING_PAGE_DESCRIPTOR_SIZE,
    .bDescriptorType = WEBUSB_DT_URL,
    .bScheme = WEBUSB_URL_SCHEME_HTTPS,
    .URL = LANDING_PAGE_URL
};

static const struct usb_device_capability_descriptor* capabilities[] = {
    (const struct usb_device_capability_descriptor*)&webusb_platform,
};

static const struct usb_bos_descriptor bos = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .wTotalLength = USB_DT_BOS_SIZE + sizeof(webusb_platform),
    .bNumDeviceCaps = sizeof(capabilities)/sizeof(capabilities[0]),
    .capabilities = capabilities
};

const struct usb_string_descriptor_struct usb_string_manufacturer_name_default = {
    2 + MANUFACTURER_NAME_LEN * 2,
    3,
    MANUFACTURER_NAME
};
const struct usb_string_descriptor_struct usb_string_product_name_default = {
    2 + PRODUCT_NAME_LEN * 2,
    3,
    PRODUCT_NAME
};


// **************************************************************
//   Descriptors List
// **************************************************************

// This table provides access to all the descriptor data above.

const usb_descriptor_list_t usb_descriptor_list[] = {
    {0x0100, device_descriptor, sizeof(device_descriptor)},
    {0x0200, config_descriptor, sizeof(config_descriptor)},
    {0x0300, (const uint8_t *)&string0, 0},
    {0x0301, (const uint8_t *)&usb_string_manufacturer_name, 0},
    {0x0302, (const uint8_t *)&usb_string_product_name, 0},
    {0x03EE, (const uint8_t *)&usb_string_microsoft, 0},
    {0x0F00, (const uint8_t *)&bos, sizeof(bos)},
    {0x0F01, (const uint8_t *)&landing_url_descriptor, LANDING_PAGE_DESCRIPTOR_SIZE},
    {0, NULL, 0}
};