#include <windows.h>
#include <stdio.h>
#include <string.h>


//-------------------------------------------------------------------------------
// Dispatch for setting scroll
//-------------------------------------------------------------------------------
// calls imported from fsHidRaw.dll  
void SetKbdFeatures(unsigned int, unsigned int, unsigned int, unsigned char*);
void OutputToDevice(unsigned int, unsigned int, unsigned int, unsigned char*);
void OutputDebugStringW(LPCWSTR lpOutputString);

DWORD __security_cookie = 0x12345678;
void __security_check_cookie(DWORD cookie) {
    if (cookie != __security_cookie) {
        printf("Stack cookie corruption detected!\n");
        exit(1);
    }
}

// Function implementing the assembly logic
unsigned int hid_scroll(char input_char) {
    unsigned int stack_cookie;
    DWORD tick_start, tick_end;
    unsigned char var_1C[8] = {0};
    unsigned char var_10[8] = {0};
    char var_8 = 0, var_14 = 0;
    
    stack_cookie = __security_cookie ^ (unsigned int)&stack_cookie;

    var_8 = 0;
    memset(var_10, 0, sizeof(var_10));
    var_10[0] = 0x13;   // Set lower byte
    var_10[1] = 0x09;   // Set higher byte
    var_10[2] = input_char;  // Set input character into third byte
    SetKbdFeatures(0x17EF, 0x60EE, 0x0C, var_10);

    // wait 50ms
    tick_start = GetTickCount();
    do {
        tick_end = GetTickCount();
    } while ((tick_end - tick_start) < 50);

    // call OutputToDevice
    var_14 = 0;
    memset(var_1C, 0, sizeof(var_1C));
    var_1C[0] = 0x18;   // Set lower byte
    var_1C[1] = 0x09;   // Set higher byte
    var_1C[2] = input_char;  // Set input character into third byte

    OutputToDevice(0x17EF, 0x60E1, 0xFF01, var_1C);

    OutputDebugStringW(L"SetScroll\n");

    // 50ms wait
    tick_start = GetTickCount();
    do {
        tick_end = GetTickCount();
    } while ((tick_end - tick_start) < 50);

    // check stack cookie for corruption
    __security_check_cookie(stack_cookie);

    return 0;
}
