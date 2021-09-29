#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#pragma pack(push,8) // Make sure we have consistent structure packings
#ifdef __cplusplus
extern "C" {
#endif
#define MAKE_NVAPI_VERSION(typeName,ver) (NvU32)(sizeof(typeName) | ((ver)<<16))
#define GET_NVAPI_VERSION(ver) (NvU32)((ver)>>16)
#define GET_NVAPI_SIZE(ver) (NvU32)((ver) & 0xffff)
#define NVAPI_GENERIC_STRING_MAX    4096
#define NVAPI_LONG_STRING_MAX       256
#define NVAPI_SHORT_STRING_MAX      64
#define NVAPI_MAX_PHYSICAL_GPUS             64
#define NVAPI_MAX_LOGICAL_GPUS              64
#define NV_DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
/* 64-bit types for compilers that support them, plus some obsolete variants */
#if defined(__GNUC__) || defined(__arm) || defined(__IAR_SYSTEMS_ICC__) || defined(__ghs__) || defined(_WIN64)
typedef unsigned long long NvU64; /* 0 to 18446744073709551615          */
#else
typedef unsigned __int64   NvU64; /* 0 to 18446744073709551615              */
#endif

// mac os 32-bit still needs this
#if (defined(macintosh) || defined(__APPLE__)) && !defined(__LP64__)
typedef signed long        NvS32; /* -2147483648 to 2147483647               */
#else
typedef signed int         NvS32; /* -2147483648 to 2147483647               */
#endif

NV_DECLARE_HANDLE(NvLogicalGpuHandle);         // One or more physical GPUs acting in concert (SLI)
NV_DECLARE_HANDLE(NvPhysicalGpuHandle);        // A single physical GPU

typedef unsigned long    NvU32;
typedef unsigned short   NvU16;
typedef unsigned char    NvU8;

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
typedef struct NV_GPU_PERF_PSTATES20_PARAM_DELTA
{
    int value;

    struct
    {
        int min;
        int max;
    } valueRange;
} NV_GPU_PERF_PSTATES20_PARAM_DELTA;

typedef struct NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY
{
    unsigned int domainId;
    unsigned int editable : 1;
    unsigned int reserved : 31;
    unsigned int voltageUV;
    NV_GPU_PERF_PSTATES20_PARAM_DELTA voltageDeltaUV;
} NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY;

typedef struct NV_GPU_PSTATE20_CLOCK_ENTRY
{
    unsigned int domainId;
    unsigned int typeId;
    unsigned int editable : 1;
    unsigned int reserved : 31;
    NV_GPU_PERF_PSTATES20_PARAM_DELTA frequencyDeltaKHz;

    union
    {
        struct
        {
            unsigned int frequencyKHz;
        } single;

        struct
        {
            unsigned int minFrequencyKHz;
            unsigned int maxFrequencyKHz;
            unsigned int domainId;
            unsigned int minVoltageUV;
            unsigned int maxVoltageUV;
        } range;
    } data;
} NV_GPU_PSTATE20_CLOCK_ENTRY;
typedef struct NV_GPU_PERF_PSTATES20_INFO
{
    unsigned int version;
    unsigned int editable : 1;
    unsigned int reserved : 31;
    unsigned int numPStates;
    unsigned int numClocks;
    unsigned int numBaseVoltages;

    struct
    {
        unsigned int pStateId;
        unsigned int editable : 1;
        unsigned int reserved : 31;
        NV_GPU_PSTATE20_CLOCK_ENTRY clocks[8];
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY baseVoltages[4];
    } pStates[16];

    struct overVoltage
    {
        unsigned int numVoltages;
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY voltages[4];
    } overVoltage;
} NV_GPU_PERF_PSTATES20_INFO;
typedef struct NV_GPU_POWER_STATUS
{
    NvU32 version;
    NvU32 count;

    struct
    {
        NvU32 unknown1;
        NvU32 unknown2;
        NvU32 power;
        NvU32 unknown3;
    } entries[4];
} NV_GPU_POWER_STATUS;
#define NV_GPU_PERF_CLIENT_POWER_POLICIES_STATUS_VER MAKE_NVAPI_VERSION(NV_GPU_POWER_STATUS,1)
void* (__cdecl *NvAPI_QueryInterface)(unsigned int offset);
int (__cdecl *NvAPI_Initialize)();
int (__cdecl *NvAPI_Unload)();
int (__cdecl *NvAPI_EnumPhysicalGPUs)(NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *gpuCount);
int (__cdecl *NvAPI_GPU_GetBusId)(NvPhysicalGpuHandle gpuHandle, NvU32 *busId);
int (__cdecl *NvAPI_GPU_GetRamMaker)(NvPhysicalGpuHandle gpuHandle, NV_RAM_MAKER *maker);
int (__cdecl *NvAPI_GPU_GetFullName)(NvPhysicalGpuHandle gpuHandle, NvAPI_ShortString *name);
int (__cdecl *NvAPI_GPU_GetRamType)(NvPhysicalGpuHandle gpuHandle, NV_RAM_Type *type);
int (__cdecl *NvAPI_GPU_SetPstates20)(NvPhysicalGpuHandle gpuHandle, NV_GPU_PERF_PSTATES20_INFO *pStatesInfo);
int (__cdecl *NvAPI_GPU_ClientPowerPoliciesSetStatus)(NvPhysicalGpuHandle gpuHandle, NV_GPU_POWER_STATUS *powerStatus);

NvPhysicalGpuHandle NvPhysicalGpuHandle__[NVAPI_MAX_PHYSICAL_GPUS];


void *NvApiGpuHandles[128];
void *handles[64];

NvU32 GpuCount;
NvU32 BusId;

bool Initialize();
bool Unload();
bool EnumGpus();
bool GetBusId(OUT unsigned int* budid,int Index);
void GetAllBusId();
bool GetGpuInfo(OUT char *data, int Index);
int SetCoreClock(unsigned busId, int Clock);
int SetMemoryClock(unsigned busId, int Clock);
int SetPowerLimit(unsigned busId, unsigned int power);
bool SetOverClock(unsigned int busId, int Core, int Memory, unsigned int Power);

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
            NvAPI_EnumPhysicalGPUs = (int (__cdecl*)(NvPhysicalGpuHandle*, NvU32*)) NvAPI_QueryInterface(0xE5AC921F);
            NvAPI_GPU_GetBusId = (int (__cdecl *)(NvPhysicalGpuHandle, NvU32*)) NvAPI_QueryInterface(0x1BE0B8E5);
            NvAPI_GPU_GetRamMaker = (int (__cdecl *)(NvPhysicalGpuHandle, NV_RAM_MAKER* )) NvAPI_QueryInterface(0x42AEA16A);
            NvAPI_GPU_GetFullName = (int (__cdecl *)(NvPhysicalGpuHandle, NvAPI_ShortString*)) NvAPI_QueryInterface(0xCEEE8E9F);
            NvAPI_GPU_GetRamType =  (int (__cdecl *)(NvPhysicalGpuHandle, NV_RAM_Type*)) NvAPI_QueryInterface(0x57F7CAAC);
            NvAPI_GPU_SetPstates20 = (int (__cdecl*)(NvPhysicalGpuHandle, NV_GPU_PERF_PSTATES20_INFO*)) NvAPI_QueryInterface(0x0F4DAE6B);
            NvAPI_GPU_ClientPowerPoliciesSetStatus = (int (__cdecl*)(NvPhysicalGpuHandle, NV_GPU_POWER_STATUS*)) NvAPI_QueryInterface(0xAD95F5ED);
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
    result = NvAPI_EnumPhysicalGPUs(NvPhysicalGpuHandle__, &GpuCount);
    if (result == 0) {return true;}
    return false;
}

bool GetBusId(OUT unsigned int* budid, int Index) {
    if (!NvAPI_GPU_GetBusId) {return false;}
    int result = -1;
    result = NvAPI_GPU_GetBusId(NvPhysicalGpuHandle__[Index], (NvU32*)budid);
    if (result == 0) {
        NvPhysicalGpuHandle__[*budid] = NvPhysicalGpuHandle__[Index];
        return true;
    }
    printf("result : %d\n", result);
    return false;
}
void GetAllBusId() {
    if (!NvAPI_GPU_GetBusId) {return;}
    int result = -1;
    for (unsigned int i = 0; i < GpuCount; ++i) {
        result = NvAPI_GPU_GetBusId(NvPhysicalGpuHandle__[i], &BusId);
        if (result == 0) {
            NvPhysicalGpuHandle__[BusId] = NvPhysicalGpuHandle__[i];
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

    result = NvAPI_GPU_GetRamMaker(NvPhysicalGpuHandle__[Index], &ramMaker);
    if (result != 0) {
        return false;
    }
    result = NvAPI_GPU_GetRamType(NvPhysicalGpuHandle__[Index], &ramType);
    if (result != 0) {
        return false;
    }
    result = NvAPI_GPU_GetFullName(NvPhysicalGpuHandle__[Index], &nvApiShortString);
    if (result != 0) {
        return false;
    }
    sprintf(data, R"({"name":"%s", "maker":%d, "type":%d})", nvApiShortString, ramMaker, ramType);
    return true;
}
int SetCoreClock(unsigned int Busid, int Clock) {
    if (!NvAPI_GPU_SetPstates20) return false;
    NV_GPU_PERF_PSTATES20_INFO pStatesInfo = { 0 };
    pStatesInfo.version = MAKE_NVAPI_VERSION(pStatesInfo, 2);
    pStatesInfo.numPStates = 1;
    pStatesInfo.numClocks = 1;
    pStatesInfo.pStates[0].pStateId = 0;
    pStatesInfo.pStates[0].clocks[0].domainId = 0;
    pStatesInfo.pStates[0].clocks[0].typeId = 0;
    pStatesInfo.pStates[0].clocks[0].frequencyDeltaKHz.value = Clock * 1000;

    return NvAPI_GPU_SetPstates20(NvPhysicalGpuHandle__[Busid], &pStatesInfo);;
}
int SetMemoryClock(unsigned int Busid, int Clock) {
    if (!NvAPI_GPU_SetPstates20) return false;
    NV_GPU_PERF_PSTATES20_INFO pStatesInfo = { 0 };
    pStatesInfo.version = MAKE_NVAPI_VERSION(pStatesInfo, 2);
    pStatesInfo.numPStates = 1;
    pStatesInfo.numClocks = 1;
    pStatesInfo.pStates[0].pStateId = 0;
    pStatesInfo.pStates[0].clocks[0].domainId = 4;
    pStatesInfo.pStates[0].clocks[0].typeId = 0;
    pStatesInfo.pStates[0].clocks[0].frequencyDeltaKHz.value = Clock * 1000;


    return NvAPI_GPU_SetPstates20(NvPhysicalGpuHandle__[Busid], &pStatesInfo);;
}
int SetPowerLimit(unsigned int Busid, unsigned int power) {
    if (!NvAPI_GPU_ClientPowerPoliciesSetStatus) return false;
    NV_GPU_POWER_STATUS powerStatus = { 0 };

    powerStatus.version = NV_GPU_PERF_CLIENT_POWER_POLICIES_STATUS_VER;
    powerStatus.count = 1;
    powerStatus.entries[0].power = power * 1000;
    return NvAPI_GPU_ClientPowerPoliciesSetStatus(NvPhysicalGpuHandle__[Busid], &powerStatus);;
}

bool SetOverClock(unsigned int busId, int Core, int Memory, unsigned int Power) {

    int core = SetCoreClock(busId, Core);
    int memory = SetMemoryClock(busId, Memory);
    int power = SetPowerLimit(busId, Power);
    if (core != 0) {return false;}
    if (memory != 0) {return false;}
    if (power != 0) {return false;}
    return true;
}
