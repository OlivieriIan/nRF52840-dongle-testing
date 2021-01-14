from bluepy.btle import Scanner, Peripheral
import time
"""
Device 
  dev.addr:         d8:c2:e6:99:b8:56
  dev.addrType:     random
  dev.rssidev.rssi: -61 dB
"""
if __name__ == "__main__":
    scanner = Scanner()
    devices = scanner.scan(1.0)
    target_name = "Nordic_Blinky"
    target_address = None
    for dev in devices:
        for (adtype, desc, value) in dev.getScanData():
            # print("Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi))
            if (desc == "Complete Local Name") and (value == target_name):
                target_address = dev.addr
                target_addr_type = dev.addrType
                print("Device with name '{}' found! Device address: {}. RSSI: {}"\
                    .format(target_name, target_address, dev.rssi))
                break
        if target_address is not None: # Device found
            break

    if target_address is None:
        print("No device device with name '{}' found. Closing program."\
            .format(target_name))
        exit()
    
    peripheral = Peripheral()
    peripheral.connect(target_address, target_addr_type)
    print("=== Characteristics ===")
    characteristics = peripheral.getCharacteristics()
    led_characteristic = None
    btn_characteristic = None
    for c in characteristics:
        # print("UUID: {}. Properties: {}. supportsRead: {}. propertiesToString: {}"\
        #     .format(c.uuid, c.properties, c.supportsRead(), c.propertiesToString()))
        if c.uuid == "00001524-1212-efde-1523-785feabcd123": # Button
            btn_characteristic = c
            print("UUID: {}. Value: {}".format(c.uuid, c.read()))
        if c.uuid == "00001525-1212-efde-1523-785feabcd123": # LED
            led_characteristic = c
            

    print("BTN characteristic: {}. LED characteristic: {}"\
        .format(btn_characteristic, led_characteristic))
    #led_val = b'\x00'
    if (led_characteristic is not None) and (btn_characteristic is not None):
        print("Press the button in order to toggle the LED!")
        while 1:
            led_val = btn_characteristic.read()
            # if   led_val == b'\x00': led_val = b'\x01'
            # elif led_val == b'\x01': led_val = b'\x00'
            led_characteristic.write(led_val)

    # print("=== Services ===")
    # services = peripheral.getServices()
    # for s in services:
    #     print("Service UUID: {}".format(s.uuid))
    #print(peripheral.getDescriptors())
    peripheral.disconnect()