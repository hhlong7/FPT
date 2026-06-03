#include "fault_evidence_codec.h"
#include "bit_ops.h"

Std_ReturnType FaultEvidenceCodec_Encode(const FaultEvidence_t* evidence,
                                         uint8_t* out_data,
                                         uint8_t out_size)
{
    if ((evidence == 0) || (out_data == 0) || (out_size < FAULT_EVIDENCE_ENCODED_SIZE)) {
        return E_INVALID_PARAM;
    }

    out_data[0] = (uint8_t)evidence->fault_code;
    out_data[1] = (uint8_t)evidence->state_before_fault;
    out_data[2] = (uint8_t)((evidence->timestamp_ms >> 24U) & 0xFFU);
    out_data[3] = (uint8_t)((evidence->timestamp_ms >> 16U) & 0xFFU);
    out_data[4] = (uint8_t)((evidence->timestamp_ms >> 8U) & 0xFFU);
    out_data[5] = (uint8_t)(evidence->timestamp_ms & 0xFFU);
    out_data[6] = 0xA5U;
    out_data[7] = 0x5AU;

    return E_OK;
}

Std_ReturnType FaultEvidenceCodec_Decode(const uint8_t* data,
                                         uint8_t length,
                                         FaultEvidence_t* out_evidence)
{
    if ((data == 0) || (out_evidence == 0) || (length < FAULT_EVIDENCE_ENCODED_SIZE)) {
        return E_INVALID_PARAM;
    }

    out_evidence->fault_code = (FaultCode_t)data[0];
    out_evidence->state_before_fault = (SystemState_t)data[1];
    out_evidence->timestamp_ms = ((uint32_t)data[2] << 24U) |
                                 ((uint32_t)data[3] << 16U) |
                                 ((uint32_t)data[4] << 8U) |
                                 (uint32_t)data[5];

    return E_OK;
}
