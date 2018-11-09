// testcreatefile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <winternl.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

const NTSTATUS STATUS_SUCCESS = 0;

void test_createfile(string filename)
{
   cout << "Filename " << filename << endl;
   HANDLE hFile = ::CreateFileA(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   cout << (hFile == INVALID_HANDLE_VALUE ? "Invalid" : "Valid");
   cout << endl;
}

extern "C" typedef NTSYSAPI NTSTATUS(NTAPI *PNtCreateFile)(
   PHANDLE            FileHandle,
   ACCESS_MASK        DesiredAccess,
   POBJECT_ATTRIBUTES ObjectAttributes,
   PIO_STATUS_BLOCK  IoStatusBlock,
   PLARGE_INTEGER     AllocationSize,
   ULONG              FileAttributes,
   ULONG              ShareAccess,
   ULONG              CreateDisposition,
   ULONG              CreateOptions,
   PVOID              EaBuffer,
   ULONG              EaLength
   );

extern "C" typedef NTSYSAPI VOID(NTAPI *PRtlInitUnicodeString)(
   PUNICODE_STRING DestinationString,
   PCWSTR SourceString
   );

void test_ntcreatefile(string filename)
{
   HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
   auto aNtCreateFile = (PNtCreateFile) GetProcAddress(hNtDll, "NtCreateFile");
   auto aRtlInitUnicodeString = (PRtlInitUnicodeString) GetProcAddress(hNtDll, "RtlInitUnicodeString");

   cout << "Filename " << filename << endl;
   PCWSTR filePath = L"\\Device\\HarddiskVolume6\\Temp\\test_file.txt";
   UNICODE_STRING unicodeString;
   aRtlInitUnicodeString(&unicodeString, filePath);

   OBJECT_ATTRIBUTES objAttribs = { 0 };
   InitializeObjectAttributes(&objAttribs, &unicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);

   const int allocSize = 2048;
   LARGE_INTEGER largeInteger;
   largeInteger.QuadPart = allocSize;

   IO_STATUS_BLOCK ioStatusBlock = { 0 };

   HANDLE hFile = INVALID_HANDLE_VALUE;
   NTSTATUS status = aNtCreateFile(&hFile, STANDARD_RIGHTS_ALL, &objAttribs, &ioStatusBlock, &largeInteger,
      FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_CREATE, FILE_NON_DIRECTORY_FILE, NULL, NULL);

   if (status != STATUS_SUCCESS)
   {
      std::cout << "Cannot create file, status: " << status << std::endl;
      return;
   }
   cout << (hFile == INVALID_HANDLE_VALUE ? "Invalid" : "Valid");
   cout << endl;

   //  -1073741766 (ERROR_PATH_NOT_FOUND) 
}



int main()
{
   test_createfile(R"/(D:\github\test_createfile_c_drive.txt)/");
   test_createfile(R"/(\Device\HarddiskVolume3\github\test_createfile_devices.txt)/");
   test_ntcreatefile(R"/(D:\github\test_ntcreatefile_c_drive.txt)/");
   test_ntcreatefile(R"/(\Device\HarddiskVolume3\github\test_ntcreatefile_devices.txt)/");
   return 0;
}

