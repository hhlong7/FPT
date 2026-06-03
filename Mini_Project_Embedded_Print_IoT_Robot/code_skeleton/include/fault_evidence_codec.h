#ifndef FAULT_EVIDENCE_CODEC_H
#define FAULT_EVIDENCE_CODEC_H

#include "app_types.h"
#include "std_types.h"

#define FAULT_EVIDENCE_ENCODED_SIZE  (8U)

Std_ReturnType FaultEvidenceCodec_Encode(const FaultEvidence_t* evidence,
                                         uint8_t* out_data,
                                         uint8_t out_size);

Std_ReturnType FaultEvidenceCodec_Decode(const uint8_t* data,
                                         uint8_t length,
                                         FaultEvidence_t* out_evidence);

#endif
