# @fehabox Smart URC Pro --- ESP32 Developer Guide

## 1. Purpose

This guide explains how an ESP32 developer can build, test and integrate
a custom device with @fehabox Smart URC Pro.

The supplied reference firmware is:

``` text
ESP32_MCU_Tester_V2.ino
```

It is a communication tester and reference implementation for the
classic ESP32.

The tester implements the same command parser over:

-   USB Serial
-   Bluetooth Classic SPP
-   Wi-Fi TCP

The goal is to let a developer verify Smart URC communication before
connecting a real project.

------------------------------------------------------------------------

# 2. ESP32 MCU Tester V2

The supplied tester uses:

``` text
USB Serial      9600 baud
Bluetooth SPP   ESP32_MCU_TEST
Wi-Fi AP        ESP32_MCU_TEST
TCP             port 5000
Wi-Fi AP IP     192.168.4.1
Wi-Fi password  12345678
```

Important design characteristic:

> The tester sends no unsolicited communication.

It remains silent until it receives a command.

This makes it useful for testing applications because every response can
be associated with a command sent by Smart URC.

------------------------------------------------------------------------

# 3. Hardware

Use a classic ESP32 board with:

-   USB connection
-   Bluetooth Classic support
-   Wi-Fi support

The supplied sketch uses:

``` cpp
#include <Arduino.h>
#include <WiFi.h>
#include "BluetoothSerial.h"
```

`BluetoothSerial` means this reference is intended for ESP32 variants
that support Bluetooth Classic SPP. It is not a generic Bluetooth Low
Energy example.

------------------------------------------------------------------------

# 4. Flashing the tester

## Arduino IDE

1.  Install the ESP32 board package.
2.  Select the appropriate classic ESP32 board.
3.  Connect the ESP32 by USB.
4.  Select the correct COM port.
5.  Open:

``` text
ESP32_MCU_Tester_V2.ino
```

6.  Compile.
7.  Upload.

After boot, the tester deliberately produces no startup text.

That is normal.

------------------------------------------------------------------------

# 5. USB Serial test

Connect the ESP32 USB port to the Android device using a suitable USB
OTG connection.

In Smart URC Pro:

1.  Open the communication menu.
2.  Select USB.
3.  Select the ESP32 USB Serial device.
4.  Connect.
5.  Send:

``` text
PING
```

Expected response:

``` text
PONG
```

Then test:

``` text
GETFREQ
```

Expected initial response:

``` text
FREQ 14205000
```

------------------------------------------------------------------------

# 6. Bluetooth SPP test

The tester starts Classic Bluetooth using the name:

``` text
ESP32_MCU_TEST
```

On Android:

1.  Enable Bluetooth.
2.  Pair with `ESP32_MCU_TEST` if Android requires pairing.
3.  Open Smart URC Pro.
4.  Open the communication menu.
5.  Select Bluetooth.
6.  Select `ESP32_MCU_TEST`.
7.  Connect.
8.  Send:

``` text
PING
```

Expected:

``` text
PONG
```

Then:

``` text
STATUS
```

------------------------------------------------------------------------

# 7. Wi-Fi test

The tester creates its own Wi-Fi access point.

SSID:

``` text
ESP32_MCU_TEST
```

Password:

``` text
12345678
```

The tester uses:

``` text
IP:   192.168.4.1
TCP:  5000
```

Connect the Android device to this Wi-Fi network.

In Smart URC Pro configure/connect to:

``` text
192.168.4.1
port 5000
```

Then send:

``` text
PING
```

Expected:

``` text
PONG
```

------------------------------------------------------------------------

# 8. Wi-Fi discovery

Smart URC Pro uses UDP port 4210 for discovery.

The discovery probe is:

``` text
SMARTURC-DISCOVER
```

The supplied ESP32 tester V2 is primarily a TCP/command transport
tester. Its Wi-Fi control service is:

``` text
TCP 5000
```

If automatic discovery is being tested, the target firmware should
additionally implement the Smart URC discovery advertisement.

A compatible advertisement can be:

``` json
{"hostname":"VFO-Studio","ip":"192.168.4.1","port":5000}
```

or:

``` text
HOSTNAME=VFO-Studio IP=192.168.4.1 PORT=5000
```

Discovery and control are separate:

``` text
UDP 4210  → discovery
TCP 5000  → commands
```

------------------------------------------------------------------------

# 9. The command parser

All three transports use the same parser:

``` text
USB ───────┐
Bluetooth ─┼──> processCommand() ──> response
TCP ───────┘
```

This is an important design pattern for ESP32 projects.

The command implementation does not need to be duplicated for each
transport.

A developer can therefore test a command over USB first and then test
exactly the same command over Bluetooth and Wi-Fi.

------------------------------------------------------------------------

# 10. Complete tester command list

The supplied V2 tester supports:

``` text
HELP
GETFREQ
SETFREQ <frequency>

GETBFO
SETBFO <frequency>
SAVEBFO <frequency>

GETMODE
SETMODE <CW|LSB|USB|VBFO|AM|FM>

GETBAND
NEXTBAND
PREVBAND

GETDESIGN
SETDESIGN <slot>

STATUS

GETBRIGHTNESS
SETBRIGHTNESS <0-255>

SETCAL <ppb>
GETCAL
RESETCAL
FREQ2CAL <measured frequency>
READCAL
SAVECAL
APPLYCAL

PING
METER <scale> <0-100>
ECHO <text>
```

------------------------------------------------------------------------

# 11. Basic command examples

## PING

Send:

``` text
PING
```

Response:

``` text
PONG
```

This is the best first communication test.

------------------------------------------------------------------------

## Frequency

Send:

``` text
GETFREQ
```

Response:

``` text
FREQ 14205000
```

Set frequency:

``` text
SETFREQ 14300000
```

Response:

``` text
FREQ 14300000
```

The tester also updates its simulated band state.

------------------------------------------------------------------------

## BFO

Read:

``` text
GETBFO
```

Set:

``` text
SETBFO 9001000
```

Save:

``` text
SAVEBFO 9001000
```

The tester is simulating the state; SAVEBFO does not implement
non-volatile storage.

------------------------------------------------------------------------

## Mode

Read:

``` text
GETMODE
```

Set:

``` text
SETMODE LSB
```

Valid modes:

``` text
CW
LSB
USB
VBFO
AM
FM
```

------------------------------------------------------------------------

## Band

Read:

``` text
GETBAND
```

Next:

``` text
NEXTBAND
```

Previous:

``` text
PREVBAND
```

The tester returns:

``` text
BAND NEXT
```

or:

``` text
BAND PREVIOUS
```

These are reference responses rather than a full band-navigation
implementation.

------------------------------------------------------------------------

# 12. Design slot

Read:

``` text
GETDESIGN
```

Set:

``` text
SETDESIGN 3
```

Response:

``` text
DESIGN 3
```

The tester stores the value in RAM only.

------------------------------------------------------------------------

# 13. Brightness

Read:

``` text
GETBRIGHTNESS
```

Set:

``` text
SETBRIGHTNESS 150
```

Valid range:

``` text
0-255
```

Example response:

``` text
BRIGHTNESS 150
```

------------------------------------------------------------------------

# 14. Calibration

Read:

``` text
GETCAL
```

Set:

``` text
SETCAL 25
```

Reset:

``` text
RESETCAL
```

The tester also accepts:

``` text
READCAL
SAVECAL
APPLYCAL
FREQ2CAL <measured frequency>
```

These are reference/test commands.

`FREQ2CAL` currently returns:

``` text
CAL 0
```

because the calculation is deliberately simulated.

`SAVECAL` and `APPLYCAL` return confirmation text but do not implement
persistent calibration storage.

------------------------------------------------------------------------

# 15. Meter testing

Send:

``` text
METER S 75
```

Response:

``` text
METER S 75
```

The value must be between 0 and 100.

Examples:

``` text
METER S 0
METER S 50
METER S 100
```

This is useful for testing a Smart URC control that sends a meter value.

------------------------------------------------------------------------

# 16. ECHO

Send:

``` text
ECHO Hello Smart URC
```

Response:

``` text
ECHO Hello Smart URC
```

ECHO is useful for transport and text-buffer testing.

------------------------------------------------------------------------

# 17. STATUS

Send:

``` text
STATUS
```

The tester returns several lines describing its simulated state,
including:

``` text
STATUS
FREQ 14205000
BFO 9000000
MODE USB
BAND 20m
DESIGN 0
BRIGHTNESS 100
CAL 0
BT ESP32_MCU_TEST
WIFI 192.168.4.1
TCP 5000
```

This is a useful multi-line response test.

------------------------------------------------------------------------

# 18. HELP

Send:

``` text
HELP
```

The tester returns the supported command names and argument forms.

This is useful for verifying the Smart URC HELP/LEARN communication
path.

The tester's HELP output is a reference command list. A production
device can provide richer command-definition information if its Smart
URC integration requires parameters, labels, choices or ranges.

------------------------------------------------------------------------

# 19. Error handling

Invalid commands return an explicit response.

Example:

``` text
ABC
```

returns:

``` text
UNKNOWN ABC
```

Invalid SETFREQ:

``` text
SETFREQ TEST
```

returns:

``` text
ERROR SETFREQ <frequency>
```

Invalid mode:

``` text
SETMODE XYZ
```

returns:

``` text
ERROR MODE
```

Invalid brightness:

``` text
SETBRIGHTNESS 300
```

returns:

``` text
ERROR SETBRIGHTNESS <0-255>
```

This makes the tester useful for testing application error handling as
well as successful commands.

------------------------------------------------------------------------

# 20. Line protocol

Commands are line based.

The tester:

-   ignores carriage return (`\r`)
-   processes a command when newline (`\n`) is received
-   limits input lines to 159 characters
-   returns `ERROR LINE TOO LONG` if the buffer is exceeded

A developer should therefore send:

``` text
COMMAND\n
```

or a normal line ending containing newline.

There is no continuous telemetry stream in the tester.

------------------------------------------------------------------------

# 21. No unsolicited output

This is deliberate.

The tester does NOT print startup messages such as:

``` text
ESP32 started
WiFi connected
Bluetooth ready
```

unless those messages are responses to a command.

This prevents debug text from being mixed into the application
communication channel.

For a real Smart URC device, unsolicited/debug output should also be
kept away from the command transport unless the protocol explicitly
supports it.

------------------------------------------------------------------------

# 22. Building a real ESP32 device

The easiest development path is:

``` text
ESP32 MCU Tester V2
        ↓
learn/test communication
        ↓
replace simulated state
        ↓
connect real hardware
        ↓
keep same command protocol
        ↓
test with Smart URC Pro
```

For example, replace:

``` cpp
testFrequency
```

with the actual transceiver frequency variable or hardware driver.

Keep:

``` text
GETFREQ
SETFREQ
```

as the external protocol.

This allows the Android application to remain unchanged while the ESP32
implementation becomes real.

------------------------------------------------------------------------

# 23. Recommended development sequence

### Stage 1 --- Transport

Test:

``` text
PING
```

over:

``` text
USB
Bluetooth
Wi-Fi
```

### Stage 2 --- Read commands

Test:

``` text
GETFREQ
GETBFO
GETMODE
GETBAND
STATUS
```

### Stage 3 --- Write commands

Test:

``` text
SETFREQ
SETBFO
SETMODE
SETBRIGHTNESS
SETDESIGN
```

### Stage 4 --- Parameters

Test invalid and valid values.

### Stage 5 --- HELP / learning

Test the device command description and Smart URC learning.

### Stage 6 --- Real hardware

Replace the simulated variables with actual hardware functions.

### Stage 7 --- Production firmware

Remove development-only commands such as ECHO where appropriate and
implement the final device protocol.

------------------------------------------------------------------------

# 24. Developer checklist

Before declaring an ESP32 device Smart URC compatible:

-   [ ] USB Serial responds correctly
-   [ ] Bluetooth Classic SPP responds correctly, if supported
-   [ ] Wi-Fi TCP responds correctly, if supported
-   [ ] PING returns PONG
-   [ ] Commands use newline termination
-   [ ] No unwanted debug output is mixed into the protocol
-   [ ] GET commands return predictable responses
-   [ ] SET commands return confirmation or explicit errors
-   [ ] Parameter ranges are validated
-   [ ] HELP/learning is tested
-   [ ] LOGIN behavior is tested if used
-   [ ] Wi-Fi discovery is tested if implemented
-   [ ] Multiple-line responses are handled correctly
-   [ ] The Android response window is checked
-   [ ] The same command set is verified on each supported transport

------------------------------------------------------------------------

# 25. Example ESP32 project architecture

A production project can follow this pattern:

``` text
                Smart URC Pro
                     │
          ┌──────────┼──────────┐
          │          │          │
         USB        BT         TCP
          │          │          │
          └──────────┼──────────┘
                     ↓
             Command Receiver
                     ↓
              Command Parser
                     ↓
             Command Handlers
                     ↓
          ┌──────────┼──────────┐
          │          │          │
       Radio       Display    Calibration
       Driver       Driver       Driver
```

The important principle is to keep the transport layer separate from the
command implementation.

------------------------------------------------------------------------

# 26. Reference firmware status

`ESP32_MCU_Tester_V2.ino` is a **developer/test reference**, not a
complete transceiver firmware.

Several functions intentionally simulate hardware state.

Examples:

-   frequency
-   BFO
-   calibration
-   brightness
-   design slot
-   meter

The purpose is to provide a predictable target for testing Smart URC Pro
communication and command handling.

------------------------------------------------------------------------

# 27. 73 --- Build your own device

Smart URC Pro is designed so that a developer does not need to wait for
the Android application to support a specific equipment model.

If your ESP32 project exposes a compatible command interface, Smart URC
can become its remote-control and development interface.

Start with:

``` text
PING
```

Then:

``` text
STATUS
```

Then:

``` text
HELP
```

Then implement your real commands one at a time.

73 --- @fehabox
