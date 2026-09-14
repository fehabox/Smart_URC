# @fehabox Smart URC Pro --- User Guide

## 1. What is Smart URC Pro?

https://play.google.com/apps/internaltest/4700536097818201780

@fehabox Smart URC Pro is a universal communication and command-learning
remote controller for compatible equipment.

It can communicate through:

-   USB Serial
-   Bluetooth Classic SPP
-   Wi-Fi / TCP

It is device-agnostic: the application is not tied to one radio,
instrument, or controller. A compatible device can provide command
information, or commands can be entered manually.

Smart URC Pro is the paid Pro edition. Smart URC Free is a separate USB
Serial edition.

------------------------------------------------------------------------

## 2. First start

1.  Install Smart URC Pro.
2.  Connect or prepare the equipment you want to control.
3.  Open Smart URC Pro.
4.  Open the communication menu using the menu button at the left of the
    header.
5.  Select the required transport:
    -   USB
    -   Bluetooth
    -   Wi-Fi
6.  Select/connect the desired device.
7.  Confirm that the response area shows communication responses.

The application uses one common command path for USB, Bluetooth and
Wi-Fi, so the same command can normally be tested over different
transports when the device supports them.

------------------------------------------------------------------------

# 3. USB Serial

## How to connect

1.  Connect the USB Serial device to the Android device.
2.  Open the communication menu.
3.  Select **USB**.
4.  Select the detected USB device if required.
5.  Connect.
6.  Send a simple test command such as:

``` text
PING
```

A compatible device should answer:

``` text
PONG
```

### USB settings

The reference/test environment uses:

``` text
9600 baud
8 data bits
No parity
1 stop bit
```

The USB line protocol is command-per-line: terminate commands with
Enter/newline.

------------------------------------------------------------------------

# 4. Bluetooth Classic SPP

Smart URC Pro supports Bluetooth Classic Serial Port Profile (SPP).

## How to connect

1.  Turn on Bluetooth on the Android device.
2.  Make sure the target device is powered and discoverable/pairable.
3.  Pair the Android device with the target when required by Android.
4.  Open the Smart URC communication menu.
5.  Select **Bluetooth**.
6.  Select the required paired device from the scrollable device list.
7.  Connect.
8.  Test with:

``` text
PING
```

Expected response:

``` text
PONG
```

The Pro Bluetooth connection supports common SPP connection fallbacks
for compatible devices.

------------------------------------------------------------------------

# 5. Wi-Fi / TCP

Smart URC Pro can communicate with equipment using TCP/IP.

## Direct connection

If the device provides an IP address and TCP port:

1.  Connect the Android device to the same network, or to the device's
    Wi-Fi access point.
2.  Select **Wi-Fi**.
3.  Enter/select the device hostname or IP address.
4.  Enter the TCP port.
5.  Connect.
6.  Test with:

``` text
PING
```

Expected response:

``` text
PONG
```

## Wi-Fi discovery

The Pro discovery mechanism uses UDP port:

``` text
4210
```

The application sends:

``` text
SMARTURC-DISCOVER
```

A compatible device may advertise information such as:

``` json
{"hostname":"VFO-Studio","ip":"192.168.4.1","port":5000}
```

The UDP discovery port and the TCP control port are different. For
example, discovery may use UDP 4210 while the actual control connection
uses TCP 5000.

------------------------------------------------------------------------

# 6. Device list

The communication menu provides a unified device-selection area for the
available transports.

Depending on the connection type you can select:

-   USB device
-   Bluetooth device
-   Wi-Fi device

For Bluetooth, the device list is scrollable so multiple paired devices
can be displayed.

------------------------------------------------------------------------

# 7. Sending a command manually

The command field can be used to test equipment directly.

Example:

``` text
PING
```

Press **SEND**.

For a frequency command:

``` text
SETFREQ 14205000
```

A compatible device may respond:

``` text
FREQ 14205000
```

The exact commands and responses depend on the connected device.

------------------------------------------------------------------------

# 8. LEARN / HELP

One of the main Pro features is command learning.

A compatible device can provide a HELP response describing the commands
it supports.

Typical workflow:

``` text
CONNECT
   ↓
LEARN / HELP
   ↓
device returns command information
   ↓
Smart URC interprets the definitions
   ↓
local command menu is created
```

After learning, the command controls can be used instead of manually
typing every command.

For a device that does not provide a compatible HELP description, use
**Manual Learning**.

------------------------------------------------------------------------

# 9. Manual Learning

Manual Learning is intended for custom equipment and development
projects.

Use it when:

-   the device has no HELP function
-   the device's HELP format is not supported
-   you already have a command list
-   you are developing a new ESP32 or other controller

Enter or paste the command definitions using the format supported by the
current Smart URC Pro release.

The application can then create a local command menu from the learned
information.

------------------------------------------------------------------------

# 10. Commands with parameters

Smart URC Pro supports commands that require values.

Examples include:

``` text
SETFREQ <frequency>
SETBFO <frequency>
SETBRIGHTNESS <0-255>
SETDESIGN <slot>
SETCAL <ppb>
```

For commands with defined choices, the application can guide the user to
valid values.

Example:

``` text
SETMODE <CW|LSB|USB|VBFO|AM|FM>
```

The user can select an appropriate mode rather than manually
constructing the entire command.

------------------------------------------------------------------------

# 11. Saving a learned menu

After a command menu has been learned or manually created:

1.  Verify the commands.
2.  Use **SAVE** in the local-menu area.
3.  Save the current learned menu.
4.  The saved menu can later be reloaded instead of learning it again.

The local menu is separate from the connected device itself.

------------------------------------------------------------------------

# 12. LOGIN

Smart URC Pro supports a universal LOGIN command interface for
compatible devices.

Use:

``` text
LOGIN <username> <password>
```

The application provides a username/password dialog and masks the
password field.

The connected device remains responsible for deciding whether the
credentials are valid and what access they provide.

## Saved credentials

If the user chooses to save credentials, Smart URC Pro protects the
locally stored credentials using Android Keystore-backed encryption.

------------------------------------------------------------------------

# 13. Response window

The response area shows communication received from the connected
device.

Use the **CLEAR** control when you want to remove previous responses and
start a clean test.

This is particularly useful during development and troubleshooting.

------------------------------------------------------------------------

# 14. Recommended first test

For a new compatible device:

1.  Connect using USB, Bluetooth or Wi-Fi.
2.  Send:

``` text
PING
```

3.  Confirm:

``` text
PONG
```

4.  Send:

``` text
STATUS
```

5.  Send:

``` text
HELP
```

6.  If the device supports Smart URC learning, use **LEARN / HELP**.
7.  Test one read command.
8.  Test one write/set command.
9.  Save the learned menu.

------------------------------------------------------------------------

# 15. Troubleshooting

## No USB device

-   Check the USB cable/adapter.
-   Check Android USB permissions.
-   Check that the hardware provides a compatible USB Serial interface.
-   Disconnect/reconnect the adapter and try again.

## Bluetooth device not shown

-   Confirm Bluetooth is enabled.
-   Pair the device in Android first when required.
-   Make sure the target uses Bluetooth Classic SPP.
-   Refresh/select the correct device in the Smart URC Bluetooth list.

## Wi-Fi does not connect

-   Confirm the Android device and target are on the correct network.
-   Check the IP address.
-   Check the TCP port.
-   Remember that UDP discovery port 4210 is not the TCP control port.
-   Try a direct IP/TCP connection if discovery is unavailable.

## PING works but a command does not

This normally means communication is working but the command is not
implemented or is formatted differently by the target.

Check the device command specification and its expected line
termination.

## LEARN does not create the expected menu

Check that the device's HELP output follows the command-definition
format expected by the Smart URC Pro release. Manual Learning can be
used when the device uses a different format.

------------------------------------------------------------------------

# 16. Example command session

A simple compatible test device may accept:

``` text
PING
PONG

GETFREQ
FREQ 14205000

SETFREQ 14300000
FREQ 14300000

GETMODE
MODE USB

SETMODE CW
MODE CW

GETBAND
BAND 20m
```

The exact command set is device-dependent.

------------------------------------------------------------------------

# 17. Pro vs Free

### Smart URC Free

-   Separate free edition
-   Basic USB Serial functionality

### Smart URC Pro

-   USB Serial
-   Bluetooth Classic SPP
-   Wi-Fi / TCP
-   Unified device selection
-   Wi-Fi discovery
-   Command learning
-   Manual learning
-   Parameter-aware commands
-   Saved learned menus
-   LOGIN
-   Protected local credentials
-   Response console
-   No advertisements

Smart URC Free and Smart URC Pro are separate editions.
