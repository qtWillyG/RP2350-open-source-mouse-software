/*
 * RP2350 (Raspberry Pi Pico 2) — Custom USB HID Mouse with selectable identity
 *
 * Board package: "Raspberry Pi Pico/RP2040/RP2350" by Earle Philhower
 * Tools menu: set "USB Stack" to "Adafruit TinyUSB"
 *
 * Demonstrates:
 *   - Overriding USB descriptors (VID/PID, Manufacturer, Product strings)
 *   - A profile table (array of structs) so you can switch device identity
 *   - Basic HID mouse move + click to prove enumeration works
 *
 * NOTE: The VID/PIDs below are placeholders / example "custom device" values.
 * Use your own. If you publish a product you need a real USB-IF Vendor ID,
 * or use a project-allocated VID such as pid.codes for hobby work.
 */

#include "Adafruit_TinyUSB.h"

// ----------------------------------------------------------------------------
// Device identity profiles  (fill these in with YOUR OWN identities)
// ----------------------------------------------------------------------------
struct MouseProfile {
  const char* name;          // human-readable label for your own reference
  uint16_t    vid;           // USB Vendor ID
  uint16_t    pid;           // USB Product ID
  const char* manufacturer;  // USB iManufacturer string
  const char* product;       // USB iProduct string
};

// Example placeholder profiles — replace with your own device identities.
// (cafe / c0de / etc. are obviously-fake example IDs.)
const MouseProfile profiles[] = {
  { "Custom A", 0xCAFE, 0x4001, "MyLab",      "Custom HID Mouse A" },
  { "Custom B", 0xCAFE, 0x4002, "MyLab",      "Custom HID Mouse B" },
  { "Dev Rig",  0x1209, 0x0001, "DevWorks",   "Prototype Pointer"  }, // 0x1209 = pid.codes test VID
  { "Test Jig", 0xC0DE, 0x0010, "QA Bench",   "HID Test Fixture"   },
};
const size_t NUM_PROFILES = sizeof(profiles) / sizeof(profiles[0]);

// Which profile to apply on boot. Change this index, or wire up a switch/jumper
// and select based on a GPIO read (see selectProfileFromHardware() below).
uint8_t activeProfile = 0;

// ----------------------------------------------------------------------------
// HID setup — standard boot-mouse report descriptor from TinyUSB
// ----------------------------------------------------------------------------
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_MOUSE()
};

Adafruit_USBD_HID usb_hid;

// HID report IDs (single report here, so ID 0)
enum { RID_MOUSE = 0 };

// ----------------------------------------------------------------------------
// Optional: pick a profile from hardware (e.g. a DIP switch on GP14/GP15)
// ----------------------------------------------------------------------------
uint8_t selectProfileFromHardware() {
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  uint8_t sel = (!digitalRead(14)) | ((!digitalRead(15)) << 1);
  if (sel >= NUM_PROFILES) sel = 0;
  return sel;
}

// ----------------------------------------------------------------------------
void setup() {
  // Uncomment to choose identity from a jumper/switch instead of the constant:
  // activeProfile = selectProfileFromHardware();

  const MouseProfile& p = profiles[activeProfile];

  // --- Override USB descriptors. Must happen before begin()/enumeration. ---
  TinyUSBDevice.setID(p.vid, p.pid);
  TinyUSBDevice.setManufacturerDescriptor(p.manufacturer);
  TinyUSBDevice.setProductDescriptor(p.product);

  // --- Configure and start the HID mouse interface ---
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // On the RP2350 Arduino core, USB may already be up by the time setup() runs.
  // Force a re-enumeration so the new descriptors take effect on the host.
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.begin(115200);
}

// ----------------------------------------------------------------------------
// Basic HID mouse functionality to verify it works
// ----------------------------------------------------------------------------
void mouseMove(int8_t dx, int8_t dy) {
  if (!usb_hid.ready()) return;
  usb_hid.mouseMove(RID_MOUSE, dx, dy);
}

void mouseLeftClick() {
  if (!usb_hid.ready()) return;
  usb_hid.mouseButtonPress(RID_MOUSE, MOUSE_BUTTON_LEFT);
  delay(15);
  usb_hid.mouseButtonRelease(RID_MOUSE);
}

void loop() {
  // Simple proof-of-life: nudge the cursor in a small square every 2 seconds.
  if (usb_hid.ready()) {
    mouseMove(20, 0);  delay(200);
    mouseMove(0, 20);  delay(200);
    mouseMove(-20, 0); delay(200);
    mouseMove(0, -20); delay(200);
  }
  delay(2000);
}
