#ifndef __AVCODECDEFINITIONS_H__
#define __AVCODECDEFINITIONS_H__

#ifdef __SWITCH__
extern "C"
{
    #include <libavcodec/avcodec.h>
}
#else

/**
 * HWAccel is experimental and is thus avoided in favor of non experimental
 * codecs
 */
auto constexpr AV_HWACCEL_CODEC_CAP_EXPERIMENTAL = 0x0200;

/**
 * Hardware acceleration should be used for decoding even if the codec level
 * used is unknown or higher than the maximum supported level reported by the
 * hardware driver.
 *
 * It's generally a good idea to pass this flag unless you have a specific
 * reason not to, as hardware tends to under-report supported levels.
 */
auto constexpr AV_HWACCEL_FLAG_IGNORE_LEVEL = (1 << 0);

/**
 * Hardware acceleration can output YUV pixel formats with a different chroma
 * sampling than 4:2:0 and/or other than 8 bits per component.
 */
auto constexpr AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH = (1 << 1);

/**
 * Hardware acceleration should still be attempted for decoding when the
 * codec profile does not match the reported capabilities of the hardware.
 *
 * For example, this can be used to try to decode baseline profile H.264
 * streams in hardware - it will often succeed, because many streams marked
 * as baseline profile actually conform to constrained baseline profile.
 *
 * @warning If the stream is actually not supported then the behaviour is
 *          undefined, and may include returning entirely incorrect output
 *          while indicating success.
 */
auto constexpr AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH = (1 << 2);

/**
     * Which multithreading methods to use.
     * Use of FF_THREAD_FRAME will increase decoding delay by one frame per thread,
     * so clients which cannot provide future frames should not use it.
     *
     * - encoding: Set by user, otherwise the default is used.
     * - decoding: Set by user, otherwise the default is used.
     */
auto constexpr FF_THREAD_FRAME = 1; ///< Decode more than one frame at once
auto constexpr FF_THREAD_SLICE = 2; ///< Decode more than one part of a single frame at once

/**
 * @ingroup lavc_decoding
 */
enum AVDiscard{
    /* We leave some space between them for extensions (drop some
     * keyframes for intra-only or drop just some bidir frames). */
    AVDISCARD_NONE    =-16, ///< discard nothing
    AVDISCARD_DEFAULT =  0, ///< discard useless packets like 0 size packets in avi
    AVDISCARD_NONREF  =  8, ///< discard all non reference
    AVDISCARD_BIDIR   = 16, ///< discard all bidirectional frames
    AVDISCARD_NONINTRA= 24, ///< discard all non intra frames
    AVDISCARD_NONKEY  = 32, ///< discard all frames except keyframes
    AVDISCARD_ALL     = 48, ///< discard all
};

/**
 * Allow decoders to produce frames with data planes that are not aligned
 * to CPU requirements (e.g. due to cropping).
 */
auto constexpr AV_CODEC_FLAG_UNALIGNED = (1 <<  0);
/**
 * Use fixed qscale.
 */
auto constexpr AV_CODEC_FLAG_QSCALE = (1 <<  1);
/**
 * 4 MV per MB allowed / advanced prediction for H.263.
 */
auto constexpr AV_CODEC_FLAG_4MV = (1 <<  2);
/**
 * Output even those frames that might be corrupted.
 */
auto constexpr AV_CODEC_FLAG_OUTPUT_CORRUPT = (1 <<  3);
/**
 * Use qpel MC.
 */
auto constexpr AV_CODEC_FLAG_QPEL = (1 <<  4);
/**
 * Don't output frames whose parameters differ from first
 * decoded frame in stream.
 */
auto constexpr AV_CODEC_FLAG_DROPCHANGED = (1 <<  5);
/**
 * Use internal 2pass ratecontrol in first pass mode.
 */
auto constexpr AV_CODEC_FLAG_PASS1 = (1 <<  9);
/**
 * Use internal 2pass ratecontrol in second pass mode.
 */
auto constexpr AV_CODEC_FLAG_PASS2 = (1 << 10);
/**
 * loop filter.
 */
auto constexpr AV_CODEC_FLAG_LOOP_FILTER = (1 << 11);
/**
 * Only decode/encode grayscale.
 */
auto constexpr AV_CODEC_FLAG_GRAY = (1 << 13);
/**
 * error[?] variables will be set during encoding.
 */
auto constexpr AV_CODEC_FLAG_PSNR = (1 << 15);
/**
 * Input bitstream might be truncated at a random location
 * instead of only at frame boundaries.
 */
auto constexpr AV_CODEC_FLAG_TRUNCATED = (1 << 16);
/**
 * Use interlaced DCT.
 */
auto constexpr AV_CODEC_FLAG_INTERLACED_DCT = (1 << 18);
/**
 * Force low delay.
 */
auto constexpr AV_CODEC_FLAG_LOW_DELAY = (1 << 19);
/**
 * Place global headers in extradata instead of every keyframe.
 */
auto constexpr AV_CODEC_FLAG_GLOBAL_HEADER = (1 << 22);
/**
 * Use only bitexact stuff (except (I)DCT).
 */
auto constexpr AV_CODEC_FLAG_BITEXACT = (1 << 23);
/* Fx : Flag for H.263+ extra options */
/**
 * H.263 advanced intra coding / MPEG-4 AC prediction
 */
auto constexpr AV_CODEC_FLAG_AC_PRED = (1 << 24);
/**
 * interlaced motion estimation
 */
auto constexpr AV_CODEC_FLAG_INTERLACED_ME = (1 << 29);
auto constexpr AV_CODEC_FLAG_CLOSED_GOP = (1U << 31);

/**
 * Allow non spec compliant speedup tricks.
 */
auto constexpr AV_CODEC_FLAG2_FAST = (1 <<  0);
/**
 * Skip bitstream encoding.
 */
auto constexpr AV_CODEC_FLAG2_NO_OUTPUT = (1 <<  2);
/**
 * Place global headers at every keyframe instead of in extradata.
 */
auto constexpr AV_CODEC_FLAG2_LOCAL_HEADER = (1 <<  3);

/**
 * timecode is in drop frame format. DEPRECATED!!!!
 */
auto constexpr AV_CODEC_FLAG2_DROP_FRAME_TIMECODE = (1 << 13);

/**
 * Input bitstream might be truncated at a packet boundaries
 * instead of only at frame boundaries.
 */
auto constexpr AV_CODEC_FLAG2_CHUNKS = (1 << 15);
/**
 * Discard cropping information from SPS.
 */
auto constexpr AV_CODEC_FLAG2_IGNORE_CROP = (1 << 16);

/**
 * Show all frames before the first keyframe
 */
auto constexpr AV_CODEC_FLAG2_SHOW_ALL = (1 << 22);
/**
 * Export motion vectors through frame side data
 */
auto constexpr AV_CODEC_FLAG2_EXPORT_MVS = (1 << 28);
/**
 * Do not skip samples and export skip information as frame side data
 */
auto constexpr AV_CODEC_FLAG2_SKIP_MANUAL = (1 << 29);
/**
 * Do not reset ASS ReadOrder field on flush (subtitles decoding)
 */
auto constexpr AV_CODEC_FLAG2_RO_FLUSH_NOOP = (1 << 30);

#endif

#endif
