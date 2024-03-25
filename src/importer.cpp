#include "importer.h"
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/stl.h>


namespace py = pybind11;
using namespace pybind11::literals;


PhysfsImporter:: PhysfsImporter(std::string archiveFilePath) {
    auto physfs = py::module_::import("memory_importer.physfs");
    try
    {
        physfs.attr("init")();
    }
    catch(const py::error_already_set& e)
    {
        if(debug) {
            std::cerr << "Unknown exception occur: " << e.what() << std::endl;
        }
    }
    physfs.attr("mount")(archiveFilePath);
}


py::object PhysfsImporter::find_spec(py::str fullname, std::optional<py::object> path, std::optional<py::handle> target) {
    // TODO: rewrite with pure cpp
    py::dict locals;
    if (debug) {
        std::cout << "find_spec(fullname='" << fullname << "'";
        if (path) {
            std::cout << ", path=" << *path;
        }
        if (target) {
            std::cout << ", target=" << *target;
        }
        std::cout << ")" << std::endl;
    }

    locals["module_name"] = fullname;
    locals["module_path"] = fullname.attr("replace")(".", "/");
    locals["loader"] = this;

    py::str module_name = fullname;
    py::str module_path = fullname.attr("replace")(".", "/");

    auto physfs = py::module_::import("memory_importer.physfs");
    // TODO: pure cpp ModuleSpec
    auto ModuleSpec = py::module_::import("importlib.machinery").attr("ModuleSpec");
    auto _NamespacePath = py::module_::import("_frozen_importlib_external").attr("_NamespacePath");
    py::str realpath = "";
    py::bool_ is_package = false;
    py::str ext = "";
    for (auto _ext : {"", ".pyc", ".py"}) {
        try
        {
            auto info = physfs.attr("stat")(module_path + py::str(_ext));
            realpath = module_path + py::str(_ext);
            ext = py::str(_ext);
            break;
        }
        catch(const py::error_already_set& e)
        {
            continue;
        }
    }
    if (realpath == py::str("")) {
        if (debug) {
            std::cout << "Exception: " << "not a module in physfs" << std::endl;
        }
        return py::none();
    }
    if (ext == py::str("")) {
        for (auto _ext : {".pyc", ".py"}) {
            try
            {
                auto info = physfs.attr("stat")(module_path + py::str("/__init__") + py::str(_ext));
                realpath = module_path + py::str("/__init__") + py::str(_ext);
                ext = py::str(_ext);
                is_package = true;
                break;
            }
            catch(const py::error_already_set& e)
            {
                continue;
            }
        }
    }
    auto spec = ModuleSpec(
        "name"_a=module_name,
        "loader"_a=this,
        "origin"_a=py::str("physfs://") + realpath,
        "is_package"_a=is_package
    );
    if (is_package) {
        if (ext == py::str("")) {
            spec.attr("submodule_search_locations") = py::make_tuple(module_path);
        } else {
            spec.attr("submodule_search_locations") = _NamespacePath(module_name, py::make_tuple(module_path), py::cpp_function(&PhysfsImporter::find_spec));
        }
        spec.attr("has_location") = true;
    }
    return spec;
}


py::none PhysfsImporter::create_module(py::object spec) {
    // return None to indicate that the spec should create the new module.
    return py::none();
}


void PhysfsImporter::exec_module(py::module_ py_module) {
    // TODO: rewrite with pure cpp
    auto spec = py_module.attr("__spec__");
    auto module_name = spec.attr("name").cast<py::str>();
    auto module_path = spec.attr("origin").attr("replace")("physfs://", "");

    auto locals = py::dict();
    auto marshal = py::module_::import("marshal");
    auto physfs = py::module_::import("memory_importer.physfs");

    auto exec = py::module_::import("builtins").attr("exec");
    if (physfs.attr("stat")(module_path).attr("filetype").cast<py::object>() != physfs.attr("PHYSFS_FileType").attr("PHYSFS_FILETYPE_DIRECTORY").cast<py::object>()) {
        auto data = physfs.attr("cat")(module_path).cast<py::bytes>();
        if (module_path.attr("endswith")(".pyc").cast<bool>()) {
            std::optional<ssize_t> start = 16;
            std::optional<ssize_t> stop;
            std::optional<ssize_t> step;
            exec(marshal.attr("loads")(data[py::slice(start, stop, step)]).cast<py::object>(), py_module.attr("__dict__"));
        } else {
            exec(data, py_module.attr("__dict__"));
        }
    }
}


void register_memory_importer(py::module_ &m) {
    m.doc() = R"pbdoc(
        MemoryImporter - Import your python code from memory or archive file.
        -----------------------
    )pbdoc";

    py::class_<PhysfsImporter, std::shared_ptr<PhysfsImporter>>(m, "PhysfsImporter")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def("find_spec", &PhysfsImporter::find_spec, py::arg("fullname"), py::arg("path").none(true) = py::none(), py::arg("target").none(true) = py::none())
        .def("create_module", &PhysfsImporter::create_module)
        .def("exec_module", &PhysfsImporter::exec_module, py::arg("module").none(false))
        .def_property("debug", [](PhysfsImporter &i ) -> py::bool_ { return i.debug;}, [](PhysfsImporter &i, py::bool_ v) { i.debug = v;})
    ;
}
