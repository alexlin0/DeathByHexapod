
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

if __name__ == "__main__":
    main()

