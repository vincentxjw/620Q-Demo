cmake_minimum_required(VERSION 3.1)
project(myvideo)

set(TOOLCHAIN_DIR "/opt/arm-AX620E-linux-uclibcgnueabihf/")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-AX620E-linux-uclibcgnueabihf-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-AX620E-linux-uclibcgnueabihf-gcc)  #这两行可用下面的两行替代 但需要配置环境便利CROSS_COMPILE
# set(CMAKE_CXX_COMPILER $ENV{CROSS_COMPILE}g++)
# set(CMAKE_C_COMPILER $ENV{CROSS_COMPILE}gcc)
message("----------- $ENV{CROSS_COMPILE}")
# c++11标准
set(CMAKE_CXX_STANDARD 11)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+fp+simd+crypto+crc")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--allow-shlib-undefined -Waddress-of-packed-member")
# add_compile_options(-O3) 

set(AX620Q_SDK_DIR "/home/vincent/workspace/projects/620Q/ax620q_bsp_sdk")
set(SAMPLE_PATH ${AX620Q_SDK_DIR}/msp/sample)
set(SAMPLE_COMMON_PATH ${SAMPLE_PATH}/common)
set(AX620Q_SDK_LIB_PATH ${AX620Q_SDK_DIR}/msp/out/arm_uclibc)
set(SSL_PATH ${AX620Q_SDK_DIR}/third-party/openssl/arm)
set(LIVE_PATH ${AX620Q_SDK_DIR}/third-party/live/out/arm/uclibc)
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/libs/opencv/lib/cmake/opencv4)


# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

message("LIVE_PATH:" ${LIVE_PATH})
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${SAMPLE_COMMON_PATH}
    ${AX620Q_SDK_LIB_PATH}/include
    ${LIVE_PATH}/include/BasicUsageEnvironment
    ${LIVE_PATH}/include/groupsock
    ${LIVE_PATH}/include/liveMedia
    ${LIVE_PATH}/include/UsageEnvironment
    ${SSL_PATH}/include
    ${SAMPLE_PATH}/rtsp
)

set(SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/ivps_hal.c
    ${CMAKE_SOURCE_DIR}/src/vin_ivps_venc_rtsp.c
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${SAMPLE_COMMON_PATH}/common_venc.c
    ${SAMPLE_COMMON_PATH}/common_venc_log.c
    ${SAMPLE_COMMON_PATH}/common_arg_parse.c
    ${SAMPLE_COMMON_PATH}/common_cam.c
    ${SAMPLE_COMMON_PATH}/common_sys.c
    ${SAMPLE_COMMON_PATH}/common_vin.c
    ${SAMPLE_COMMON_PATH}/common_isp.c
    ${SAMPLE_COMMON_PATH}/common_nt.c
    ${SAMPLE_COMMON_PATH}/common_hw.c
    ${SAMPLE_PATH}/rtsp/AXFramedSource.cpp
    ${SAMPLE_PATH}/rtsp/AXLiveServerMediaSession.cpp
    ${SAMPLE_PATH}/rtsp/AXRtspServer.cpp
    ${SAMPLE_PATH}/rtsp/AXRtspWrapper.cpp

    ${CMAKE_SOURCE_DIR}/src/Application.cpp
)


find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
	message("OpenCV_DIR:" ${OpenCV_DIR})
endif()

include_directories( 
        ${OpenCV_INCLUDE_DIRS}) 


message("found opencv:" ${OpenCV_VERSION} ${OpenCV_LIBRARIES})

set(Libraries
libax_ivps.a
libax_sys.a
pthread
libax_nt_ctrl.a
libax_proton.a
libax_engine_tiny.a
libax_interpreter.a
libax_ae.a
libax_awb.a
libax_af.a
libax_mipi.a
libax_nt_stream.a
libax_venc.a
#ssl crypto
liveMedia groupsock BasicUsageEnvironment UsageEnvironment
libsns_dummy.a
libsns_sc4210.a
libax_sys.a
rt
m
dl
${OpenCV_LIBRARIES}
)

link_directories( 
    ${AX620Q_SDK_LIB_PATH}/lib
    ${LIVE_PATH}/lib
    ${SSL_PATH}/uclibc/lib
)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${Libraries})

install(TARGETS ${PROJECT_NAME} DESTINATION ./)