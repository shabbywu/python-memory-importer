if(NOT DEFINED Python_EXECUTABLE)
  find_package(Python3 COMPONENTS Development REQUIRED)
endif()
find_package(pybind11 CONFIG)
if(NOT DEFINED SKIP_FIND_PHYSFS)
  find_package(PhysFS CONFIG REQUIRED)
endif()


option(BUILD_MEMORY_IMPORTER_MODULE "Build memory-importer.pyd?" ON)
if(BUILD_MEMORY_IMPORTER_MODULE)
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
  target_link_libraries(memory-importer PUBLIC
    $<IF:$<TARGET_EXISTS:PhysFS::PhysFS-static>,PhysFS::PhysFS-static,>
    pybind11::pybind11
    $<IF:$<TARGET_EXISTS:Python3::Python>,Python3::Python,>
  )
  set_target_properties(memory-importer PROPERTIES OUTPUT_NAME "memory_importer")
endif()

option(BUILD_MEMORY_IMPORTER_LIBRARY "Build memory-importer as static library?" OFF)
if(BUILD_MEMORY_IMPORTER_LIBRARY)
  set(STATIC_SOURCE
    physfs.shim.cpp physfs.shim.h
    importer.cpp importer.h
  )
  pybind11_add_module(memory-importer-static STATIC NO_EXTRAS ${STATIC_SOURCE})
  set_property(TARGET memory-importer-static PROPERTY CXX_STANDARD 17)
  target_link_libraries(memory-importer-static PUBLIC
    $<IF:$<TARGET_EXISTS:PhysFS::PhysFS-static>,PhysFS::PhysFS-static,>
    $<IF:$<TARGET_EXISTS:Python3::Python>,Python3::Python,>
  )
endif()
