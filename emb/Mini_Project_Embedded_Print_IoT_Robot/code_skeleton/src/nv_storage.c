#include "nv_storage.h"

static NvConfigBlock_t g_nv_config;
static FaultEvidence_t g_fault_evidence;

uint16_t NvStorage_CalculateCrc16(const uint8_t* data, uint16_t length)
{
    uint16_t crc = 0xFFFFU;
    uint16_t index;

    for (index = 0U; index < length; ++index) {
        crc ^= data[index];
        crc = (uint16_t)((crc >> 1U) | (crc << 15U));
    }

    return crc;
}

void NvStorage_Init(void)
{
    g_nv_config.version = 1U;
    g_nv_config.length = NV_CONFIG_DATA_LENGTH;
    g_nv_config.crc16 = NvStorage_CalculateCrc16(g_nv_config.data, NV_CONFIG_DATA_LENGTH);
}

Std_ReturnType NvStorage_LoadConfig(NvConfigBlock_t* block)
{
    uint16_t crc;

    if (block == 0) {
        return E_INVALID_PARAM;
    }

    crc = NvStorage_CalculateCrc16(g_nv_config.data, g_nv_config.length);
    if (crc != g_nv_config.crc16) {
        return E_CRC_FAIL;
    }

    *block = g_nv_config;
    return E_OK;
}

Std_ReturnType NvStorage_SaveConfig(const NvConfigBlock_t* block)
{
    if (block == 0) {
        return E_INVALID_PARAM;
    }

    g_nv_config = *block;
    g_nv_config.crc16 = NvStorage_CalculateCrc16(g_nv_config.data, g_nv_config.length);
    return E_OK;
}

Std_ReturnType NvStorage_SaveFaultEvidence(const FaultEvidence_t* evidence)
{
    if (evidence == 0) {
        return E_INVALID_PARAM;
    }

    g_fault_evidence = *evidence;
    return E_OK;
}
