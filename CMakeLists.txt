# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.13)

# Enable VCPKG if VCPKG_ROOT defined.
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "")
endif()

# Enable VCPKG if VCPKG_ROOT defined.
option(INSTALL_PYTHON "Install Python with vcpkg?" OFF)
if(INSTALL_PYTHON)
  list(APPEND VCPKG_MANIFEST_FEATURES "with-python3")
else()
  list(APPEND VCPKG_MANIFEST_FEATURES "without-python3")
endif()

set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_LIST_DIR}/.vcpkg/physfs")

project ("python-memory-importer")

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(src)
