cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(space_invaders)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Space Invaders")
set(VITA_TITLEID  "SPCNVD")
set(VITA_VERSION  "01.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
  ../common
)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
SDL2_mixer
SDL2_image 
SDL2_ttf
SDL2
webp
freetype
vorbisfile
vorbis
ogg
mikmod
pthread
jpeg
png16
mpg123
z
c
m
mad

FLAC
pthread

vita2d
SceDisplay_stub
SceCtrl_stub
SceAudio_stub
SceSysmodule_stub
SceGxm_stub
SceCommonDialog_stub
SceAppMgr_stub
SceTouch_stub
SceHid_stub
SceMotion_stub
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE rd/blaster.ogg
  FILE rd/bodenstaendig.ogg
  FILE rd/bullet.png
  FILE rd/enemy-bullet.png
  FILE rd/explode.png
  FILE rd/explode1.ogg
  FILE rd/fmg_splash.png
  FILE rd/gameover_ui.png
  FILE rd/invader32x32x4.png
  FILE rd/player.png
  FILE rd/pusher.ogg
  FILE rd/space3.png
  FILE rd/vermin_vibes_1989.ttf
  FILE rd/win_ui.png
)
