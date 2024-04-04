#include <pybind11/pybind11.h>
#include "physfs.shim.h"
#include "importer.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace py = pybind11;


PYBIND11_MODULE(memory_importer, m) {
    memory_importer::register_memory_importer(m);
    py::module mPhysfs = m.def_submodule("physfs", "physfs lib");
    memory_importer::physfs::register_physfs(mPhysfs);
#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif

m.attr("__author__") = "shabbywu<shabbywu@qq.com>";
}
