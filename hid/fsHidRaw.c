#include <windows.h>
#include <wchar.h>
#include <stdio.h>

typedef struct _HIDD_ATTRIBUTES {
    DWORD   Size;
    WORD    VendorID;
    WORD    ProductID;
    WORD    VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;


wchar_t *Block;

// For the block ptr
int __fastcall sub_10004060(wchar_t *Source, int a2, int a3, int a4, int a5)
{
  wchar_t *v7; // eax
  wchar_t *v9; // ecx
  bool v10; // zf
  wchar_t Destination[512]; // [esp+8h] [ebp-404h] BYREF

  v7 = (wchar_t *)malloc(0x414u);
  dword_10020674 = v7;
  if ( !v7 )
  {
    _loaddll((char *)1);
    JUMPOUT(0x1000416B);
  }
  *((_DWORD *)v7 + 260) = 0;
  memset(Destination, 0, sizeof(Destination));
  wcscpy_s(Destination, 0x200u, Source);
  if ( _wcsupr_s(Destination, 0x200u) )
    return 0;
  wcscpy_s(dword_10020674, 0x200u, Destination);
  v9 = dword_10020674;
  v10 = Block == 0;
  *((_DWORD *)dword_10020674 + 257) = a3;
  *((_DWORD *)v9 + 258) = a4;
  *((_DWORD *)v9 + 256) = a2;
  *((_DWORD *)v9 + 259) = a5;
  if ( v10 )
    Block = v9;
  else
    *(_DWORD *)(dword_10020678 + 1040) = v9;
  dword_10020678 = (int)v9;
  return 1;
}


int __fastcall sub_10001390(LPCWSTR lpFileName, struct _HIDP_CAPS *a2, PHIDD_ATTRIBUTES Attributes, NTSTATUS *a4)
{
  int v4; // edi
  HANDLE FileW; // eax
  void *v6; // esi
  NTSTATUS Caps; // eax
  void *v9; // [esp-4h] [ebp-20h]
  PHIDP_PREPARSED_DATA PreparsedData; // [esp+14h] [ebp-8h] BYREF

  v4 = 0;
  PreparsedData = 0;
  FileW = CreateFileW(lpFileName, 0, 3u, 0, 3u, 0, 0);
  v6 = FileW;
  if ( FileW == (HANDLE)-1 || !FileW )
    return v4;
  if ( HidD_GetPreparsedData(FileW, &PreparsedData) )
  {
    Attributes->Size = 12;
    if ( !HidD_GetAttributes(v6, Attributes) )
    {
      v9 = v6;
LABEL_6:
      CloseHandle(v9);
      HidD_FreePreparsedData(PreparsedData);
      return 0;
    }
    Caps = HidP_GetCaps(PreparsedData, a2);
    v9 = v6;
    *a4 = Caps;
    if ( Caps != 1114112 )
      goto LABEL_6;
    CloseHandle(v6);
    HidD_FreePreparsedData(PreparsedData);
    return 1;
  }
  CloseHandle(v6);
  return 0;
}

BOOL OutputToDevice(int vendorId, int productId, int arg2, int arg3, const void* buffer, int bufferSize) {
    HIDD_ATTRIBUTES Attributes;
    DWORD NumberOfBytesWritten = 0;
    HANDLE hFile;
    wchar_t FileName[1024];
    wchar_t var_408[1024];
    int var_85C;
    int var_846, var_842;
    
    void* Block = Block; // Placeholder for the actual block reference.
    
    if (!Block) {
        return FALSE;
    }
    
    while (Block) {
        memset(FileName, 0, sizeof(FileName));
        memset(var_408, 0, sizeof(var_408));
        
        wcscpy_s(FileName, sizeof(FileName) / sizeof(wchar_t), (wchar_t*)Block);
        
        // Assuming sub_10001390 is some function that fills Attributes based on FileName.
        if (!sub_10001390(FileName, &var_85C, &Attributes, &var_846)) {
            return FALSE;
        }
        
        // Create the file
        hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return FALSE;
        }

        // Check the attributes
        if (Attributes.VendorID == vendorId &&
            Attributes.ProductID == productId &&
            var_846 == arg2 &&
            var_842 == arg3) {
            
            // Write to the file
            BOOL result = WriteFile(hFile, buffer, bufferSize, &NumberOfBytesWritten, NULL);
            
            // Log the result
            wchar_t logMessage[1024];
            wsprintfW(logMessage, L"Res = %02d, OutputToDevice() to VID = %04X, PID = %04X, Bytes Written = %d", 
                      result, Attributes.VendorID, Attributes.ProductID, NumberOfBytesWritten);
            
            CloseHandle(hFile);
            return result;
        }

        CloseHandle(hFile);
        
        // Move to the next block in the chain (block chaining logic from assembly)
        Block = ((void**)Block)[1];
    }

    return FALSE;
}


int __stdcall SetKbdFeatures(int a1, int a2, int a3, int a4, void *a5)
{
  wchar_t *v5; // esi
  int v6; // edi
  HANDLE FileW; // eax
  void *v8; // esi
  int v10; // [esp+10h] [ebp-858h] BYREF
  struct _HIDD_ATTRIBUTES Attributes; // [esp+14h] [ebp-854h] BYREF
  struct _HIDP_CAPS v12; // [esp+20h] [ebp-848h] BYREF
  WCHAR FileName[512]; // [esp+60h] [ebp-808h] BYREF
  WCHAR v14[514]; // [esp+460h] [ebp-408h] BYREF

  v5 = Block;
  v6 = 0;
  for ( dword_10020674 = Block; v5; dword_10020674 = v5 )
  {
    memset(FileName, 0, sizeof(FileName));
    memset(v14, 0, 0x400u);
    wcscpy_s(FileName, 0x200u, v5);
    if ( sub_10001390(FileName, &v12, &Attributes, &v10) )
    {
      FileW = CreateFileW(FileName, 0x40000000u, 3u, 0, 3u, 0, 0);
      v8 = FileW;
      if ( FileW != (HANDLE)-1 )
      {
        if ( FileW )
        {
          if ( v12.UsagePage == a3 && v12.Usage == a4 && Attributes.VendorID == a1 && Attributes.ProductID == a2 )
          {
            v6 = HidD_SetFeature(FileW, a5, v12.FeatureReportByteLength);
            memset(v14, 0, 0x400u);
            wsprintfW(
              v14,
              L"Res = %02d, SetKbdFeatures() to VID = %04x, PID = %04x, Ver = %04x, UsgPg = %04x, Usg = %04x\n",
              v6,
              Attributes.VendorID,
              Attributes.ProductID,
              Attributes.VersionNumber,
              v12.UsagePage,
              v12.Usage);
          }
          CloseHandle(v8);
        }
      }
    }
    v5 = (wchar_t *)*((_DWORD *)dword_10020674 + 260);
  }
  return v6;
}
