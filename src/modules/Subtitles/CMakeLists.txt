cmake_minimum_required(VERSION 3.16)
project(Subtitles)

set(Subtitles_HDR
    SRT.hpp
    Classic.hpp
    Subtitles.hpp
)

set(Subtitles_SRC
    SRT.cpp
    Classic.cpp
    Subtitles.cpp
)

set(Subtitles_RESOURCES
    icon.qrc
)

add_library(${PROJECT_NAME} MODULE
    ${Subtitles_HDR}
    ${Subtitles_SRC}
    ${Subtitles_RESOURCES}
)

if(USE_PCH)
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        ${Subtitles_HDR}
    )
endif()

libqmplay2_set_target_params()

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${MODULES_INSTALL_PATH})
