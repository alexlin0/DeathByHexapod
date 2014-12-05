
/**
PURPOSE:
    (CompVision interface.)
LIBRARY DEPENDENCY:
    ((../src/TrickCompVision.cpp))
*/

#ifndef _TRICKCOMPVISION_HH_
#define _TRICKCOMPVISION_HH_

#ifndef TRICK_ICG
#ifndef SWIG
#include "OpenNI.h"
#include <opencv2/core/core.hpp>
#endif
#endif
#include "FrameEventCallback.hh"

enum CameraResolution {
   RES_320x240 ,
   RES_640x480
} ;

class TrickCompVision {

    public:
        TrickCompVision() ;
        int initialization() ;
        int change_tracking_color() ;
        int scheduled() ;
        int shutdown() ;

        int low_h ;
        int high_h ;
        int low_s ;
        int high_s ;
        int low_v ;
        int high_v ;

        int last_x ;
        int last_y ;

        int last_depth_processed ;
        int last_color_processed ;

        unsigned int min_size ;

        bool show_depth_window ;
        bool show_color_window ;
        bool show_threshold_window ;
        bool show_control_window ;
        bool show_contour ;
        bool show_depth_text ;

        CameraResolution cam_res ;
#ifndef TRICK_ICG
#ifndef SWIG
        FrameEventCallback depth_cb ;
        FrameEventCallback color_cb ;

        openni::Device device;
        openni::VideoStream depth;
        openni::VideoStream color;
        openni::VideoFrameRef frame;

        cv::Mat * last_depth ;
        cv::Mat thresh_image ;
        cv::Mat hsv_image ;

        cv::Point mouse_pos ;
#endif
#endif
    private:
        // This object is not copyable
        void operator =(const TrickCompVision &) {};

} ;

#endif
