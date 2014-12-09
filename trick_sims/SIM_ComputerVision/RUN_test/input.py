
import trick

def main():
    trick.exec_set_time_tic_value(3000000)
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_software_frame(0.05)
    trick.exec_set_freeze_frame(0.05)
    trick.sim_control_panel_set_enabled(True)
    trick.exec_set_terminate_time(50000.0)
    trick.frame_log_on()

    trick.exec_set_thread_process_type( 1 , trick.PROCESS_TYPE_ASYNC_CHILD )
    trick.exec_set_thread_async_cycle_time( 1 , 0.016666666666666666 )
    trick.exec_set_thread_cpu_affinity(0 , 0) ;
    trick.exec_set_thread_cpu_affinity(1 , 2) ;

    comp_vision.tcv.cam_res = trick.RES_640x480
    #comp_vision.tcv.show_depth_window = False
    #comp_vision.tcv.show_control_window = False
    #comp_vision.tcv.show_color_window = False
    #comp_vision.tcv.show_threshold_window = False
    #comp_vision.tcv.show_depth = False

if __name__ == "__main__":
    main()

