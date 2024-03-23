#pragma once
#include <filesystem>
#include <pybind11/pybind11.h>


namespace py = pybind11;


class PhysfsImporter {
    public:
        py::object ModuleSpecClass;
        py::object ModuleClass;
        py::bool_ debug;

        PhysfsImporter() {
            ModuleSpecClass = py::module_::import("importlib.machinery").attr("ModuleSpec");
            // A trick in python, new module by calling `type(sys)(module_name)`
            // ref: https://github.com/python/cpython/blob/main/Lib/importlib/_bootstrap.py#L48
            ModuleClass = py::type::of(py::module_::import("sys"));
        }
        PhysfsImporter(std::string archiveFilePath);

        // PEP-451 finder.find_spec() method for the ``sys.meta_path`` hook.

        // fullname     fully qualified name of the module
        // path         None for a top-level module, or [package.__path__] or  _NamespacePath for
        //              submodules or subpackages. unused by this Finder
        // target       unused by this Finder

        // Finders are still responsible for identifying, and typically creating, the loader that should be used to load a
        // module. That loader will now be stored in the module spec returned by find_spec() rather than returned directly.
        // As is currently the case without the PEP-452, if a loader would be costly to create, that loader can be designed
        // to defer the cost until later.

        // Finders must return ModuleSpec objects when find_spec() is called. This new method replaces find_module() and
        // find_loader() (in the PathEntryFinder case). If a loader does not have find_spec(), find_module() and
        // find_loader() are used instead, for backward-compatibility.
        py::object find_spec(py::str fullname, std::optional<py::object> path, std::optional<py::object> target);

        // PEP-451 loader.create_module() method for the ``sys.meta_path`` hook.

        // Loaders may also implement create_module() that will return a new module to exec. It may return None to indicate
        // that the default module creation code should be used. One use case, though atypical, for create_module() is to
        // provide a module that is a subclass of the builtin module type. Most loaders will not need to implement
        // create_module().

        // create_module() should properly handle the case where it is called more than once for the same spec/module. This
        // may include returning None or raising ImportError.
        py::none create_module(py::object spec);

        // PEP-451 loader.exec_module() method for the ``sys.meta_path`` hook.

        // Loaders will have a new method, exec_module(). Its only job is to "exec" the module and consequently populate
        // the module's namespace. It is not responsible for creating or preparing the module object, nor for any cleanup
        // afterward. It has no return value. exec_module() will be used during both loading and reloading.

        // exec_module() should properly handle the case where it is called more than once. For some kinds of modules this
        // may mean raising ImportError every time after the first time the method is called. This is particularly relevant
        // for reloading, where some kinds of modules do not support in-place reloading.
        void exec_module(py::module_ module);
};


void register_memory_importer(py::module_ &m);