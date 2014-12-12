
DEATHBYHEXAPOD_HOME ?= $(abspath ../..)

TRICK_CFLAGS += -I${DEATHBYHEXAPOD_HOME}/trick_models -Wno-shadow
TRICK_CXXFLAGS += -I${DEATHBYHEXAPOD_HOME}/trick_models -Wno-shadow

