
/**
PURPOSE:
    (OpenNI2 interface.)
LIBRARY DEPENDENCY:
    ((../src/FrameEventCallback.cpp))
*/

#ifndef _FRAMEEVENTCALLBACK_HH_
#define _FRAMEEVENTCALLBACK_HH_

#ifndef TRICK_ICG
#ifndef SWIG
#include <vector>
#include "OpenNI.h"
#include <opencv2/core/core.hpp>

class FrameEventCallback : public openni::VideoStream::NewFrameListener {
    public:
       FrameEventCallback() ;
       void onNewFrame(openni::VideoStream& stream) ;

       cv::Mat * getLastFrame() ;
       int getLastFrameIndex() ;

    protected:
       int read_index ;
       int write_index ;
       int last_written ;

       std::vector<cv::Mat> img_data ;

       openni::VideoFrameRef m_frame;
};

#endif
#endif
#endif
