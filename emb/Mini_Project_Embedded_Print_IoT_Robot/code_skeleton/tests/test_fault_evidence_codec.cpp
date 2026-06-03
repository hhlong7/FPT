#include <cassert>
extern "C" {
#include "fault_evidence_codec.h"
}

int main()
{
    FaultEvidence_t in{};
    FaultEvidence_t out{};
    uint8_t raw[FAULT_EVIDENCE_ENCODED_SIZE] = {0U};

    in.fault_code = FAULT_COMM_TIMEOUT;
    in.state_before_fault = SYSTEM_STATE_ACTIVE;
    in.timestamp_ms = 0x12345678UL;

    assert(FaultEvidenceCodec_Encode(&in, raw, FAULT_EVIDENCE_ENCODED_SIZE) == E_OK);
    assert(raw[0] == (uint8_t)FAULT_COMM_TIMEOUT);
    assert(raw[1] == (uint8_t)SYSTEM_STATE_ACTIVE);
    assert(raw[6] == 0xA5U);
    assert(raw[7] == 0x5AU);

    assert(FaultEvidenceCodec_Decode(raw, FAULT_EVIDENCE_ENCODED_SIZE, &out) == E_OK);
    assert(out.fault_code == FAULT_COMM_TIMEOUT);
    assert(out.state_before_fault == SYSTEM_STATE_ACTIVE);
    assert(out.timestamp_ms == 0x12345678UL);

    assert(FaultEvidenceCodec_Encode(0, raw, FAULT_EVIDENCE_ENCODED_SIZE) == E_INVALID_PARAM);
    assert(FaultEvidenceCodec_Decode(0, FAULT_EVIDENCE_ENCODED_SIZE, &out) == E_INVALID_PARAM);
    assert(FaultEvidenceCodec_Decode(raw, 4U, &out) == E_INVALID_PARAM);

    return 0;
}
