# tp2-driver-reversal
Windows drivers firmware findings for Thinkpad Trackpoint Keyboard II.  
You can find the official software being looked at here: [https://support.lenovo.com/au/en/accessories/trackpoint_keyboard](https://support.lenovo.com/au/en/accessories/trackpoint_keyboard)

## findings
### scrolling
scrolling related functionality can be found within the logonset files.  
it looks like its mostly a wrapper around the hid interface sending these bytes:
```c
var_14 = 0;
memset(var_1C, 0, sizeof(var_1C));
var_1C[0] = 0x18;   // Set lower byte
var_1C[1] = 0x09;   // Set higher byte
var_1C[2] = input_char;  // Set input character into third byte
OutputToDevice(0x17EF, 0x60E1, 0xFF01, var_1C); // vendor id, product id probably, ... buffer(var_1c)
```
and
```c
var_8 = 0;
memset(var_10, 0, sizeof(var_10));
var_10[0] = 0x13;   // Set lower byte
var_10[1] = 0x09;   // Set higher byte
var_10[2] = input_char;  // Set input character into third byte
SetKbdFeatures(0x17EF, 0x60EE, 0x0C, var_10); // vendor id, product ..., buffer(var_10)
```