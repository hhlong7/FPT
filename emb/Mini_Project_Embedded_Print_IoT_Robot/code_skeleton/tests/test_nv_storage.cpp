#include <cassert>
extern "C" {
#include "nv_storage.h"
}

int main()
{
    NvConfigBlock_t block{};
    Std_ReturnType status;

    NvStorage_Init();
    status = NvStorage_LoadConfig(&block);
    assert(status == E_OK);
    assert(block.version == 1U);
    assert(block.length == NV_CONFIG_DATA_LENGTH);

    block.data[0] = 0x5AU;
    status = NvStorage_SaveConfig(&block);
    assert(status == E_OK);

    status = NvStorage_LoadConfig(&block);
    assert(status == E_OK);
    assert(block.data[0] == 0x5AU);

    return 0;
}
