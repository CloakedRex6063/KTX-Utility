#include "KtxUtility.hpp"

#include <span>
#include <vector>

#include "GL_Format.hpp"
#include "array"
#include "cassert"
#include "fstream"
#include "iostream"
#include "variant"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

constexpr std::array ktxIdentifier{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

constexpr u32 ktxHeaderSize = 64;

constexpr std::array ktx2Identifier{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};
constexpr u32 ktx2HeaderSize = 80;

constexpr u32 endianRef = 0x04030201;
constexpr u32 endianRefRev = 0x01020304;

namespace
{
    enum class KtxUtility_VkFormat
    {
        VK_FORMAT_UNDEFINED = 0,
        VK_FORMAT_R4G4_UNORM_PACK8 = 1,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
        VK_FORMAT_R5G6B5_UNORM_PACK16 = 4,
        VK_FORMAT_B5G6R5_UNORM_PACK16 = 5,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
        VK_FORMAT_R8_UNORM = 9,
        VK_FORMAT_R8_SNORM = 10,
        VK_FORMAT_R8_USCALED = 11,
        VK_FORMAT_R8_SSCALED = 12,
        VK_FORMAT_R8_UINT = 13,
        VK_FORMAT_R8_SINT = 14,
        VK_FORMAT_R8_SRGB = 15,
        VK_FORMAT_R8G8_UNORM = 16,
        VK_FORMAT_R8G8_SNORM = 17,
        VK_FORMAT_R8G8_USCALED = 18,
        VK_FORMAT_R8G8_SSCALED = 19,
        VK_FORMAT_R8G8_UINT = 20,
        VK_FORMAT_R8G8_SINT = 21,
        VK_FORMAT_R8G8_SRGB = 22,
        VK_FORMAT_R8G8B8_UNORM = 23,
        VK_FORMAT_R8G8B8_SNORM = 24,
        VK_FORMAT_R8G8B8_USCALED = 25,
        VK_FORMAT_R8G8B8_SSCALED = 26,
        VK_FORMAT_R8G8B8_UINT = 27,
        VK_FORMAT_R8G8B8_SINT = 28,
        VK_FORMAT_R8G8B8_SRGB = 29,
        VK_FORMAT_B8G8R8_UNORM = 30,
        VK_FORMAT_B8G8R8_SNORM = 31,
        VK_FORMAT_B8G8R8_USCALED = 32,
        VK_FORMAT_B8G8R8_SSCALED = 33,
        VK_FORMAT_B8G8R8_UINT = 34,
        VK_FORMAT_B8G8R8_SINT = 35,
        VK_FORMAT_B8G8R8_SRGB = 36,
        VK_FORMAT_R8G8B8A8_UNORM = 37,
        VK_FORMAT_R8G8B8A8_SNORM = 38,
        VK_FORMAT_R8G8B8A8_USCALED = 39,
        VK_FORMAT_R8G8B8A8_SSCALED = 40,
        VK_FORMAT_R8G8B8A8_UINT = 41,
        VK_FORMAT_R8G8B8A8_SINT = 42,
        VK_FORMAT_R8G8B8A8_SRGB = 43,
        VK_FORMAT_B8G8R8A8_UNORM = 44,
        VK_FORMAT_B8G8R8A8_SNORM = 45,
        VK_FORMAT_B8G8R8A8_USCALED = 46,
        VK_FORMAT_B8G8R8A8_SSCALED = 47,
        VK_FORMAT_B8G8R8A8_UINT = 48,
        VK_FORMAT_B8G8R8A8_SINT = 49,
        VK_FORMAT_B8G8R8A8_SRGB = 50,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
        VK_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
        VK_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
        VK_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
        VK_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
        VK_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
        VK_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
        VK_FORMAT_R16_UNORM = 70,
        VK_FORMAT_R16_SNORM = 71,
        VK_FORMAT_R16_USCALED = 72,
        VK_FORMAT_R16_SSCALED = 73,
        VK_FORMAT_R16_UINT = 74,
        VK_FORMAT_R16_SINT = 75,
        VK_FORMAT_R16_SFLOAT = 76,
        VK_FORMAT_R16G16_UNORM = 77,
        VK_FORMAT_R16G16_SNORM = 78,
        VK_FORMAT_R16G16_USCALED = 79,
        VK_FORMAT_R16G16_SSCALED = 80,
        VK_FORMAT_R16G16_UINT = 81,
        VK_FORMAT_R16G16_SINT = 82,
        VK_FORMAT_R16G16_SFLOAT = 83,
        VK_FORMAT_R16G16B16_UNORM = 84,
        VK_FORMAT_R16G16B16_SNORM = 85,
        VK_FORMAT_R16G16B16_USCALED = 86,
        VK_FORMAT_R16G16B16_SSCALED = 87,
        VK_FORMAT_R16G16B16_UINT = 88,
        VK_FORMAT_R16G16B16_SINT = 89,
        VK_FORMAT_R16G16B16_SFLOAT = 90,
        VK_FORMAT_R16G16B16A16_UNORM = 91,
        VK_FORMAT_R16G16B16A16_SNORM = 92,
        VK_FORMAT_R16G16B16A16_USCALED = 93,
        VK_FORMAT_R16G16B16A16_SSCALED = 94,
        VK_FORMAT_R16G16B16A16_UINT = 95,
        VK_FORMAT_R16G16B16A16_SINT = 96,
        VK_FORMAT_R16G16B16A16_SFLOAT = 97,
        VK_FORMAT_R32_UINT = 98,
        VK_FORMAT_R32_SINT = 99,
        VK_FORMAT_R32_SFLOAT = 100,
        VK_FORMAT_R32G32_UINT = 101,
        VK_FORMAT_R32G32_SINT = 102,
        VK_FORMAT_R32G32_SFLOAT = 103,
        VK_FORMAT_R32G32B32_UINT = 104,
        VK_FORMAT_R32G32B32_SINT = 105,
        VK_FORMAT_R32G32B32_SFLOAT = 106,
        VK_FORMAT_R32G32B32A32_UINT = 107,
        VK_FORMAT_R32G32B32A32_SINT = 108,
        VK_FORMAT_R32G32B32A32_SFLOAT = 109,
        VK_FORMAT_R64_UINT = 110,
        VK_FORMAT_R64_SINT = 111,
        VK_FORMAT_R64_SFLOAT = 112,
        VK_FORMAT_R64G64_UINT = 113,
        VK_FORMAT_R64G64_SINT = 114,
        VK_FORMAT_R64G64_SFLOAT = 115,
        VK_FORMAT_R64G64B64_UINT = 116,
        VK_FORMAT_R64G64B64_SINT = 117,
        VK_FORMAT_R64G64B64_SFLOAT = 118,
        VK_FORMAT_R64G64B64A64_UINT = 119,
        VK_FORMAT_R64G64B64A64_SINT = 120,
        VK_FORMAT_R64G64B64A64_SFLOAT = 121,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
        VK_FORMAT_D16_UNORM = 124,
        VK_FORMAT_X8_D24_UNORM_PACK32 = 125,
        VK_FORMAT_D32_SFLOAT = 126,
        VK_FORMAT_S8_UINT = 127,
        VK_FORMAT_D16_UNORM_S8_UINT = 128,
        VK_FORMAT_D24_UNORM_S8_UINT = 129,
        VK_FORMAT_D32_SFLOAT_S8_UINT = 130,
        VK_FORMAT_BC1_RGB_UNORM_BLOCK = 131,
        VK_FORMAT_BC1_RGB_SRGB_BLOCK = 132,
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
        VK_FORMAT_BC2_UNORM_BLOCK = 135,
        VK_FORMAT_BC2_SRGB_BLOCK = 136,
        VK_FORMAT_BC3_UNORM_BLOCK = 137,
        VK_FORMAT_BC3_SRGB_BLOCK = 138,
        VK_FORMAT_BC4_UNORM_BLOCK = 139,
        VK_FORMAT_BC4_SNORM_BLOCK = 140,
        VK_FORMAT_BC5_UNORM_BLOCK = 141,
        VK_FORMAT_BC5_SNORM_BLOCK = 142,
        VK_FORMAT_BC6H_UFLOAT_BLOCK = 143,
        VK_FORMAT_BC6H_SFLOAT_BLOCK = 144,
        VK_FORMAT_BC7_UNORM_BLOCK = 145,
        VK_FORMAT_BC7_SRGB_BLOCK = 146,
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
        VK_FORMAT_EAC_R11_UNORM_BLOCK = 153,
        VK_FORMAT_EAC_R11_SNORM_BLOCK = 154,
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
        VK_FORMAT_G8B8G8R8_422_UNORM = 1000156000,
        VK_FORMAT_B8G8R8G8_422_UNORM = 1000156001,
        VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM = 1000156002,
        VK_FORMAT_G8_B8R8_2PLANE_420_UNORM = 1000156003,
        VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM = 1000156004,
        VK_FORMAT_G8_B8R8_2PLANE_422_UNORM = 1000156005,
        VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM = 1000156006,
        VK_FORMAT_R10X6_UNORM_PACK16 = 1000156007,
        VK_FORMAT_R10X6G10X6_UNORM_2PACK16 = 1000156008,
        VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
        VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
        VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
        VK_FORMAT_R12X4_UNORM_PACK16 = 1000156017,
        VK_FORMAT_R12X4G12X4_UNORM_2PACK16 = 1000156018,
        VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
        VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
        VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
        VK_FORMAT_G16B16G16R16_422_UNORM = 1000156027,
        VK_FORMAT_B16G16R16G16_422_UNORM = 1000156028,
        VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM = 1000156029,
        VK_FORMAT_G16_B16R16_2PLANE_420_UNORM = 1000156030,
        VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM = 1000156031,
        VK_FORMAT_G16_B16R16_2PLANE_422_UNORM = 1000156032,
        VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM = 1000156033,
        VK_FORMAT_G8_B8R8_2PLANE_444_UNORM = 1000330000,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
        VK_FORMAT_G16_B16R16_2PLANE_444_UNORM = 1000330003,
        VK_FORMAT_A4R4G4B4_UNORM_PACK16 = 1000340000,
        VK_FORMAT_A4B4G4R4_UNORM_PACK16 = 1000340001,
        VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK = 1000066000,
        VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK = 1000066001,
        VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK = 1000066002,
        VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK = 1000066003,
        VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK = 1000066004,
        VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK = 1000066005,
        VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK = 1000066006,
        VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK = 1000066007,
        VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK = 1000066008,
        VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK = 1000066009,
        VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK = 1000066010,
        VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK = 1000066011,
        VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK = 1000066012,
        VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK = 1000066013,
        VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
        VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
        VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
        VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
        VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
        VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
        VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
        VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
        VK_FORMAT_R16G16_SFIXED5_NV = 1000464000,
        VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR = 1000470000,
        VK_FORMAT_A8_UNORM_KHR = 1000470001,
        VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK,
        VK_FORMAT_G8B8G8R8_422_UNORM_KHR = VK_FORMAT_G8B8G8R8_422_UNORM,
        VK_FORMAT_B8G8R8G8_422_UNORM_KHR = VK_FORMAT_B8G8R8G8_422_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
        VK_FORMAT_R10X6_UNORM_PACK16_KHR = VK_FORMAT_R10X6_UNORM_PACK16,
        VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR = VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
        VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        VK_FORMAT_R12X4_UNORM_PACK16_KHR = VK_FORMAT_R12X4_UNORM_PACK16,
        VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR = VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
        VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G16B16G16R16_422_UNORM_KHR = VK_FORMAT_G16B16G16R16_422_UNORM,
        VK_FORMAT_B16G16R16G16_422_UNORM_KHR = VK_FORMAT_B16G16R16G16_422_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
        VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
        VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,
        VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT = VK_FORMAT_A4R4G4B4_UNORM_PACK16,
        VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT = VK_FORMAT_A4B4G4R4_UNORM_PACK16,
        // VK_FORMAT_R16G16_S10_5_NV is a deprecated alias
        VK_FORMAT_R16G16_S10_5_NV = VK_FORMAT_R16G16_SFIXED5_NV,
        VK_FORMAT_MAX_ENUM = 0x7FFFFFFF
    };

    enum class KtxFormatSizeFlagBits
    {
        eKtxFormatSizePackedBit = 0x00000001,
        eKtxFormatSizeCompressedBit = 0x00000002,
        eKtxFormatSizePalettizedBit = 0x00000004,
        eKtxFormatSizeDepthBit = 0x00000008,
        eKtxFormatSizeStencilBit = 0x00000010,
        eKtxFormatSizeYuvsdaBit = 0x00000020,
    };

    template<typename Enum>
    class Flags
    {
    public:
        constexpr Flags(Enum value = static_cast<Enum>(0)) : enumValue(value) {}

        constexpr Flags& operator=(int intValue)
        {
            enumValue = static_cast<Enum>(intValue);
            return *this;
        }

        constexpr Flags operator|(Flags other) const
        {
            return Flags(static_cast<Enum>(static_cast<int>(enumValue) | static_cast<int>(other.enumValue)));
        }

        constexpr bool operator&(Flags other) const
        {
            return static_cast<int>(enumValue) & static_cast<int>(other.enumValue);
        }

        constexpr Flags operator^(Flags other) const
        {
            return Flags(static_cast<Enum>(static_cast<int>(enumValue) ^ static_cast<int>(other.enumValue)));
        }

        constexpr Enum value() const { return enumValue; }

    private:
        Enum enumValue;
    };

    template<typename Enum>
    constexpr Flags<Enum> operator|(Enum lhs, Enum rhs)
    {
        return Flags<Enum>(lhs) | Flags<Enum>(rhs);
    }

    template<typename Enum>
    constexpr bool operator&(Enum lhs, Enum rhs)
    {
        return Flags<Enum>(lhs) & Flags<Enum>(rhs);
    }

    struct KtxFormatSize
    {
        Flags<KtxFormatSizeFlagBits> flags;
        u32 palleteSize;
        u32 blockSize;
        u32 blockWidth;
        u32 blockHeight;
        u32 blockDepth;
        u32 minBlocksX;
        u32 minBlocksY;
    };

    struct KtxHeader
    {
        std::array<u8, 12> identifier;
        u32 endianness;
        u32 glType;
        u32 glTypeSize;
        u32 glFormat;
        u32 glInternalFormat;
        u32 glBaseInternalFormat;
        u32 pixelWidth;
        u32 pixelHeight;
        u32 pixelDepth;
        u32 numArrayElements;
        u32 numFaces;
        u32 numMipLevels;
        u32 keyValueData;
    };

    enum class KtxOrientationX
    {
        eLeft = 'l',
        eRight = 'r',
    };

    enum class KtxOrientationY
    {
        eUp = 'u',
        eDown = 'd',
    };

    enum class KtxOrientationZ
    {
        eIn = 'i',
        eOut = 'o'
    };

    struct KtxOrientation
    {
        KtxOrientationX x;
        KtxOrientationY y;
        KtxOrientationZ z;
    };

    struct UT_HashHandle
    {
        void* prev;
        void* next;
        UT_HashHandle* hPrev;
        UT_HashHandle* hNext;

        void* key;
        unsigned keyLen;
        unsigned hashV;
    };

    struct KtxHashList
    {
        u32 keyLen;
        char* key;
        u32 valueLen;
        void* value;
        UT_HashHandle hh;
    };

    struct KtxTexture
    {
        KtxFormatSize formatSize;
        u32 typeSize;
        bool isArray;
        bool isCubeMap;
        bool isCompressed;
        bool generateMipmaps;
        u32 baseWidth;
        u32 baseHeight;
        u32 baseDepth;
        u32 numDimensions;
        u32 numLevels;
        u32 numLayers;
        u32 numFaces;
        KtxOrientation orientation;
        KtxHashList* kvDataHead;
        u32 kvDataLen;
        u8* kvData;
        u64 dataSize;
        u8* pData;
    };

    struct KtxTexture1
    {
        KtxTexture texture;
        u32 glFormat;
        u32 glInternalFormat;
        u32 glBaseInternalFormat;
        u32 glType;
        bool needSwap;
    };

    typedef struct ktxIndexEntry32
    {
        u32 byteOffset;
        u32 byteLength;
    } ktxIndexEntry32;

    typedef struct ktxIndexEntry64
    {
        u64 byteOffset;
        u64 byteLength;
    } ktxIndexEntry64;

    struct Ktx2Header
    {
        std::array<u8, 12> identifier;
        u32 vkFormat;
        u32 typeSize;
        u32 pixelWidth;
        u32 pixelHeight;
        u32 pixelDepth;
        u32 layerCount;
        u32 faceCount;
        u32 levelCount;
        u32 superCompressionScheme;
        ktxIndexEntry32 dataFormatDescriptor;
        ktxIndexEntry32 keyValueData;
        ktxIndexEntry64 superCompressionGlobalData;
    };

    struct KtxSupplementalInfo
    {
        u8 compressed;
        u8 generateMipmaps;
        u16 textureDimension;
    };

    std::variant<KtxHeader, Ktx2Header> DetermineHeader(std::fstream& file)
    {
        std::array<u8, 12> buffer{};
        file.read(reinterpret_cast<char*>(buffer.data()), 12);
        if (std::equal(buffer.begin(), buffer.end(), ktxIdentifier.begin()))
        {
            KtxHeader header{};
            std::ranges::copy(buffer, header.identifier.begin());
            file.read(reinterpret_cast<char*>(&header.endianness), 4);
            file.read(reinterpret_cast<char*>(&header.glType), 4);
            file.read(reinterpret_cast<char*>(&header.glTypeSize), 4);
            file.read(reinterpret_cast<char*>(&header.glFormat), 4);
            file.read(reinterpret_cast<char*>(&header.glInternalFormat), 4);
            file.read(reinterpret_cast<char*>(&header.glBaseInternalFormat), 4);
            file.read(reinterpret_cast<char*>(&header.pixelWidth), 4);
            file.read(reinterpret_cast<char*>(&header.pixelHeight), 4);
            file.read(reinterpret_cast<char*>(&header.pixelDepth), 4);
            file.read(reinterpret_cast<char*>(&header.numArrayElements), 4);
            file.read(reinterpret_cast<char*>(&header.numFaces), 4);
            file.read(reinterpret_cast<char*>(&header.numMipLevels), 4);
            file.read(reinterpret_cast<char*>(&header.keyValueData), 4);
            return header;
        }
        if (std::equal(buffer.begin(), buffer.end(), ktx2Identifier.begin()))
        {
            Ktx2Header header{};
            std::ranges::copy(buffer, header.identifier.begin());
            file.read(reinterpret_cast<char*>(&header.vkFormat), 4);
            file.read(reinterpret_cast<char*>(&header.typeSize), 4);
            file.read(reinterpret_cast<char*>(&header.pixelWidth), 4);
            file.read(reinterpret_cast<char*>(&header.pixelHeight), 4);
            file.read(reinterpret_cast<char*>(&header.pixelDepth), 4);
            file.read(reinterpret_cast<char*>(&header.layerCount), 4);
            file.read(reinterpret_cast<char*>(&header.faceCount), 4);
            file.read(reinterpret_cast<char*>(&header.levelCount), 4);
            file.read(reinterpret_cast<char*>(&header.superCompressionScheme), 4);
            file.read(reinterpret_cast<char*>(&header.dataFormatDescriptor), 8);
            file.read(reinterpret_cast<char*>(&header.keyValueData), 8);
            file.read(reinterpret_cast<char*>(&header.superCompressionGlobalData), 16);
            return header;
        }
        assert(false && "File is not a ktx texture!");
        return {};
    }

    constexpr u32 SwapEndian32(u32 value)
    {
        return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
    }

    KtxSupplementalInfo CheckHeader(KtxHeader& header)
    {
        KtxSupplementalInfo info{};
        if (header.endianness == endianRefRev)
        {
            auto* start = &header.glType;
            for (u32 i = 0; i < 12; ++i)
            {
                start[i] = SwapEndian32(start[i]);
            }

            assert(header.glTypeSize != 1 && header.glTypeSize != 2 && header.glTypeSize != 4 && "Unsupported size!");
        } else if (header.endianness != endianRef)
        {
            assert(false && "Corrupt Data");
        }

        if (header.glType == 0 || header.glFormat == 0)
        {
            if (header.glType + header.glFormat != 0)
            {
                assert(false && "Unsupported format!");
            }
            info.compressed = 1;
        }

        assert(header.glFormat != header.glInternalFormat && "Corrupt Data");
        assert(header.pixelWidth > 0 && "Image must width greater than 0");
        if (header.pixelDepth > 0)
        {
            assert(header.pixelHeight > 0 && "3D Image must have height greater than 0");
        }

        if (header.pixelDepth > 0)
        {
            assert(header.numArrayElements == 0 && "3D array textures not supported");
            info.textureDimension = 3;
        } else if (header.pixelHeight > 0)
        {
            info.textureDimension = 2;
        } else
        {
            info.textureDimension = 1;
        }

        if (header.numFaces == 6)
        {
            assert(info.textureDimension == 2 && "Cube map must have 2D faces");
        } else if (header.numFaces != 1)
        {
            assert(false && "Unsupported faces!");
        }

        if (header.numMipLevels == 0)
        {
            info.generateMipmaps = 1;
            header.numMipLevels = 1;
        } else
        {
            info.generateMipmaps = 0;
        }

        const auto maxDim = std::max(std::max(header.pixelWidth, header.pixelHeight), header.pixelDepth);
        // Bit magic to check mip levels are a maximum of 1 + log2(max(width, height, depth)
        if (maxDim < 1u << header.numMipLevels - 1)
        {
            assert(false && "Unsupported mipmap count");
        }

        return info;
    }

    u32 CalculatePadding(const int n, const int nBytes)
    {
        return static_cast<u32>(n * std::ceil(static_cast<float>(nBytes) / n));
    }

    void AddKvPair(const KtxHashList* pHead, const char* key, const u32 valueLen, const void* value)
    {
        assert(pHead && key && "Invalid data");
        assert((valueLen == 0 || value) && " Invalid values");

        u32 keyLen = std::strlen(key) + 1;
        KtxHashList kv;

        assert(keyLen != -1 && "Invalid value");

        kv.key = reinterpret_cast<char*>(&kv) + sizeof(KtxHashList);
        kv.keyLen = keyLen;
        std::memcpy(kv.key, key, keyLen);
        kv.valueLen = valueLen;

        if (valueLen > 0)
        {
            kv.value = kv.key + keyLen;
            std::memcpy(kv.value, value, valueLen);
        }
        else
        {
            kv.value = nullptr;
        }
    }
    
    void HashListDeserialize(const KtxHashList* pHead, const u32 kvdLen, void* pKvd)
    {
        char* src = static_cast<char*>(pKvd);

        assert(kvdLen > 0 && pKvd != nullptr && pHead != nullptr);

        while (src < static_cast<char*>(pKvd) + kvdLen)
        {
            char* key;
            u32 keyLen, valueLen;
            void* value;
            u32 keyAndValueByteSize = *reinterpret_cast<u32*>(src);

            src += sizeof(keyAndValueByteSize);
            key = src;
            keyLen = keyAndValueByteSize;

            while (keyLen < keyAndValueByteSize && key[keyLen] != '\0') keyLen++;

            assert(keyLen != keyAndValueByteSize && key[keyLen] == '\0' && "Unsupported key length!");

            if (keyLen >= 3 && key[0] == '\xEF' && key[1] == '\xBB' && key[2] == '\xBF') {
                // Forbidden BOM
                assert(false && "Unsupported key length!");
            }

            keyLen += 1;
            value = key + keyLen;
            valueLen = keyAndValueByteSize - keyLen;

            AddKvPair(pHead, key, valueLen, valueLen > 0 ? value : nullptr);
            src += CalculatePadding(4, keyAndValueByteSize);
        }
    }

    void FindHash()
    {
    }
    
    KtxHashList HashListFindEntry(const KtxHashList* pHead, const char* key)
    {
        assert(pHead && key && "Invalid data");
        KtxHashList* kv;
    }
    
    void* HashListFindValue(KtxHashList* pHead, const char* key, u32 valueLen)
    {
        assert(valueLen > 0 && "Invalid value");
        KtxHashList* entry;
        HashListFindEntry()
    }
    
    KtxFormatSize GetFormatSize(const u32 internalFormat)
    {
        KtxFormatSize formatSize{.minBlocksX = 1, .minBlocksY = 1};
        switch (internalFormat)
        {
            case GL_R8: // 1-component, 8-bit unsigned normalized
            case GL_R8_SNORM: // 1-component, 8-bit signed normalized
            case GL_R8UI: // 1-component, 8-bit unsigned integer
            case GL_R8I: // 1-component, 8-bit signed integer
            case GL_SR8: // 1-component, 8-bit sRGB
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 1 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RG8: // 2-component, 8-bit unsigned normalized
            case GL_RG8_SNORM: // 2-component, 8-bit signed normalized
            case GL_RG8UI: // 2-component, 8-bit unsigned integer
            case GL_RG8I: // 2-component, 8-bit signed integer
            case GL_SRG8: // 2-component, 8-bit sRGB
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 2 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB8: // 3-component, 8-bit unsigned normalized
            case GL_RGB8_SNORM: // 3-component, 8-bit signed normalized
            case GL_RGB8UI: // 3-component, 8-bit unsigned integer
            case GL_RGB8I: // 3-component, 8-bit signed integer
            case GL_SRGB8: // 3-component, 8-bit sRGB
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 3 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA8: // 4-component, 8-bit unsigned normalized
            case GL_RGBA8_SNORM: // 4-component, 8-bit signed normalized
            case GL_RGBA8UI: // 4-component, 8-bit unsigned integer
            case GL_RGBA8I: // 4-component, 8-bit signed integer
            case GL_SRGB8_ALPHA8: // 4-component, 8-bit sRGB
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 4 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            //
            // 16 bits per component
            //
            case GL_R16: // 1-component, 16-bit unsigned normalized
            case GL_R16_SNORM: // 1-component, 16-bit signed normalized
            case GL_R16UI: // 1-component, 16-bit unsigned integer
            case GL_R16I: // 1-component, 16-bit signed integer
            case GL_R16F: // 1-component, 16-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 2 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RG16: // 2-component, 16-bit unsigned normalized
            case GL_RG16_SNORM: // 2-component, 16-bit signed normalized
            case GL_RG16UI: // 2-component, 16-bit unsigned integer
            case GL_RG16I: // 2-component, 16-bit signed integer
            case GL_RG16F: // 2-component, 16-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 4 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB16: // 3-component, 16-bit unsigned normalized
            case GL_RGB16_SNORM: // 3-component, 16-bit signed normalized
            case GL_RGB16UI: // 3-component, 16-bit unsigned integer
            case GL_RGB16I: // 3-component, 16-bit signed integer
            case GL_RGB16F: // 3-component, 16-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 6 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA16: // 4-component, 16-bit unsigned normalized
            case GL_RGBA16_SNORM: // 4-component, 16-bit signed normalized
            case GL_RGBA16UI: // 4-component, 16-bit unsigned integer
            case GL_RGBA16I: // 4-component, 16-bit signed integer
            case GL_RGBA16F: // 4-component, 16-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 8 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            //
            // 32 bits per component
            //
            case GL_R32UI: // 1-component, 32-bit unsigned integer
            case GL_R32I: // 1-component, 32-bit signed integer
            case GL_R32F: // 1-component, 32-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 4 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RG32UI: // 2-component, 32-bit unsigned integer
            case GL_RG32I: // 2-component, 32-bit signed integer
            case GL_RG32F: // 2-component, 32-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 8 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB32UI: // 3-component, 32-bit unsigned integer
            case GL_RGB32I: // 3-component, 32-bit signed integer
            case GL_RGB32F: // 3-component, 32-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 12 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA32UI: // 4-component, 32-bit unsigned integer
            case GL_RGBA32I: // 4-component, 32-bit signed integer
            case GL_RGBA32F: // 4-component, 32-bit floating-point
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            //
            // Packed
            //
            case GL_R3_G3_B2: // 3-component 3:3:2, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB4: // 3-component 4:4:4, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 12;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB5: // 3-component 5:5:5, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB565: // 3-component 5:6:5, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB10: // 3-component 10:10:10, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB12: // 3-component 12:12:12, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 36;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA2: // 4-component 2:2:2:2, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA4: // 4-component 4:4:4:4, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGBA12: // 4-component 12:12:12:12, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 48;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB5_A1: // 4-component 5:5:5:1, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB10_A2: // 4-component 10:10:10:2, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_RGB10_A2UI: // 4-component 10:10:10:2, unsigned integer
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_R11F_G11F_B10F: // 3-component 11:11:10, floating-point
            case GL_RGB9_E5: // 3-component/exp 9:9:9/5, floating-point
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePackedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            //
            // S3TC/DXT/BC
            //
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: // line through 3D space, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: // line through 3D space plus 1-bit alpha, 4x4 blocks, unsigned
                                                   // normalized
            case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT: // line through 3D space, 4x4 blocks, sRGB
            case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT: // line through 3D space plus 1-bit alpha, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: // line through 3D space plus line through 1D space, 4x4 blocks,
                                                   // unsigned normalized
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: // line through 3D space plus 4-bit alpha, 4x4 blocks, unsigned
                                                   // normalized
            case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT: // line through 3D space plus line through 1D space, 4x4
                                                         // blocks, sRGB
            case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT: // line through 3D space plus 4-bit alpha, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            case GL_COMPRESSED_LUMINANCE_LATC1_EXT: // line through 1D space, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT: // line through 1D space, 4x4 blocks, signed normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT: // two lines through 1D space, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT: // two lines through 1D space, 4x4 blocks, signed
                                                                 // normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            case GL_COMPRESSED_RED_RGTC1: // line through 1D space, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_RED_RGTC1: // line through 1D space, 4x4 blocks, signed normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RG_RGTC2: // two lines through 1D space, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_RG_RGTC2: // two lines through 1D space, 4x4 blocks, signed normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: // 3-component, 4x4 blocks, unsigned floating-point
            case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: // 3-component, 4x4 blocks, signed floating-point
            case GL_COMPRESSED_RGBA_BPTC_UNORM: // 4-component, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: // 4-component, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            //
            // ETC
            //
            case GL_ETC1_RGB8_OES: // 3-component ETC1, 4x4 blocks, unsigned normalized" ),
            case GL_COMPRESSED_RGB8_ETC2: // 3-component ETC2, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ETC2: // 3-component ETC2, 4x4 blocks, sRGB
            case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2: // 4-component ETC2 with 1-bit alpha, 4x4 blocks, unsigned
                                                              // normalized
            case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2: // 4-component ETC2 with 1-bit alpha, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA8_ETC2_EAC: // 4-component ETC2, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC: // 4-component ETC2, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            case GL_COMPRESSED_R11_EAC: // 1-component ETC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_R11_EAC: // 1-component ETC, 4x4 blocks, signed normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RG11_EAC: // 2-component ETC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SIGNED_RG11_EAC: // 2-component ETC, 4x4 blocks, signed normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            //
            // PVRTC
            //
            case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG: // 3-component PVRTC, 8x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT: // 3-component PVRTC, 8x4 blocks, sRGB
            case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: // 4-component PVRTC, 8x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT: // 4-component PVRTC, 8x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 8;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                formatSize.minBlocksX = 2;
                formatSize.minBlocksY = 2;
                break;
            case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG: // 3-component PVRTC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT: // 3-component PVRTC, 4x4 blocks, sRGB
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: // 4-component PVRTC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT: // 4-component PVRTC, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                formatSize.minBlocksX = 2;
                formatSize.minBlocksY = 2;
                break;
            case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG: // 4-component PVRTC, 8x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG: // 4-component PVRTC, 8x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 8;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG: // 4-component PVRTC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG: // 4-component PVRTC, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            //
            // ASTC
            //
            case GL_COMPRESSED_RGBA_ASTC_4x4_KHR: // 4-component ASTC, 4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR: // 4-component ASTC, 4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_5x4_KHR: // 4-component ASTC, 5x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR: // 4-component ASTC, 5x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 5;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_5x5_KHR: // 4-component ASTC, 5x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR: // 4-component ASTC, 5x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 5;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_6x5_KHR: // 4-component ASTC, 6x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR: // 4-component ASTC, 6x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 6;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_6x6_KHR: // 4-component ASTC, 6x6 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR: // 4-component ASTC, 6x6 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 6;
                formatSize.blockHeight = 6;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_8x5_KHR: // 4-component ASTC, 8x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR: // 4-component ASTC, 8x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 8;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_8x6_KHR: // 4-component ASTC, 8x6 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR: // 4-component ASTC, 8x6 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 8;
                formatSize.blockHeight = 6;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_8x8_KHR: // 4-component ASTC, 8x8 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR: // 4-component ASTC, 8x8 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 8;
                formatSize.blockHeight = 8;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_10x5_KHR: // 4-component ASTC, 10x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR: // 4-component ASTC, 10x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 10;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_10x6_KHR: // 4-component ASTC, 10x6 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR: // 4-component ASTC, 10x6 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 10;
                formatSize.blockHeight = 6;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_10x8_KHR: // 4-component ASTC, 10x8 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR: // 4-component ASTC, 10x8 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 10;
                formatSize.blockHeight = 8;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_10x10_KHR: // 4-component ASTC, 10x10 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR: // 4-component ASTC, 10x10 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 10;
                formatSize.blockHeight = 10;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_12x10_KHR: // 4-component ASTC, 12x10 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR: // 4-component ASTC, 12x10 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 12;
                formatSize.blockHeight = 10;
                formatSize.blockDepth = 1;
                break;
            case GL_COMPRESSED_RGBA_ASTC_12x12_KHR: // 4-component ASTC, 12x12 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: // 4-component ASTC, 12x12 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 12;
                formatSize.blockHeight = 12;
                formatSize.blockDepth = 1;
                break;

            case GL_COMPRESSED_RGBA_ASTC_3x3x3_OES: // 4-component ASTC, 3x3x3 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES: // 4-component ASTC, 3x3x3 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 3;
                formatSize.blockHeight = 3;
                formatSize.blockDepth = 3;
                break;
            case GL_COMPRESSED_RGBA_ASTC_4x3x3_OES: // 4-component ASTC, 4x3x3 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES: // 4-component ASTC, 4x3x3 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 3;
                formatSize.blockDepth = 3;
                break;
            case GL_COMPRESSED_RGBA_ASTC_4x4x3_OES: // 4-component ASTC, 4x4x3 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES: // 4-component ASTC, 4x4x3 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 3;
                break;
            case GL_COMPRESSED_RGBA_ASTC_4x4x4_OES: // 4-component ASTC, 4x4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES: // 4-component ASTC, 4x4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 4;
                break;
            case GL_COMPRESSED_RGBA_ASTC_5x4x4_OES: // 4-component ASTC, 5x4x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES: // 4-component ASTC, 5x4x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 5;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 4;
                break;
            case GL_COMPRESSED_RGBA_ASTC_5x5x4_OES: // 4-component ASTC, 5x5x4 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES: // 4-component ASTC, 5x5x4 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 5;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 4;
                break;
            case GL_COMPRESSED_RGBA_ASTC_5x5x5_OES: // 4-component ASTC, 5x5x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES: // 4-component ASTC, 5x5x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 5;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 5;
                break;
            case GL_COMPRESSED_RGBA_ASTC_6x5x5_OES: // 4-component ASTC, 6x5x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES: // 4-component ASTC, 6x5x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 6;
                formatSize.blockHeight = 5;
                formatSize.blockDepth = 5;
                break;
            case GL_COMPRESSED_RGBA_ASTC_6x6x5_OES: // 4-component ASTC, 6x6x5 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES: // 4-component ASTC, 6x6x5 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 6;
                formatSize.blockHeight = 6;
                formatSize.blockDepth = 5;
                break;
            case GL_COMPRESSED_RGBA_ASTC_6x6x6_OES: // 4-component ASTC, 6x6x6 blocks, unsigned normalized
            case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES: // 4-component ASTC, 6x6x6 blocks, sRGB
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 6;
                formatSize.blockHeight = 6;
                formatSize.blockDepth = 6;
                break;

            //
            // ATC
            //
            case GL_ATC_RGB_AMD: // 3-component, 4x4 blocks, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;
            case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD: // 4-component, 4x4 blocks, unsigned normalized
            case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD: // 4-component, 4x4 blocks, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeCompressedBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 128;
                formatSize.blockWidth = 4;
                formatSize.blockHeight = 4;
                formatSize.blockDepth = 1;
                break;

            //
            // Palletized
            //
            case GL_PALETTE4_RGB8_OES: // 3-component 8:8:8,   4-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 16 * 24;
                formatSize.blockSize = 4;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_PALETTE4_RGBA8_OES: // 4-component 8:8:8:8, 4-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 16 * 32;
                formatSize.blockSize = 4;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_PALETTE4_R5_G6_B5_OES: // 3-component 5:6:5,   4-bit palette, unsigned normalized
            case GL_PALETTE4_RGBA4_OES: // 4-component 4:4:4:4, 4-bit palette, unsigned normalized
            case GL_PALETTE4_RGB5_A1_OES: // 4-component 5:5:5:1, 4-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 16 * 16;
                formatSize.blockSize = 4;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_PALETTE8_RGB8_OES: // 3-component 8:8:8,   8-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 256 * 24;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_PALETTE8_RGBA8_OES: // 4-component 8:8:8:8, 8-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 256 * 32;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_PALETTE8_R5_G6_B5_OES: // 3-component 5:6:5,   8-bit palette, unsigned normalized
            case GL_PALETTE8_RGBA4_OES: // 4-component 4:4:4:4, 8-bit palette, unsigned normalized
            case GL_PALETTE8_RGB5_A1_OES: // 4-component 5:5:5:1, 8-bit palette, unsigned normalized
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizePalettizedBit;
                formatSize.palleteSize = 256 * 16;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            //
            // Depth/stencil
            //
            case GL_DEPTH_COMPONENT16:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeDepthBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_DEPTH_COMPONENT24:
            case GL_DEPTH_COMPONENT32:
            case GL_DEPTH_COMPONENT32F:
            case GL_DEPTH_COMPONENT32F_NV:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeDepthBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_STENCIL_INDEX1:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 1;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_STENCIL_INDEX4:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 4;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_STENCIL_INDEX8:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_STENCIL_INDEX16:
                formatSize.flags = KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 16;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_DEPTH24_STENCIL8:
                formatSize.flags =
                        KtxFormatSizeFlagBits::eKtxFormatSizeDepthBit | KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 32;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
            case GL_DEPTH32F_STENCIL8:
            case GL_DEPTH32F_STENCIL8_NV:
                formatSize.flags =
                        KtxFormatSizeFlagBits::eKtxFormatSizeDepthBit | KtxFormatSizeFlagBits::eKtxFormatSizeStencilBit;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 64;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;

            default:
                formatSize.flags = 0;
                formatSize.palleteSize = 0;
                formatSize.blockSize = 0 * 8;
                formatSize.blockWidth = 1;
                formatSize.blockHeight = 1;
                formatSize.blockDepth = 1;
                break;
        }
        return formatSize;
    }
} // namespace


void KTX::LoadKTXFromFile(const std::string_view fileName, const KtxCreateFlags flags)
{
    std::fstream file(fileName.data(), std::ios::in | std::ios::binary);

    if (!file.is_open())
        assert(false && "Failed to open file");

    const auto header = DetermineHeader(file);
    if (std::holds_alternative<KtxHeader>(header))
    {
        auto header1 = std::get<KtxHeader>(header);
        auto supplementInfo = CheckHeader(header1);
        KtxTexture texture{
                .formatSize = GetFormatSize(header1.glInternalFormat),
                .typeSize = header1.glTypeSize,
                .baseWidth = header1.pixelWidth,
                .numDimensions = supplementInfo.textureDimension,
                .orientation{KtxOrientationX::eRight, KtxOrientationY::eDown, KtxOrientationZ::eOut},
        };
        assert(supplementInfo.textureDimension > 0 && supplementInfo.textureDimension < 4);

        switch (supplementInfo.textureDimension)
        {
            case 1:
                texture.baseHeight = texture.baseDepth = 1;
                break;
            case 2:
                texture.baseHeight = header1.pixelHeight;
                texture.baseDepth = 1;
                break;
            case 3:
                texture.baseHeight = header1.pixelHeight;
                texture.baseDepth = header1.pixelDepth;
                break;
        }


        if (header1.numArrayElements > 0)
        {
            texture.numLayers = header1.numArrayElements;
            texture.isArray = true;
        } else
        {
            texture.numLayers = 1;
            texture.isArray = false;
        }
        texture.numFaces = header1.numFaces;
        if (header1.numFaces == 6)
        {
            texture.isCubeMap = true;
        } else
        {
            texture.isCubeMap = false;
        }
        texture.numLevels = header1.numMipLevels;
        texture.isCompressed = supplementInfo.compressed;
        texture.generateMipmaps = supplementInfo.generateMipmaps;
        texture.typeSize = header1.glTypeSize;

        KtxTexture1 texture1{
                .texture = texture,
                .glFormat = header1.glFormat,
                .glBaseInternalFormat = header1.glBaseInternalFormat,
                .glType = header1.glType,
        };
        if (header1.endianness == endianRefRev)
        {
            texture1.needSwap = true;
        }

        texture.kvData = nullptr;
        if (header1.keyValueData > 0)
        {
            const auto kvdLen = header1.keyValueData;
            auto pKvd = std::vector<u8>(kvdLen);
            file.read(reinterpret_cast<char*>(pKvd.data()), kvdLen);

            if (texture1.needSwap)
            {
                u8* src = pKvd.data();
                u8* end = pKvd.data() + kvdLen;
                while (src < end)
                {
                    auto keyAndValueByteSize = static_cast<u32>(*src);
                    SwapEndian32(keyAndValueByteSize);
                    src += CalculatePadding(4, keyAndValueByteSize);
                }
            }

            char* orientation;
            u32 orientationLen;

            HashListDeserialize(texture.kvDataHead, kvdLen, pKvd.data());
            HashListFindValue
        }

    } else
    {
        auto header2 = std::get<Ktx2Header>(header);
    }
}
