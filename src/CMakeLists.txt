if(NOT DEFINED Python_EXECUTABLE)
  find_package(Python3 COMPONENTS Development REQUIRED)
endif()
find_package(pybind11 CONFIG)


set(SOURCE_FILE
  importer.cpp
)

if(APPLE)
    set(OTHER_LDFLAGS ${OTHER_LDFLAGS} "-framework IOKit -framework Foundation")
    # To use .cpp file in ObjC we required to convert that .cpp extension to .mm for ObjC++ compiler only. (thanks Mats for your comment it's helped me a lot.)
    file(COPY_FILE physfs.shim.cpp physfs.shim.mm)
    LIST(APPEND SOURCE_FILE physfs.shim.mm)
else()
  LIST(APPEND SOURCE_FILE physfs.shim.cpp)
endif()


option(BUILD_MEMORY_IMPORTER_MODULE "Build memory-importer.pyd?" ON)
if(BUILD_MEMORY_IMPORTER_MODULE)
  set(MODULE_SOURCE main.cpp ${SOURCE_FILE})
  pybind11_add_module(memory-importer ${MODULE_SOURCE})
  set_property(TARGET memory-importer PROPERTY CXX_STANDARD 17)
  if(VERSION_INFO)
    target_compile_definitions(memory-importer PRIVATE VERSION_INFO=${VERSION_INFO})
  endif()
  target_link_libraries(memory-importer PUBLIC
    pybind11::pybind11
    $<IF:$<TARGET_EXISTS:Python3::Python>,Python3::Python,>
    ${OTHER_LDFLAGS}
  )
  set_target_properties(memory-importer PROPERTIES OUTPUT_NAME "memory_importer")
endif()

option(BUILD_MEMORY_IMPORTER_LIBRARY "Build memory-importer as static library?" OFF)
if(BUILD_MEMORY_IMPORTER_LIBRARY)
  set(STATIC_SOURCE ${SOURCE_FILE})
  pybind11_add_module(memory-importer-static STATIC NO_EXTRAS ${STATIC_SOURCE})
  set_property(TARGET memory-importer-static PROPERTY CXX_STANDARD 17)
  target_link_libraries(memory-importer-static PUBLIC
    $<IF:$<TARGET_EXISTS:Python3::Python>,Python3::Python,>
    ${OTHER_LDFLAGS}
  )
endif()
