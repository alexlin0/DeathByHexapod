
#include "FrameEventCallback.hh"

FrameEventCallback::FrameEventCallback() : read_index(-1) , write_index(-1) , last_written(-1) , img_data(10) {}

void FrameEventCallback::onNewFrame(openni::VideoStream& stream) {
    stream.readFrame(&m_frame);

    switch (m_frame.getVideoMode().getPixelFormat()) {
        case openni::PIXEL_FORMAT_DEPTH_1_MM:
        case openni::PIXEL_FORMAT_DEPTH_100_UM: {
            openni::DepthPixel* pDepth;
            pDepth = (openni::DepthPixel*)m_frame.getData();

            write_index++ ;
            if ( write_index >= 9 ) {
                write_index = 0 ;
            }

            img_data[write_index] = cv::Mat(m_frame.getHeight(), m_frame.getWidth(), CV_16UC1, pDepth) ;

            last_written = write_index ;
        }
        break;
        case openni::PIXEL_FORMAT_RGB888: {
            openni::RGB888Pixel* pColor;

            pColor = (openni::RGB888Pixel*)m_frame.getData();

            write_index++ ;
            if ( write_index >= 9 ) {
                write_index = 0 ;
            }
            img_data[write_index] = cv::Mat(m_frame.getHeight(), m_frame.getWidth(), CV_8UC3, pColor) ;

            last_written = write_index ;

        }
        break;
        default:
            printf("Unknown format\n");
    }

}

cv::Mat * FrameEventCallback::getLastFrame() {
    if ( last_written > 0 ) {
        return &img_data[last_written] ;
    }
    return NULL ;
}

int FrameEventCallback::getLastFrameIndex() {
    return last_written ;
}
