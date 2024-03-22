set(PHYSFS_BUILD_STATIC ON)
set(PHYSFS_BUILD_SHARED OFF)
set(PHYSFS_BUILD_TEST FALSE)
set(PHYSFS_BUILD_DOCS FALSE)
set(PHYSFS_DISABLE_INSTALL ON)

if(NOT DEFINED Python_EXECUTABLE)
  find_package(Python3 COMPONENTS Development REQUIRED)
endif()
find_package(pybind11 CONFIG)
find_package(PhysFS CONFIG REQUIRED)


set(MODULE_SOURCE main.cpp 
  physfs.shim.cpp physfs.shim.h
  importer.cpp importer.h
  )

pybind11_add_module(memory-importer ${MODULE_SOURCE})
set_property(TARGET memory-importer PROPERTY CXX_STANDARD 17)
if(VERSION_INFO)
  target_compile_definitions(memory-importer PRIVATE VERSION_INFO=${VERSION_INFO})
endif()

target_include_directories(memory-importer PUBLIC 
  ${PHYSFS_INCLUDE_DIR}
)
target_link_libraries(memory-importer PRIVATE 
  $<IF:$<TARGET_EXISTS:PhysFS::PhysFS>,PhysFS::PhysFS,PhysFS::PhysFS-static> 
  pybind11::pybind11
)
set_target_properties(memory-importer PROPERTIES OUTPUT_NAME "memory_importer")


# set(STATIC_SOURCE shim.cpp shim.h)
# pybind11_add_module(memory-importer-static STATIC NO_EXTRAS ${MODULE_SOURCE})
# set_property(TARGET memory-importer-static PROPERTY CXX_STANDARD 17)
# target_include_directories(memory-importer-static PUBLIC 
#   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/pybind11/include
#   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/physfs/src
#   )
# target_link_libraries(memory-importer-static PUBLIC physfs-static)
