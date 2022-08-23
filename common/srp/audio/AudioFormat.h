#ifndef __AUDIOFORMAT_H__
#define __AUDIOFORMAT_H__

/*
#define AUDIO_U8        0x0008  // < Unsigned 8-bit samples
#define AUDIO_S8        0x8008  // < Signed 8-bit samples
#define AUDIO_U16LSB    0x0010  // < Unsigned 16-bit samples
#define AUDIO_S16LSB    0x8010  // < Signed 16-bit samples
#define AUDIO_U16MSB    0x1010  // < As above, but big-endian byte order
#define AUDIO_S16MSB    0x9010  // < As above, but big-endian byte order
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB
*/

#include <vector>
#include <unordered_map>
#include <string>

namespace audio
{
    enum class AudioFormat
    {
        Unsigned8Bit,
        Signed8Bit,
        LittleEndian_Unsigned16Bit,
        LittleEndian_Signed16Bit,
        BigEndian_Unsigned16Bit,
        BigEndian_Signed16Bit,
        DefaultUnsigned16Bit = LittleEndian_Unsigned16Bit,
        DefaultSigned16Bit = LittleEndian_Signed16Bit
    };

    static std::unordered_map<AudioFormat, std::string> const audioFormatDescMap
    {
        { AudioFormat::Unsigned8Bit, "8 Bit (Unsigned)" },
        { AudioFormat::Signed8Bit, "8 Bit (Signed)" },
        { AudioFormat::LittleEndian_Unsigned16Bit, "16 Bit (Unsigned - Little Endian)" },
        { AudioFormat::LittleEndian_Signed16Bit, "16 Bit (Signed - Little Endian)" },
        { AudioFormat::BigEndian_Unsigned16Bit, "16 Bit (Unsigned - Big Endian)" },
        { AudioFormat::BigEndian_Signed16Bit, "16 Bit (Signed - Big Endian)" }
    };

    static std::unordered_map<AudioFormat, std::string> const audioFormatStrMap
    {
        { AudioFormat::Unsigned8Bit, "unsigned_8bit" },
        { AudioFormat::Signed8Bit, "signed_8bit" },
        { AudioFormat::LittleEndian_Unsigned16Bit, "unsigned_16bit_little-endian" },
        { AudioFormat::LittleEndian_Signed16Bit, "signed_16bit_little-endian" },
        { AudioFormat::BigEndian_Unsigned16Bit, "unsigned_16bit_big-endian" },
        { AudioFormat::BigEndian_Signed16Bit, "signed_16bit_big-endian" }
    };

    std::string audioFormatToStr(AudioFormat format);
    std::string audioFormatToDesc(AudioFormat format);
    AudioFormat audioFormatStrToEnum(std::string const s);
}

#endif
