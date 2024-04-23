#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

#define SERVICE_UUID "b42e4a8e-ade7-11e4-89d3-123b93f75cba"
#define CHAR_UUID "b42e4dcc-ade7-11e4-89d3-123b93f75cba"
#define SERIAL_NUMBER 2950026145

struct CurrentValues
{
  float humidity;
  uint16_t radonSTA;
  uint16_t radonLTA;
  float temperature;

  static CurrentValues fromBytes(uint8_t *data)
  {
    uint8_t version = data[0];

    if (version == 1)
    {
      uint8_t humidity = data[1];
      uint16_t radonSTA = data[5] << 8 | data[4];
      uint16_t radonLTA = data[7] << 8 | data[6];
      uint16_t temperature = data[9] << 8 | data[8];

      return {
          humidity / 2.0f,
          radonSTA,
          radonLTA,
          temperature / 100.0f,
      };
    }
    return {};
  }
};

class Wave2
{
private:
  uint16_t id;
  uint32_t serialNumber;
  BLEClient *pClient;
  BLERemoteCharacteristic *pCharacteristic;
  std::string macAddress;

public:
  Wave2(uint32_t serial) : serialNumber(serial), pClient(nullptr), pCharacteristic(nullptr) {}

  bool isConnected()
  {
    return pClient != nullptr && pClient->isConnected();
  }

  std::string discover()
  {
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    BLEScanResults foundDevices = pBLEScan->start(3);
    for (int i = 0; i < foundDevices.getCount(); i++)
    {
      BLEAdvertisedDevice device = foundDevices.getDevice(i);

      uint8_t *mfrData;
      mfrData = (uint8_t *)device.getManufacturerData().data();

      if (parseSerialNumber(mfrData) == serialNumber)
      {
        Serial.println("######################################################################");
        Serial.println("Found device with matching serial number");
        Serial.print("ID: ");
        Serial.print(id);
        Serial.print(", SN: ");
        Serial.print(serialNumber);
        Serial.print(", ");
        Serial.println(device.toString().c_str());
        Serial.println("######################################################################");

        return device.getAddress().toString();
      }
    }
    return "";
  }

  void connect(int retries = 1)
  {
    int tries = 0;
    while (!isConnected() && tries < retries)
    {
      tries++;
      if (macAddress.empty())
      {
        macAddress = discover();
      }
      if (!macAddress.empty())
      {
        pClient = BLEDevice::createClient();
        pClient->connect(BLEAddress(macAddress));
        BLERemoteService *pService = pClient->getService(SERVICE_UUID);
        if (pService != nullptr)
        {
          pCharacteristic = pService->getCharacteristic(CHAR_UUID);
        }
      }
    }
  }

  void disconnect()
  {
    if (pClient != nullptr)
    {
      pClient->disconnect();
      pClient = nullptr;
      pCharacteristic = nullptr;
    }
  }

  CurrentValues read()
  {
    if (pCharacteristic != nullptr)
    {
      pCharacteristic->readValue();
      uint8_t *data = pCharacteristic->readRawData();
      if (data != nullptr)
      {
        return CurrentValues::fromBytes(data);
      }
    }
    return CurrentValues();
  }

private:
  uint32_t parseSerialNumber(uint8_t *MFRdata)
  {
    id = (MFRdata[1] << 8) | MFRdata[0];
    serialNumber = (MFRdata[5] << 24) | (MFRdata[4] << 16) | (MFRdata[3] << 8) | MFRdata[2];

    if (id == 0x0334)
    {
      Serial.print("ID: ");
      Serial.println(id);
      Serial.print("Serial number: ");
      Serial.println(serialNumber);
      return serialNumber;
    }
    else
    {
      return 0; // Return 0 for non-Airthings devices
    }
  }
};

Wave2 wave2(SERIAL_NUMBER);
const int SAMPLE_PERIOD = 5; // Time in seconds between reading the current values

void setup()
{
  Serial.begin(115200);
  BLEDevice::init("");
}

void loop()
{
  wave2.connect(3);
  CurrentValues currentValues = wave2.read();

  Serial.print("Humidity: ");
  Serial.print(currentValues.humidity);
  Serial.print(" %rH, Temperature: ");
  Serial.print(currentValues.temperature);
  Serial.print(" *C, Radon STA: ");
  Serial.print(currentValues.radonSTA);
  Serial.print(" Bq/m3, Radon LTA: ");
  Serial.print(currentValues.radonLTA);
  Serial.println(" Bq/m3");

  wave2.disconnect();
  delay(SAMPLE_PERIOD * 1000);
}