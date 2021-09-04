message(STATUS "Building Z80 core...")
add_subdirectory(z80/BeeZ80)

message(STATUS "Building SN76489 core...")
set(SN76489_SOURCES
	psg/BeePSG/BeePSG/sn76489/sn76489.cpp)

set(SN76489_HEADERS
	sn76489.h)

add_library(sn76489 STATIC ${SN76489_SOURCES} ${SN76489_HEADERS})