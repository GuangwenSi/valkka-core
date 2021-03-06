#ifndef muxer_HEADER_GUARD
#define muxer_HEADER_GUARD
/*
 * muxer.h : FFmpeg muxers, implemented as Valkka framefilters
 * 
 * Copyright 2017, 2018, 2019 Valkka Security Ltd. and Sampsa Riikonen.
 * 
 * Authors: Sampsa Riikonen <sampsa.riikonen@iki.fi>
 * 
 * This file is part of the Valkka library.
 * 
 * Valkka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 *
 */

/** 
 *  @file    muxer.h
 *  @author  Sampsa Riikonen
 *  @date    2019
 *  @version 0.16.0 
 *  
 *  @brief   FFmpeg muxers, implemented as Valkka framefilters
 * 
 * 
 *  Some references:
 *  - https://stackoverflow.com/questions/54931744/mux-remux-box-in-memory-h264-frames-to-mp4-via-c-c-no-ffmpeg-cmdline
 *  - https://gist.github.com/AlexVestin/15b90d72f51ff7521cd7ce4b70056dff
 * 
 */ 

#endif


#include "constant.h"
#include "framefilter.h"


class MuxFrameFilter : public FrameFilter {                          // <pyapi>
    
public:                                                              // <pyapi>
    MuxFrameFilter(const char* name, FrameFilter *next = NULL);      // <pyapi>
    virtual ~MuxFrameFilter();                                       // <pyapi>
    
protected:
    bool active;                       ///< Writing to file has been requested (but not necessarily achieved..)
    bool ready;                        ///< Got enough setup frames
    bool initialized;                  ///< File was opened ok : codec_contexes, streams and av_format_context reserved (should be freed at some point)
    long int mstimestamp0;             ///< Time of activation (i.e. when the recording started)
    long int zerotime;                 ///< Start time set explicitly by the user
    bool zerotimeset;
    uint32_t missing, ccf;
    std::string format_name;
    
protected: //libav stuff
    AVFormatContext               *av_format_ctx;
    AVIOContext                   *avio_ctx;
    uint8_t                       *avio_ctx_buffer;
    AVRational                    timebase;
    std::vector<AVCodecContext*>  codec_contexes;
    std::vector<AVStream*>        streams;
    AVFormatContext               *av_format_context;
    AVPacket                      *avpkt;
    AVDictionary                  *av_dict;
    
    
    static const size_t avio_ctx_buffer_size = 4096;
  
protected: //mutex stuff
    std::mutex              mutex;     ///< Mutex protecting the "active" boolean
    std::condition_variable condition; ///< Condition variable for the mutex

protected: //frames
    std::vector<SetupFrame>     setupframes;        ///< deep copies of the arrived setup frames
    
public:
    // MuxFrame                    internal_frame;     ///< outgoing muxed frame // TODO
    BasicFrame                     internal_frame; 
  
protected:
    virtual void defineMux() = 0; ///< Define container format (format_name) & muxing parameters (av_dict).  Define in child classes.
    virtual void go(Frame* frame);
    void initMux();           ///< Open file, reserve codec_contexes, streams, write preamble, set initialized=true if success
    void closeMux();          ///< Close file, dealloc codec_contexes, streams
    void deActivate_();
  
public: // API calls                                                                           // <pyapi>
    // setFileName(const char* fname); ///< Sets the output filename                           // <pyapi>
    void activate(long int zerotime=0);       ///< Request streaming to asap (when config frames have arrived) // <pyapi>
    void deActivate();                                           ///< Stop streaming           // <pyapi>
    
public:
    static int write_packet(void *opaque, uint8_t *buf, int buf_size);
    static int read_packet(void *opaque, uint8_t *buf, int buf_size) {return 0;} // dummy function
    static int64_t seek(void *opaque, int64_t offset, int whence) {return 0;} // dummy function
};                                                                                             // <pyapi>



class FragMP4MuxFrameFilter : public MuxFrameFilter {                       // <pyapi>
    
public:                                                                     // <pyapi>
    FragMP4MuxFrameFilter(const char* name, FrameFilter *next = NULL);      // <pyapi>
    virtual ~FragMP4MuxFrameFilter();                                       // <pyapi>
    
public:
    // MP4MuxFrame internal_frame; // TODO
    BasicFrame internal_frame;
    
protected:
    virtual void defineMux();
};                                                                           // <pyapi>





