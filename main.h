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
    unsigned int version;
    unsigned int count;

    struct
    {
        unsigned int unknown1;
        unsigned int unknown2;
        unsigned int power;
        unsigned int unknown3;
    } entries[4];
} NV_GPU_POWER_STATUS;

void* (__cdecl *NvAPI_QueryInterface)(unsigned int offset);
int (__cdecl *NvAPI_Initialize)();
int (__cdecl *NvAPI_Unload)();
int (__cdecl *NvAPI_EnumPhysicalGPUs)(void **gpuHandles, unsigned int *gpuCount);
int (__cdecl *NvAPI_GPU_GetBusId)(void *gpuHandle, unsigned int *busId);
int (__cdecl *NvAPI_GPU_GetRamMaker)(void *gpuHandle, NV_RAM_MAKER *maker);
int (__cdecl *NvAPI_GPU_GetFullName)(void *gpuHandle, NvAPI_ShortString *name);
int (__cdecl *NvAPI_GPU_GetRamType)(void *gpuHandle, NV_RAM_Type *type);
int (__cdecl *NvAPI_GPU_SetPstates20)(void *gpuHandle, NV_GPU_PERF_PSTATES20_INFO *pStatesInfo);
int (__cdecl *NvAPI_GPU_ClientPowerPoliciesSetStatus)(void *gpuHandle, NV_GPU_POWER_STATUS *powerStatus);

void *NvApiGpuHandles[128];
void *handles[64];

unsigned int GpuCount;
unsigned int BusId;

bool Initialize();
bool Unload();
bool EnumGpus();
bool GetBusId(OUT unsigned int* budid,int Index);
void GetAllBusId();
bool GetGpuInfo(OUT char *data, int Index);
bool SetCoreClock(unsigned busId, int Clock);
bool SetMemoryClock(unsigned busId, int Clock);
bool SetPowerLimit(unsigned busId, unsigned int power);
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
            NvAPI_EnumPhysicalGPUs = (int (__cdecl*)(void**, unsigned int*)) NvAPI_QueryInterface(0xE5AC921F);
            NvAPI_GPU_GetBusId = (int (__cdecl *)(void*, unsigned int*)) NvAPI_QueryInterface(0x1BE0B8E5);
            NvAPI_GPU_GetRamMaker = (int (__cdecl *)(void*, NV_RAM_MAKER* )) NvAPI_QueryInterface(0x42AEA16A);
            NvAPI_GPU_GetFullName = (int (__cdecl *)(void*, NvAPI_ShortString*)) NvAPI_QueryInterface(0xCEEE8E9F);
            NvAPI_GPU_GetRamType =  (int (__cdecl *)(void*, NV_RAM_Type*)) NvAPI_QueryInterface(0x57F7CAAC);
            NvAPI_GPU_SetPstates20 = (int (__cdecl*)(void*, NV_GPU_PERF_PSTATES20_INFO*)) NvAPI_QueryInterface(0x0F4DAE6B);
            NvAPI_GPU_ClientPowerPoliciesSetStatus = (int (__cdecl*)(void*, NV_GPU_POWER_STATUS*)) NvAPI_QueryInterface(0xAD95F5ED);
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

bool GetBusId(OUT unsigned int* budid, int Index) {
    if (!NvAPI_GPU_GetBusId) {return false;}
    int result = -1;
    result = NvAPI_GPU_GetBusId(handles[Index], budid);
    if (result == 0) {
        NvApiGpuHandles[*budid] = handles[Index];
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
bool SetCoreClock(unsigned int Busid, int Clock) {
    if (!NvAPI_GPU_SetPstates20) return false;
    int result = -1;
    NV_GPU_PERF_PSTATES20_INFO pStatesInfo;
    pStatesInfo.version = MAKE_NVAPI_VERSION(pStatesInfo, 2);
    pStatesInfo.numPStates = 1;
    pStatesInfo.numClocks = 1;
    pStatesInfo.pStates[0].pStateId = 0;
    pStatesInfo.pStates[0].clocks[0].domainId = 0;
    pStatesInfo.pStates[0].clocks[0].typeId = 0;
    pStatesInfo.pStates[0].clocks[0].frequencyDeltaKHz.value = Clock * 1000;
    result = NvAPI_GPU_SetPstates20(NvApiGpuHandles[Busid], &pStatesInfo);
//    printf("core overclock : %d\n", result);
    if (result == 0) {
        return true;
    }
    return false;
}
bool SetMemoryClock(unsigned int Busid, int Clock) {
    if (!NvAPI_GPU_SetPstates20) return false;
    int result = -1;
    NV_GPU_PERF_PSTATES20_INFO pStatesInfo;
    pStatesInfo.version = MAKE_NVAPI_VERSION(pStatesInfo, 2);
    pStatesInfo.numPStates = 1;
    pStatesInfo.numClocks = 1;
    pStatesInfo.pStates[0].pStateId = 0;
    pStatesInfo.pStates[0].clocks[0].domainId = 4;
    pStatesInfo.pStates[0].clocks[0].typeId = 0;
    pStatesInfo.pStates[0].clocks[0].frequencyDeltaKHz.value = Clock * 1000;

    result = NvAPI_GPU_SetPstates20(NvApiGpuHandles[Busid], &pStatesInfo);
    if (result != 0) {
        return false;
    }
    return true;
}
bool SetPowerLimit(unsigned int Busid, unsigned int power) {
    if (!NvAPI_GPU_ClientPowerPoliciesSetStatus) return false;
    int result = -1;
    NV_GPU_POWER_STATUS powerStatus;

    powerStatus.version = MAKE_NVAPI_VERSION(powerStatus, 1);
    powerStatus.count = 1;
    powerStatus.entries[0].unknown1 = 0;
    powerStatus.entries[0].power = power * 1000;
    result = NvAPI_GPU_ClientPowerPoliciesSetStatus(NvApiGpuHandles[Busid], &powerStatus);
    if (result != 0) {
        return false;
    }
    return true;

}

bool SetOverClock(unsigned int busId, int Core, int Memory, unsigned int Power) {
printf("1\n");
    if (!SetCoreClock(busId, Core)) {return false;}
    printf("2\n");
    if (!SetMemoryClock(busId, Memory)) {return false;}
    printf("3\n");
    if (!SetPowerLimit(busId, Power)) {return false;}
    printf("4\n");
    return true;
}
