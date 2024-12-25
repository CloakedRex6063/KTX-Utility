#pragma once
#include "string_view"

// Inspired From https://github.com/KhronosGroup/KTX-Software/

namespace KTX
{
    enum class KtxCreateFlags
    {
        eNone = 0, // Only loads info about the ktx texture
        eLoadImageData = 1 // Loads the entire image into memory
    };
    
    void LoadKTXFromFile(std::string_view fileName, KtxCreateFlags flags = KtxCreateFlags::eNone);
}
