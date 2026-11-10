# tcxSpout - Spout2 texture sharing (Windows, Direct3D 11)
#
# Vendors the SpoutDX part of the Spout2 SDK (libs/Spout2, BSD-2-Clause).
# On other platforms the addon builds as a stub: Sender::setup() returns false.

set(ADDON_NAME tcxSpout)

if(WIN32)
    file(GLOB TCX_SPOUT_SDK_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/Spout2/src/*.cpp)
    add_library(${ADDON_NAME} STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcxSpoutSender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcxSpoutReceiver.cpp
        ${TCX_SPOUT_SDK_SOURCES})
    # The SDK is third-party code: keep its warnings out of the build output
    if(MSVC)
        set_source_files_properties(${TCX_SPOUT_SDK_SOURCES} PROPERTIES COMPILE_OPTIONS "/w")
    endif()
    # Spout uses the ANSI Win32 API
    target_compile_definitions(${ADDON_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_include_directories(${ADDON_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/Spout2/src)
    target_link_libraries(${ADDON_NAME}
        PUBLIC TrussC
        PRIVATE d3d11 dxgi winmm)
else()
    add_library(${ADDON_NAME} STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcxSpoutSender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tcxSpoutReceiver.cpp)
    target_include_directories(${ADDON_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(${ADDON_NAME} PUBLIC TrussC)
endif()
