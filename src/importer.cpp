#include "importer.h"
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/stl.h>


namespace py = pybind11;


PhysfsImporter:: PhysfsImporter(std::string archiveFilePath) {
    // TODO: use physfs memory archive
    py::dict locals;
    locals["archive_filepath"] = archiveFilePath;
    py::exec(R"(
        from memory_importer import physfs
        try:
            physfs.init()
        except:
            ...
        physfs.mount(archive_filepath)
    )", py::globals(), locals);
}


py::object PhysfsImporter::find_spec(py::str fullname, std::optional<py::object> path, std::optional<py::object> target) {
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
    try
    {
        py::exec(R"(
            from memory_importer import physfs
            from importlib.machinery import ModuleSpec
            from importlib._bootstrap_external import _NamespacePath

            realpath = None
            is_package = False
            for ext in ["", ".pyc", ".py"]:
                try:
                    info = physfs.stat(module_path + ext)
                    realpath = module_path + ext
                    break
                except RuntimeError:
                    continue
            else:
                raise ValueError("not a module in physfs")
            if ext == "":
                for ext in [".pyc", ".py"]:
                    try:
                        info = physfs.stat(module_path + "/__init__" + ext)
                        realpath = module_path + "/__init__" + ext
                        is_package = True
                        break
                    except RuntimeError:
                        continue
                else:
                    is_package = True
            spec = ModuleSpec(
                name=module_name,
                loader=loader,
                origin="physfs://" + realpath,
                is_package=is_package
            )
            if is_package:
                if ext == "":
                    spec.submodule_search_locations = [module_path]
                else:
                    spec.submodule_search_locations = _NamespacePath(
                        module_name,
                        [module_path],
                        loader.find_spec,
                    )
                spec.has_location = True
        )", py::globals(), locals);
    }
    catch(const py::error_already_set& e)
    {
        if (debug) {
            std::cout << "Exception:" << e.what() << std::endl;
        }
        return py::none();
    }
    return locals["spec"];
}


py::none PhysfsImporter::create_module(py::object spec) {
    return py::none();
}


void PhysfsImporter::exec_module(py::module_ py_module) {
    auto spec = py_module.attr("__spec__");
    auto module_name = spec.attr("name").cast<py::str>();
    auto module_path = spec.attr("origin").attr("replace")("physfs://", "");
 
    auto locals = py::dict();
    locals["module_path"] = module_path;
    locals["module"] = py_module;
    py::exec(R"(
        import marshal
        from memory_importer import physfs
        if physfs.stat(module_path).filetype == physfs.PHYSFS_FileType.PHYSFS_FILETYPE_DIRECTORY:
            ...
        else:
            data = physfs.cat(module_path)
            if module_path.endswith(".pyc"):
                exec(marshal.loads(data[16:]), module.__dict__)
            else:
                exec(data, module.__dict__)
    )", py::globals(), locals);
}


void register_memory_importer(py::module_ &m) {
    m.doc() = R"pbdoc(
        MemoryImporter - Import your python code from memory or archive file.
        -----------------------
    )pbdoc";

    py::class_<PhysfsImporter, std::shared_ptr<PhysfsImporter>>(m, "PhysfsImporter")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def("find_spec", &PhysfsImporter::find_spec, py::arg("fullname"), py::arg("path").none(true), py::arg("target").none(true))
        .def("create_module", &PhysfsImporter::create_module)
        .def("exec_module", &PhysfsImporter::exec_module, py::arg("module").none(false))
        .def_property("debug", [](PhysfsImporter &i ) -> py::bool_ { return i.debug;}, [](PhysfsImporter &i, py::bool_ v) { i.debug = v;})
    ;
}