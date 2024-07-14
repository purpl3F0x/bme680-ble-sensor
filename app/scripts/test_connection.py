from bleak import BleakClient, BleakGATTCharacteristic
from bleak.uuids import normalize_uuid_16, uuid16_dict
import ctypes

ADDRESS = "C3:44:6F:5A:65:B8"
SERVICE_UUID = "355545c8-c32a-44c5-979d-33a5e643d800"
CHARACTERISTIC_UUID = "355545c8-c32a-44c5-979d-33a5e643d801"

class CustomBLEServiceAdvData(ctypes.LittleEndianStructure):
    _pack_ = 1  # Set structure packing to 1 byte
    _fields_ = [
        ("temperature", ctypes.c_int16),
        ("humidity", ctypes.c_uint16),
        ("pressure", ctypes.c_uint32, 24),

        ("iaq", ctypes.c_uint16, 10),
        ("iaq_accuracy", ctypes.c_uint16, 2),
        ("co2_accuracy", ctypes.c_uint16, 2),
        ("breath_voc_accuracy", ctypes.c_uint16, 2),

        ("co2_equivalent", ctypes.c_uint16),
        
        ("voc", ctypes.c_uint16, 14),
        ("stab_status", ctypes.c_uint16, 1),
        ("reserved", ctypes.c_uint16, 1),

    ]
    def __new__(self, buffer):
        b = buffer[:7] + b'\x00' + buffer[7:]
        return self.from_buffer_copy(b)

    def __init__(self, buffer):
        pass


def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    data = CustomBLEServiceAdvData(data)
    print("Temperature: {:3.2f}°C".format(data.temperature / 100), end='\t', )
    print("Humidity: {:3.2f}%".format(data.humidity / 100) , end='\t')
    print("Pressure: {:6.1f}Pa".format(data.pressure / 10), end='\t')
    print("IAQ: {:3}".format(data.iaq), end='\t')
    print("VoC: {:4.2f}".format(data.voc / (1 << 4) ) , end='\t')
    print("CO2: {:4.2f}".format(data.co2_equivalent/ (1 << 4)))

    
async def run():
    disconnect_event = asyncio.Event()
    def on_disconnect(_):
        print("Disconnected!")
        disconnect_event.set()

    scanner = BleakScanner(serivce_uuids=[SERVICE_UUID, ])
    device = await scanner.find_device_by_filter(lambda d, ad: SERVICE_UUID in ad.service_uuids, timeout = 200)

    async with BleakClient(device) as client:
        print(f"Connected: {client}")

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        client.set_disconnected_callback(on_disconnect)

        await disconnect_event.wait()



from bleak import BleakScanner

if __name__ == "__main__":
    import asyncio
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())