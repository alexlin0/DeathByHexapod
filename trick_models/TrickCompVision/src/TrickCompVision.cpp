
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "TrickCompVision.hh"
#include "sim_services/CommandLineArguments/include/command_line_protos.h"

static void on_mouse( int e, int x, int y, int d, void *ptr ) {
    (void) d ;
    if ( e == cv::EVENT_LBUTTONDOWN ) {
        cv::Point * p = (cv::Point *)ptr ;
        p->x = x ;
        p->y = y ;
        std::cout << "mouse clicked " << x << " " << y << std::endl ;
    }
}

// Initialize HSV values that isolate yellow balls.
TrickCompVision::TrickCompVision() :
 low_h(60),
 high_h(100),
 low_s(90),
 high_s(255),
 low_v(90),
 high_v(255),
 last_x(0),
 last_y(0),
 last_depth_processed(-1),
 last_color_processed(-1),
 min_size(0),
 show_depth_window(true),
 show_color_window(true),
 show_threshold_window(true),
 show_control_window(true),
 show_contour(true),
 show_depth_text(true),
 cam_res(RES_320x240),
 last_depth(NULL) {
    mouse_pos.x = -1 ;
    mouse_pos.y = -1 ;
}

int TrickCompVision::initialization() {

    if ( show_depth_window ) {
        cv::namedWindow("depth_image") ;
    }
    if ( show_color_window ) {
        cv::namedWindow("color_image") ;
        setMouseCallback("color_image", on_mouse, &mouse_pos) ;
    }
    if ( show_threshold_window ) {
        cv::namedWindow("ball_tracker") ;
    }
    if ( show_control_window ) {
        cv::namedWindow("Control") ;

        //Create trackbars in "Control" window
        cv::createTrackbar("LowH", "Control", &low_h, 179);
        cv::createTrackbar("HighH", "Control", &high_h, 179);

        cv::createTrackbar("LowS", "Control", &low_s, 255);
        cv::createTrackbar("HighS", "Control", &high_s, 255);

        cv::createTrackbar("LowV", "Control", &low_v, 255);
        cv::createTrackbar("HighV", "Control", &high_v, 255);
    }

    openni::Status rc = openni::OpenNI::initialize();
    if (rc != openni::STATUS_OK) {
        printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
        return 1;
    }

    rc = device.open(openni::ANY_DEVICE);
    if (rc != openni::STATUS_OK) {
        printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
        return 2;
    }

    if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL) {
        rc = depth.create(device, openni::SENSOR_DEPTH);
        if (rc != openni::STATUS_OK) {
            printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
            return 3;
        }
    }

    if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL) {
        rc = color.create(device, openni::SENSOR_COLOR);
        if (rc != openni::STATUS_OK) {
            printf("Couldn't create color stream\n%s\n", openni::OpenNI::getExtendedError());
            return 3;
        }
    }

    int cols , rows ;
    openni::VideoMode depth_mode = depth.getVideoMode() ;
    openni::VideoMode color_mode = color.getVideoMode() ;
    switch ( cam_res ) {
        case RES_320x240: cols = 320 ; rows = 240 ; break ;
        case RES_640x480: cols = 640 ; rows = 480 ; break ;
    }
    depth_mode.setResolution(cols, rows) ;
    color_mode.setResolution(cols, rows) ;
    depth.setVideoMode(depth_mode) ;
    color.setVideoMode(color_mode) ;

    rc = depth.start();
    if (rc != openni::STATUS_OK) {
        printf("Couldn't start the depth stream\n%s\n", openni::OpenNI::getExtendedError());
        return 4;
    }
    rc = color.start();
    if (rc != openni::STATUS_OK) {
        printf("Couldn't start the start stream\n%s\n", openni::OpenNI::getExtendedError());
        return 4;
    }

    depth.addNewFrameListener(&depth_cb);
    color.addNewFrameListener(&color_cb);

    return 0 ;
}

int TrickCompVision::change_tracking_color() {

    // check if new color in color screen chosen
    if ( mouse_pos.x != -1 ) {
        cv::Vec3b hsv_val = hsv_image.at<cv::Vec3b>(mouse_pos.y,mouse_pos.x) ;
        //std::cout << " pixel value " << (int)hsv_val[0] << " " << (int)hsv_val[1] << " " << (int)hsv_val[2] << std::endl ;
        low_h = hsv_val[0] - 20 ;
        if ( low_h < 0 ) {
            low_h = 0 ;
        }
        high_h =  hsv_val[0] + 20 ;
        if ( high_h > 179 ) {
            high_h = 179 ;
        }
        low_s = hsv_val[1] - 40 ;
        if ( low_s < 0 ) {
            low_s = 0 ;
        }
        high_s =  hsv_val[1] + 40 ;
        if ( high_s > 255 ) {
            high_s = 255 ;
        }
        low_v = hsv_val[2] - 40 ;
        if ( low_v < 0 ) {
            low_v = 0 ;
        }
        high_v =  hsv_val[2] + 40 ;
        if ( high_v > 255 ) {
            high_v = 255 ;
        }
        mouse_pos.x = -1 ;
    }
    return 0 ;
}

int TrickCompVision::scheduled() {

    if ( depth_cb.getLastFrameIndex() != last_depth_processed ) {
        last_depth_processed = depth_cb.getLastFrameIndex() ;
        last_depth = depth_cb.getLastFrame() ;
        if ( last_depth != NULL ) {
            // Xtion depth maximum value is 10000.  Scale it up to give better contrast
            if ( show_depth_window ) {
                cv::Mat depth_f(last_depth->rows, last_depth->cols, CV_8UC1) ;
                last_depth->convertTo(depth_f , CV_16UC1, 65535.0/10000.0) ;
                cv::imshow("depth_image", depth_f) ;
            }
        }
    }

    if ( color_cb.getLastFrameIndex() != last_color_processed ) {

        last_color_processed = color_cb.getLastFrameIndex() ;
        cv::Mat * last_color = color_cb.getLastFrame() ;

        if ( last_color != NULL ) {
            cv::Mat color_correct ;
            cv::Mat canny_output ;

            //Correct the color for display
            cv::cvtColor(*last_color, color_correct, CV_BGR2RGB) ;

            //Convert the image to hsv
            cv::cvtColor(*last_color, hsv_image, CV_BGR2HSV) ;

            //Threshold the image
            cv::inRange(hsv_image, cv::Scalar(low_h, low_s, low_v), cv::Scalar(high_h, high_s, high_v), thresh_image);

            //morphological opening (remove small objects from the foreground)
            cv::erode(thresh_image, thresh_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
            cv::dilate( thresh_image, thresh_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

            //morphological closing (fill small holes in the foreground)
            dilate( thresh_image, thresh_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
            erode(thresh_image, thresh_image, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

            // Detect edges using canny
            int thresh = 100 ;
            std::vector<std::vector<cv::Point> > contours;
            std::vector<cv::Vec4i> hierarchy;

            // Detect edges using canny
            Canny( thresh_image, canny_output, thresh, thresh*2, 3 );

            // Find contours
            findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

            // Get moments and mass centers, draw contours
            std::vector<cv::Moments> mu(contours.size() );
            std::vector<cv::Point2f> mc( contours.size() );
            for( unsigned int i = 0; i < contours.size(); i++ ) {
                mu[i] = moments( contours[i], false );

                // continue if the size is large enough
                if ( mu[i].m00 > min_size ) {
                    // get mass center
                    mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

                    cv::Scalar color = cv::Scalar( 0, 0, 255) ;
                    if ( !isnan(mc[i].x) and !isnan(mc[i].y) ) {
                        if ( show_contour ) {
                            cv::drawContours( color_correct, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
                        }
                        //cv::circle( color_correct, mc[i], 4, color, -1, 8, 0 );

                        if ( show_depth_text ) {
                            if ( last_depth != NULL ) {
                                int depth_val = last_depth->at< short >(mc[i].y, mc[i].x) ;
                                std::stringstream ss ;
                                ss << depth_val ;
                                cv::putText( color_correct , ss.str(), cv::Point(mc[i].x + 10, mc[i].y),
                                 cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0,0,255)) ;
                            }
                        }
                    }
                }
            }

            if ( show_color_window ) {
                cv::imshow("color_image", color_correct) ;
            }
            if ( show_threshold_window ) {
                cv::imshow("ball_tracker", thresh_image);
            }
        }
    }

    cv::waitKey(5) ;

    return 0 ;
}

int TrickCompVision::shutdown() {

    depth.removeNewFrameListener(&depth_cb);
    depth.stop();
    depth.destroy();

    color.removeNewFrameListener(&color_cb);
    color.stop();
    color.destroy();

    device.close();
    openni::OpenNI::shutdown();

    return 0 ;
}
