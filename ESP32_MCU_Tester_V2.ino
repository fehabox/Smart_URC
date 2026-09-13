/*
 * ESP32 MCU TESTER V2
 * -------------------
 * Generic communication test firmware for @fehabox Smart URC.
 *
 * Author:
 * Femi Hasani / @fehabox
 * http://fehabox.com
 *
 * Copyright (c) 2026 Femi Hasani / @fehabox
 *
 * This software is provided as an open reference/test project for
 * development and testing of compatible communication equipment.
 *
 * License:
 * Creative Commons Attribution 4.0 International (CC BY 4.0)
 *
 * You are free to share and adapt this software, provided that
 * appropriate credit is given to the original author.
 *
 * License details:
 * https://creativecommons.org/licenses/by/4.0/
 */

/*
  ESP32 MCU TESTER V2
  -------------------
  Generic communication reference/test firmware for classic ESP32.

  Transports:
    1. USB Serial      : 9600 baud
    2. Bluetooth SPP   : ESP32_MCU_TEST
    3. Wi-Fi AP + TCP  : SSID ESP32_MCU_TEST, TCP port 5000

  IMPORTANT:
    - No unsolicited Serial/BT/TCP output.
    - The ESP32 only responds after receiving a command.
    - All transports use the same command parser and command set.
    - Designed as a developer reference for the Android Remote App.

  Wi-Fi:
    AP address: 192.168.4.1
    TCP port:   5000
    Password:   12345678

  Bluetooth:
    Classic Bluetooth SPP device name: ESP32_MCU_TEST
*/

#include <Arduino.h>
#include <WiFi.h>
#include "BluetoothSerial.h"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

static const uint32_t SERIAL_BAUD = 9600;

static const char *BT_NAME = "ESP32_MCU_TEST";

static const char *WIFI_SSID = "ESP32_MCU_TEST";
static const char *WIFI_PASSWORD = "12345678";
static const uint16_t TCP_PORT = 5000;

WiFiServer tcpServer(TCP_PORT);
BluetoothSerial SerialBT;

// -----------------------------------------------------------------------------
// Simulated device state
// -----------------------------------------------------------------------------

uint32_t testFrequency = 14205000UL;
long testBfo = 9000000L;
String testBand = "20m";
String testMode = "USB";
uint8_t testBrightness = 100;
int testDesign = 0;
long testCalibration = 0;

// -----------------------------------------------------------------------------
// Line buffers
// -----------------------------------------------------------------------------

static const size_t MAX_LINE = 160;

String usbLine;
String btLine;
String tcpLine;

WiFiClient tcpClient;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

void sendLine(Print &out, const String &s)
{
  out.println(s);
}

bool parseFrequency(const char *text, uint32_t &value)
{
  if (!text) return false;

  uint64_t v = 0;
  bool hasDigit = false;

  while (*text)
  {
    char c = *text++;

    if (c >= '0' && c <= '9')
    {
      hasDigit = true;
      v = v * 10ULL + (uint64_t)(c - '0');

      if (v > 4294967295ULL)
        return false;
    }
    else if (c == '.' || c == ',' || c == ' ' || c == '\t')
    {
      // Allowed formatting characters.
    }
    else
    {
      return false;
    }
  }

  if (!hasDigit) return false;

  value = (uint32_t)v;
  return true;
}

bool parseLongValue(const char *text, long &value)
{
  if (!text) return false;

  char *endPtr = nullptr;
  long v = strtol(text, &endPtr, 10);

  if (endPtr == text || *endPtr != '\0')
    return false;

  value = v;
  return true;
}

String detectBand(uint32_t freq)
{
  // Specific bands first.
  if (freq >= 1800000UL  && freq <= 2000000UL)  return "160m";
  if (freq >= 3500000UL  && freq <= 4000000UL)  return "80m";
  if (freq >= 7000000UL  && freq <= 7300000UL)  return "40m";
  if (freq >= 10100000UL && freq <= 10150000UL) return "30m";
  if (freq >= 14000000UL && freq <= 14350000UL) return "20m";
  if (freq >= 18100000UL && freq <= 18168000UL) return "17m";
  if (freq >= 21000000UL && freq <= 21450000UL) return "15m";
  if (freq >= 24900000UL && freq <= 24990000UL) return "12m";
  if (freq >= 28000000UL && freq <= 29700000UL) return "10m";
  if (freq >= 26965000UL && freq <= 27405000UL) return "CB";

  // VHF test range.
  if (freq >= 60000000UL && freq <= 160000000UL) return "VHF";

  // Generic 0.5-30 MHz fallback.
  if (freq >= 500000UL && freq <= 30000000UL) return "0.5-30 MHz";

  return "";
}

void printHelp(Print &out)
{
  sendLine(out, "HELP");
  sendLine(out, "GETFREQ");
  sendLine(out, "SETFREQ <frequency>");
  sendLine(out, "GETBFO");
  sendLine(out, "SETBFO <frequency>");
  sendLine(out, "SAVEBFO <frequency>");
  sendLine(out, "GETMODE");
  sendLine(out, "SETMODE <CW|LSB|USB|VBFO|AM|FM>");
  sendLine(out, "GETBAND");
  sendLine(out, "NEXTBAND");
  sendLine(out, "PREVBAND");
  sendLine(out, "GETDESIGN");
  sendLine(out, "SETDESIGN <slot>");
  sendLine(out, "STATUS");
  sendLine(out, "GETBRIGHTNESS");
  sendLine(out, "SETBRIGHTNESS <0-255>");
  sendLine(out, "SETCAL <ppb>");
  sendLine(out, "GETCAL");
  sendLine(out, "RESETCAL");
  sendLine(out, "FREQ2CAL <measured frequency>");
  sendLine(out, "READCAL");
  sendLine(out, "SAVECAL");
  sendLine(out, "APPLYCAL");
  sendLine(out, "PING");
  sendLine(out, "METER <scale> <0-100>");
  sendLine(out, "ECHO <text>");
}

void printStatus(Print &out)
{
  sendLine(out, "STATUS");
  sendLine(out, "FREQ " + String(testFrequency));
  sendLine(out, "BFO " + String(testBfo));
  sendLine(out, "MODE " + testMode);
  sendLine(out, "BAND " + testBand);
  sendLine(out, "DESIGN " + String(testDesign));
  sendLine(out, "BRIGHTNESS " + String(testBrightness));
  sendLine(out, "CAL " + String(testCalibration));
  sendLine(out, "BT ESP32_MCU_TEST");
  sendLine(out, "WIFI " + String(WiFi.softAPIP()));
  sendLine(out, "TCP " + String(TCP_PORT));
}

void processCommand(String line, Print &out)
{
  line.trim();

  if (line.length() == 0)
    return;

  // strtok requires a writable buffer.
  char buffer[MAX_LINE];
  line.toCharArray(buffer, sizeof(buffer));

  char *command = strtok(buffer, " \t");

  if (!command)
    return;

  if (!strcasecmp(command, "PING"))
  {
    sendLine(out, "PONG");
    return;
  }

  if (!strcasecmp(command, "HELP"))
  {
    printHelp(out);
    return;
  }

  if (!strcasecmp(command, "STATUS"))
  {
    printStatus(out);
    return;
  }

  if (!strcasecmp(command, "GETFREQ"))
  {
    sendLine(out, "FREQ " + String(testFrequency));
    return;
  }

  if (!strcasecmp(command, "SETFREQ"))
  {
    char *arg = strtok(nullptr, "");

    uint32_t freq;
    if (!arg || !parseFrequency(arg, freq))
    {
      sendLine(out, "ERROR SETFREQ <frequency>");
      return;
    }

    String band = detectBand(freq);

    if (band.length() == 0)
    {
      sendLine(out, "ERROR NO BAND");
      return;
    }

    testFrequency = freq;
    testBand = band;

    // One response only.
    sendLine(out, "FREQ " + String(testFrequency));
    return;
  }

  if (!strcasecmp(command, "GETBFO"))
  {
    sendLine(out, "BFO " + String(testBfo));
    return;
  }

  if (!strcasecmp(command, "SETBFO"))
  {
    char *arg = strtok(nullptr, "");

    uint32_t freq;
    if (!arg || !parseFrequency(arg, freq))
    {
      sendLine(out, "ERROR SETBFO <frequency>");
      return;
    }

    testBfo = (long)freq;
    sendLine(out, "BFO " + String(testBfo));
    return;
  }

  if (!strcasecmp(command, "SAVEBFO"))
  {
    char *arg = strtok(nullptr, "");

    uint32_t freq;
    if (!arg || !parseFrequency(arg, freq))
    {
      sendLine(out, "ERROR SAVEBFO <frequency>");
      return;
    }

    testBfo = (long)freq;
    sendLine(out, "BFO " + String(testBfo));
    return;
  }

  if (!strcasecmp(command, "GETMODE"))
  {
    sendLine(out, "MODE " + testMode);
    return;
  }

  if (!strcasecmp(command, "SETMODE"))
  {
    char *arg = strtok(nullptr, " \t");

    if (!arg)
    {
      sendLine(out, "ERROR SETMODE <mode>");
      return;
    }

    String mode = arg;
    mode.toUpperCase();

    if (mode != "CW" &&
        mode != "LSB" &&
        mode != "USB" &&
        mode != "VBFO" &&
        mode != "AM" &&
        mode != "FM")
    {
      sendLine(out, "ERROR MODE");
      return;
    }

    testMode = mode;
    sendLine(out, "MODE " + testMode);
    return;
  }

  if (!strcasecmp(command, "GETBAND"))
  {
    sendLine(out, "BAND " + testBand);
    return;
  }

  if (!strcasecmp(command, "NEXTBAND"))
  {
    sendLine(out, "BAND NEXT");
    return;
  }

  if (!strcasecmp(command, "PREVBAND"))
  {
    sendLine(out, "BAND PREVIOUS");
    return;
  }

  if (!strcasecmp(command, "GETDESIGN"))
  {
    sendLine(out, "DESIGN " + String(testDesign));
    return;
  }

  if (!strcasecmp(command, "SETDESIGN"))
  {
    char *arg = strtok(nullptr, " \t");

    long value;
    if (!arg || !parseLongValue(arg, value))
    {
      sendLine(out, "ERROR SETDESIGN <slot>");
      return;
    }

    testDesign = (int)value;
    sendLine(out, "DESIGN " + String(testDesign));
    return;
  }

  if (!strcasecmp(command, "GETBRIGHTNESS"))
  {
    sendLine(out, "BRIGHTNESS " + String(testBrightness));
    return;
  }

  if (!strcasecmp(command, "SETBRIGHTNESS"))
  {
    char *arg = strtok(nullptr, " \t");

    long value;
    if (!arg || !parseLongValue(arg, value) || value < 0 || value > 255)
    {
      sendLine(out, "ERROR SETBRIGHTNESS <0-255>");
      return;
    }

    testBrightness = (uint8_t)value;
    sendLine(out, "BRIGHTNESS " + String(testBrightness));
    return;
  }

  if (!strcasecmp(command, "SETCAL"))
  {
    char *arg = strtok(nullptr, " \t");

    long value;
    if (!arg || !parseLongValue(arg, value))
    {
      sendLine(out, "ERROR SETCAL <ppb>");
      return;
    }

    testCalibration = value;
    sendLine(out, "CAL " + String(testCalibration));
    return;
  }

  if (!strcasecmp(command, "GETCAL") ||
      !strcasecmp(command, "READCAL"))
  {
    sendLine(out, "CAL " + String(testCalibration));
    return;
  }

  if (!strcasecmp(command, "RESETCAL"))
  {
    testCalibration = 0;
    sendLine(out, "CAL 0");
    return;
  }

  if (!strcasecmp(command, "FREQ2CAL"))
  {
    char *arg = strtok(nullptr, "");

    uint32_t measured;
    if (!arg || !parseFrequency(arg, measured))
    {
      sendLine(out, "ERROR FREQ2CAL <measured frequency>");
      return;
    }

    // Simulation only.
    sendLine(out, "CAL 0");
    return;
  }

  if (!strcasecmp(command, "SAVECAL"))
  {
    sendLine(out, "CAL SAVED");
    return;
  }

  if (!strcasecmp(command, "APPLYCAL"))
  {
    sendLine(out, "CAL APPLIED");
    return;
  }

  if (!strcasecmp(command, "METER"))
  {
    char *scale = strtok(nullptr, " \t");
    char *value = strtok(nullptr, " \t");

    if (!scale || !value)
    {
      sendLine(out, "ERROR METER <scale> <0-100>");
      return;
    }

    long meterValue;
    if (!parseLongValue(value, meterValue) || meterValue < 0 || meterValue > 100)
    {
      sendLine(out, "ERROR METER VALUE");
      return;
    }

    // Simulation only.
    sendLine(out, "METER " + String(scale) + " " + String(meterValue));
    return;
  }

  if (!strcasecmp(command, "ECHO"))
  {
    char *arg = strtok(nullptr, "");
    if (!arg)
    {
      sendLine(out, "ERROR ECHO <text>");
      return;
    }

    sendLine(out, String("ECHO ") + arg);
    return;
  }

  sendLine(out, String("UNKNOWN ") + command);
}

// -----------------------------------------------------------------------------
// Transport handling
// -----------------------------------------------------------------------------

void readUSB()
{
  while (Serial.available())
  {
    char c = (char)Serial.read();

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      processCommand(usbLine, Serial);
      usbLine = "";
    }
    else if (usbLine.length() < MAX_LINE - 1)
    {
      usbLine += c;
    }
    else
    {
      usbLine = "";
      Serial.println("ERROR LINE TOO LONG");
    }
  }
}

void readBluetooth()
{
  while (SerialBT.available())
  {
    char c = (char)SerialBT.read();

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      processCommand(btLine, SerialBT);
      btLine = "";
    }
    else if (btLine.length() < MAX_LINE - 1)
    {
      btLine += c;
    }
    else
    {
      btLine = "";
      SerialBT.println("ERROR LINE TOO LONG");
    }
  }
}

void acceptTCP()
{
  if (!tcpClient || !tcpClient.connected())
  {
    WiFiClient candidate = tcpServer.available();

    if (candidate)
    {
      tcpClient = candidate;
      tcpLine = "";
    }
  }
}

void readTCP()
{
  if (!tcpClient || !tcpClient.connected())
    return;

  while (tcpClient.available())
  {
    char c = (char)tcpClient.read();

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      processCommand(tcpLine, tcpClient);
      tcpLine = "";
    }
    else if (tcpLine.length() < MAX_LINE - 1)
    {
      tcpLine += c;
    }
    else
    {
      tcpLine = "";
      tcpClient.println("ERROR LINE TOO LONG");
    }
  }
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup()
{
  Serial.begin(SERIAL_BAUD);

  SerialBT.begin(BT_NAME);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

  tcpServer.begin();

  // DELIBERATELY NO OUTPUT HERE.
  // The tester must remain silent until a command is received.
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------

void loop()
{
  readUSB();
  readBluetooth();

  acceptTCP();
  readTCP();

  // No continuous Serial/BT/Wi-Fi output.
}
