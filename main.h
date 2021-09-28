#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define MAKE_NVAPI_VERSION(type, version) ((unsigned int)(sizeof(type) | ((version) << 16)))
#define NVAPI_GENERIC_STRING_MAX    4096
#define NVAPI_LONG_STRING_MAX       256
#define NVAPI_SHORT_STRING_MAX      64
typedef char NvAPI_String[NVAPI_GENERIC_STRING_MAX];
typedef char NvAPI_LongString[NVAPI_LONG_STRING_MAX];
typedef char NvAPI_ShortString[NVAPI_SHORT_STRING_MAX];
typedef enum NV_RAM_MAKER
{
    MAKER_NONE,
    MAKER_SAMSUNG,
    MAKER_QIMONDA,
    MAKER_ELPIDA,
    MAKER_ETRON,
    MAKER_NANYA,
    MAKER_HYNIX,
    MAKER_MOSEL,
    MAKER_WINBOND,
    MAKER_ELITE,
    MAKER_MICRON
} NV_RAM_MAKER;
typedef enum NV_RAM_Type
{
    Unkown,
    SDRAM,
    DDR1,
    DDR2,
    GDDR2,
    GDDR3,
    GDDR4,
    DDR3,
    GDDR5,
    LPDDR2,
    GDDR5X
} NV_RAM_Type;

void* (__cdecl *NvAPI_QueryInterface)(unsigned int offset);
int (__cdecl *NvAPI_Initialize)();
int (__cdecl *NvAPI_Unload)();
int (__cdecl *NvAPI_EnumPhysicalGPUs)(void **gpuHandles, unsigned int *gpuCount);
int (__cdecl *NvAPI_GPU_GetBusId)(void *gpuHandle, unsigned int *busId);
int (__cdecl *NvAPI_GPU_GetRamMaker)(void *gpuHandle, NV_RAM_MAKER *maker);
int (__cdecl *NvAPI_GPU_GetFullName)(void *gpuHandle, NvAPI_ShortString *name);
int (__cdecl *NvAPI_GPU_GetRamType)(void *gpuHandle, NV_RAM_Type *type);

void *NvApiGpuHandles[128];
void *handles[64];

unsigned int GpuCount;
unsigned int BusId;

bool Initialize();
bool Unload();
bool EnumGpus();
bool GetBusId(unsigned int budid,int Index);
void GetAllBusId();
bool GetGpuInfo(OUT char *data, int Index);

#ifdef __cplusplus
}
#endif

bool Initialize() {
    int result = -1;
    HMODULE nvapi = 0;
#if defined _WIN64
    char nvapiDllName[] = "nvapi64.dll";
#else
    char nvapiDllName[] = "nvapi.dll";
#endif

    nvapi = LoadLibraryA(nvapiDllName);
    result = !(nvapi != 0);
    if (result == 0)
    {
        NvAPI_QueryInterface = (void* (__cdecl*)(unsigned int)) GetProcAddress(nvapi, "nvapi_QueryInterface");
        result = !(NvAPI_QueryInterface != 0);
        if (result == 0)
        {
            NvAPI_Initialize = (int (__cdecl *)()) NvAPI_QueryInterface(0x0150E828);
            NvAPI_Unload = (int (__cdecl*)()) NvAPI_QueryInterface(0xD22BDD7E);
            NvAPI_EnumPhysicalGPUs = (int (__cdecl*)(void**, unsigned int*)) NvAPI_QueryInterface(0xE5AC921F);
            NvAPI_GPU_GetBusId = (int (__cdecl *)(void*, unsigned int*)) NvAPI_QueryInterface(0x1BE0B8E5);
            NvAPI_GPU_GetRamMaker = (int (__cdecl *)(void*, NV_RAM_MAKER* )) NvAPI_QueryInterface(0x42AEA16A);
            NvAPI_GPU_GetFullName = (int (__cdecl *)(void*, NvAPI_ShortString*)) NvAPI_QueryInterface(0xCEEE8E9F);
            NvAPI_GPU_GetRamType =  (int (__cdecl *)(void*, NV_RAM_Type*)) NvAPI_QueryInterface(0x57F7CAAC);
        }
    }

    if (!NvAPI_Initialize) {return false;}
    result = NvAPI_Initialize();
    if (result == 0)  return true;
    return false;
}

bool Unload() {
    int result = -1;
    if (!NvAPI_Unload) {return false;}
    result = NvAPI_Unload();
    if (result == 0) {return true;}
    return false;
}

bool EnumGpus() {
    int result = -1;
    if (!NvAPI_EnumPhysicalGPUs) {return false;}
    result = NvAPI_EnumPhysicalGPUs(handles, &GpuCount);
    if (result == 0) {return true;}
    return false;
}

bool GetBusId(unsigned int budid, int Index) {
    if (!NvAPI_GPU_GetBusId) {return false;}
    int result = -1;
    result = NvAPI_GPU_GetBusId(handles[Index], &budid);
    if (result == 0) {
        NvApiGpuHandles[BusId] = handles[Index];
        return true;
    }
    return false;
}
void GetAllBusId() {
    if (!NvAPI_GPU_GetBusId) {return;}
    int result = -1;
    for (unsigned int i = 0; i < GpuCount; ++i) {
        result = NvAPI_GPU_GetBusId(handles[i], &BusId);
        if (result == 0) {
            NvApiGpuHandles[BusId] = handles[i];
        }
    }
}
bool GetGpuInfo(OUT char *data,  int Index) {
    if (!NvAPI_GPU_GetRamMaker) {return false;}
    if (!NvAPI_GPU_GetRamType) {return false;}
    if (!NvAPI_GPU_GetFullName) {return false;}

    int result = -1;

    NV_RAM_MAKER ramMaker;
    NV_RAM_Type ramType;
    NvAPI_ShortString nvApiShortString;

    result = NvAPI_GPU_GetRamMaker(handles[Index], &ramMaker);
    if (result != 0) {
        return false;
    }
    result = NvAPI_GPU_GetRamType(handles[Index], &ramType);
    if (result != 0) {
        return false;
    }
    result = NvAPI_GPU_GetFullName(handles[Index], &nvApiShortString);
    if (result != 0) {
        return false;
    }
    sprintf(data, R"({"name":"%s", "maker":%d, "type":%d})", nvApiShortString, ramMaker, ramType);
    return true;
}
