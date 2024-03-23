vcpkg_minimum_required(VERSION 2022-10-12) # for ${VERSION}
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO icculus/physfs
    REF "release-${VERSION}"
    SHA512 e0d84d6ac6bd8f0973149a5add54ed5ed890b5fabb4592ba61b59a3b3e01c05e05f1754f18d7a1c8d72e68777a23cda0c50dc0512cf57a8310a950bf908f54b1
)

set(generator_param "")
if(VCPKG_TARGET_IS_UWP)
    set(generator_param WINDOWS_USE_MSBUILD)
endif()

file(APPEND "${SOURCE_PATH}/CMakeLists.txt" "add_compile_options(-fPIC)")
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    ${generator_param}
    OPTIONS
        -DPHYSFS_BUILD_STATIC=ON
        -DPHYSFS_BUILD_SHARED=OFF
        -DPHYSFS_BUILD_TEST=OFF
        -DPHYSFS_BUILD_DOCS=OFF
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/PhysFS)
vcpkg_fixup_pkgconfig()
vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/physfs.h" "defined(PHYSFS_STATIC)" "1")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
