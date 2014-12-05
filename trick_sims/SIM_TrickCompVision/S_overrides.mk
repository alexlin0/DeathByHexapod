
DEATHBYHEXAPOD_HOME ?= $(abspath ../..)

TRICK_CFLAGS += -I${DEATHBYHEXAPOD_HOME}/trick_models -I${HOME}/OpenNI2/Include -Wno-shadow
TRICK_CXXFLAGS += -I${DEATHBYHEXAPOD_HOME}/trick_models -I${HOME}/OpenNI2/Include -Wno-shadow
TRICK_LDFLAGS += -Wl,-rpath,/usr/local/lib64 -Wl,-rpath,/usr/local/lib

export TRICK_ICG_EXCLUDE = ${HOME}/OpenNI2
TRICK_USER_LINK_LIBS += -lOpenNI2 -lopencv_core -lopencv_highgui -lopencv_imgproc
