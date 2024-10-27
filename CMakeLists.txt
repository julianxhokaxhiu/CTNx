#*****************************************************************************#
#    Copyright (C) 2024 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of CTNx                                                #
#                                                                             #
#    CTNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    CTNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to continue building with cmake. \nExample: Add -DCMAKE_BUILD_TYPE=Release to your cmake command line.")
endif()
if (NOT DEFINED _DLL_VERSION OR NOT _DLL_VERSION)
  message(FATAL_ERROR "_DLL_VERSION must be set to continue building with cmake. \nExample: Add -D_DLL_VERSION=devel to your cmake command line.")
endif ()

set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--x-use-aria2")

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4099,4217"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
else()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif()

project(CTNx)

find_package(FFMPEG REQUIRED)
find_package(MPG123 REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(VGMSTREAM REQUIRED)
find_package(STACKWALKER REQUIRED)
find_package(mimalloc CONFIG REQUIRED)
find_package(SOLOUD REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)
find_package(HWINFO REQUIRED)
find_package(plog CONFIG REQUIRED)
find_package(LIBCOCOS2D REQUIRED)
find_package(glfw3 REQUIRED)

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

set(RELEASE_NAME "CTNx")

if(_DLL_VERSION STREQUAL "devel" OR _DLL_VERSION MATCHES "-")
  set(_DLL_RCVERSION "0,0,0,0")
  set(_DLL_RCSTRVERSION "0.0.0.0")
  set(PATCH_COLLECT_DUPLICATES 1)
else()
  string(REPLACE "." "," _DLL_RCVERSION ${_DLL_VERSION})
  set(_DLL_RCSTRVERSION ${_DLL_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")

configure_file(misc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

cmrc_add_resource_library(
  ${RELEASE_NAME}-resources
  ALIAS ${RELEASE_NAME}::rc
  NAMESPACE ${RELEASE_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/.logo/logo_nobg.png
)

add_library(${RELEASE_NAME} SHARED ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
target_include_directories(
  ${RELEASE_NAME}
  PRIVATE "${CMAKE_SOURCE_DIR}/src"
  PRIVATE ${FFMPEG_INCLUDE_DIRS}
  PRIVATE ${SOLOUD_INCLUDE_DIRS}
  PRIVATE ${STACKWALKER_INCLUDE_DIRS}
  PRIVATE ${VGMSTREAM_INCLUDE_DIRS}
  PRIVATE ${DETOURS_INCLUDE_DIRS}
  PRIVATE ${LIBCOCOS2D_INCLUDE_DIR}
)
target_link_directories(
  ${RELEASE_NAME}
  PRIVATE ${FFMPEG_LIBRARY_DIRS}
)
target_link_libraries(
  ${RELEASE_NAME}
  ${RELEASE_NAME}::rc
  dbghelp
  shlwapi
  psapi
  dwmapi
  winmm
  strmiids
  mimalloc-static
  tomlplusplus::tomlplusplus
  Vorbis::vorbisfile
  Vorbis::vorbis
  MPG123::libmpg123
  HWINFO::HWINFO
  plog::plog
  LIBCOCOS2D::LIBCOCOS2D
  glfw3
  ${FFMPEG_LIBRARIES}
  ${SOLOUD_LIBRARIES}
  ${STACKWALKER_LIBRARIES}
  ${VGMSTREAM_LIBRARIES}
  ${DETOURS_LIBRARY}
)
target_compile_options(
  ${RELEASE_NAME}
  PRIVATE /D_CRT_SECURE_NO_WARNINGS
  PRIVATE /DNOMINMAX
  PRIVATE /DWIN32_LEAN_AND_MEAN
  PRIVATE /Zc:strictStrings-
  PRIVATE /Zc:__cplusplus
  PRIVATE /Zc:preprocessor
  PRIVATE /Qpar
  PRIVATE /MP
  # App Defines
  PRIVATE /DAPP_CONFIG_FILE="${RELEASE_NAME}.toml"
  PRIVATE /DAPP_RELEASE_NAME="${RELEASE_NAME}"
  PRIVATE /DAPP_RELEASE_VERSION="${_DLL_RCSTRVERSION}"
)
if(PATCH_COLLECT_DUPLICATES)
  target_compile_definitions(${RELEASE_NAME}
    PRIVATE PATCH_COLLECT_DUPLICATES
  )
endif()
target_compile_features(${RELEASE_NAME}
  PRIVATE cxx_std_20
)
target_link_options(${RELEASE_NAME} PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb)

# List .gitkeep files to remove them on INSTALL
FILE(GLOB_RECURSE HEXT_GIT_KEEP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/misc/ ${CMAKE_CURRENT_SOURCE_DIR}/misc/hext/**/.gitkeep)
LIST(TRANSFORM HEXT_GIT_KEEP_FILES PREPEND ${CMAKE_BINARY_DIR}/bin/)

# INSTALL
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  COMMAND echo Preparing ${RELEASE_NAME} ${_DLL_VERSION} release...
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
  # License
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.TXT
          ${CMAKE_BINARY_DIR}/bin
  # .dll
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.dll
          ${CMAKE_BINARY_DIR}/bin
  # .pdb
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.pdb
          ${CMAKE_BINARY_DIR}/bin
  # .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.toml
          ${CMAKE_BINARY_DIR}/bin
  # hext
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/hext
          ${CMAKE_BINARY_DIR}/bin/hext
  COMMAND ${CMAKE_COMMAND} -E rm ${HEXT_GIT_KEEP_FILES}

  # ambient .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.ambient.toml
          ${CMAKE_BINARY_DIR}/bin/ambient/config.toml

  # Voice .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.voice.toml
          ${CMAKE_BINARY_DIR}/bin/voice/config.toml
)

# CPU INFO
add_custom_command(
  TARGET ${RELEASE_NAME}
  PRE_BUILD
  # Get CPU info
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/cpu-info
  # Get CPU ISA supported instructions
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/isa-info
)

# Copy CTNx release to Chrono Trigger game path if installed
execute_process(
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/FindSteamGamePath 613830
  OUTPUT_VARIABLE CT_GAME_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT "${CT_GAME_PATH}" STREQUAL "")
  cmake_path(CONVERT "${CT_GAME_PATH}" TO_CMAKE_PATH_LIST CT_GAME_PATH)
  # Ensure CTNx.toml exists so the copy on build does not error on fresh installs
  if(NOT EXISTS "${CT_GAME_PATH}/CTNx.toml")
    # Copy file if it doesn't exist
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/misc/CTNx.toml" DESTINATION "${CT_GAME_PATH}/")
  endif()
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    COMMAND echo Copying ${RELEASE_NAME} ${_DLL_VERSION} release to ${CT_GAME_PATH}...
    # Preserve the current CTNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${CT_GAME_PATH}/CTNx.toml
            ${CT_GAME_PATH}/CTNx.toml.bak
    # Copy all dist files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/bin
            ${CT_GAME_PATH}
    # Delete previous version.dll
    COMMAND ${CMAKE_COMMAND} -E remove
            ${CT_GAME_PATH}/version.dll
    # Rename CTNx.dll to version.dll
    COMMAND ${CMAKE_COMMAND} -E rename
            ${CT_GAME_PATH}/CTNx.dll
            ${CT_GAME_PATH}/version.dll
    # Delete the new copied CTNx.toml
    COMMAND ${CMAKE_COMMAND} -E remove
            ${CT_GAME_PATH}/CTNx.toml
    # Bring back the existing CTNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${CT_GAME_PATH}/CTNx.toml.bak
            ${CT_GAME_PATH}/CTNx.toml
  )
endif()
