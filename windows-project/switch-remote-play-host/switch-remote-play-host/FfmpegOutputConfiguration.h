#ifndef __FFMPEGOUTPUTCONFIGURATION_H__
#define __FFMPEGOUTPUTCONFIGURATION_H__

#include <string>
#include "FfmpegOutputOptions.h"
#include "MappedDataConfiguration.h"

class FfmpegOutputConfiguration
{
public:
    FfmpegOutputConfiguration();
    FfmpegOutputConfiguration(std::string const file);

    bool Save(ffmpeg::FfmpegOutputConfig const data);
    ffmpeg::FfmpegOutputConfig const Data() const;

private:
    MappedDataConfiguration<ffmpeg::FfmpegOutputConfig, ffmpeg::OutputParameters> configMap;
};

#endif

