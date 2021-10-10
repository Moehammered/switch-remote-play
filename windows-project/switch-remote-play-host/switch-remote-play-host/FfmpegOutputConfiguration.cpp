#include "FfmpegOutputConfiguration.h"

FfmpegOutputConfiguration::FfmpegOutputConfiguration()
    : configMap{ "ffmpeg-output-options.ini", ffmpeg::OutputParamsDesc}
{
}

FfmpegOutputConfiguration::FfmpegOutputConfiguration(std::string const file)
    : configMap{ file, ffmpeg::OutputParamsDesc }
{
}

bool FfmpegOutputConfiguration::Save(ffmpeg::FfmpegOutputConfig const data)
{
    return configMap.Save(data, ffmpeg::OutputParamsToStr);
}

ffmpeg::FfmpegOutputConfig const FfmpegOutputConfiguration::Data() const
{
    return configMap.Data(ffmpeg::OutputParamsFromStr);
}
