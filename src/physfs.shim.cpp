#include "physfs.shim.h"
#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#define PHYSFS_NO_CDROM_SUPPORT 1

#if ((defined __MACH__) && (defined __APPLE__))
#include <Foundation/Foundation.h>
#elif (defined(_MSC_VER) && (_MSC_VER >= 1700) && !_USING_V110_SDK71_)  /* _MSC_VER==1700 for MSVC 2012 */
#include <windows.h>
#include <shlobj.h>
#elif (((defined _WIN32) || (defined _WIN64)) && (!defined __CYGWIN__))
#include <windows.h>
#include <shlobj.h>
#endif

namespace py = pybind11;


namespace memory_importer {
    #include "miniphysfs.h"

    namespace physfs {
        template<typename ... Args>
        std::string string_format( const std::string& format, Args ... args )
        {
            int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
            if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
            auto size = static_cast<size_t>( size_s );
            std::unique_ptr<char[]> buf( new char[ size ] );
            snprintf( buf.get(), size, format.c_str(), args ... );
            return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        }


        struct PhysfsRuntimeError: std::runtime_error {
            typedef std::runtime_error super;
        public:
            PhysfsRuntimeError(std::string message): std::runtime_error (message) {};

            const char * what () const throw ()
            {
                return super::what();
            }
        };


        class PhysfsNotFound : public py::value_error
        {
            std::string buf;
            public:
            PhysfsNotFound(std::string message) {
                buf = "Not Found: '" + message + "'";
            };
            const char * what () const throw ()
            {
                return buf.c_str();
            }
        };


        struct PhysfsUninitialedError : public std::exception
        {
        const char * what () const throw ()
        {
            return "PHYSFS is not initialized, please call init() first";
        }
        };


        void physfs_init() {
            if (!PHYSFS_init(NULL)) {
                throw PhysfsRuntimeError(string_format("Failure. Reason: [%s]", PHYSFS_getLastError()));
            }
        }


        void physfs_deinit() {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            if (!PHYSFS_deinit()) {
                throw PhysfsRuntimeError(string_format("Failure. Reason: [%s]", PHYSFS_getLastError()));
            }
        }


        void physfs_mount(std::string source, std::string mountPoint = "/", py::bool_ appendToPath = py::bool_(0)) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            if (!PHYSFS_mount(source.c_str(), mountPoint.c_str(), appendToPath)) {
                throw PhysfsRuntimeError(string_format("Failure. Reason: [%s]", PHYSFS_getLastError()));
            }
        }

        void physfs_mount_memory(py::buffer buf, std::string bufName, std::string mountPoint = "/", py::bool_ appendToPath = py::bool_(0)) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }
            auto info = buf.request();
            if (!PHYSFS_mountMemory((const void*)info.ptr, (PHYSFS_uint64)info.itemsize * info.size, NULL, bufName.c_str(), mountPoint.c_str(), appendToPath)) {
                throw PhysfsRuntimeError(string_format("Failure. Reason: [%s]", PHYSFS_getLastError()));
            }
        }


        void physfs_unmount(std::string oldSource) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            if (!PHYSFS_unmount(oldSource.c_str())) {
                throw PhysfsRuntimeError(string_format("Failure. Reason: [%s]", PHYSFS_getLastError()));
            }
        }


        py::list enumerate_fs(std::string dir) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            char **rc;
            rc = PHYSFS_enumerateFiles(dir.c_str());

            py::list result;
            char **i;
            for (i = rc; *i != NULL; i++) {
                result.append(std::string(*i));
            }
            return result;
        }

        PHYSFS_Stat physfs_stat(std::string file) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            PHYSFS_Stat stat;

            if(!PHYSFS_stat(file.c_str(), &stat))
            {
                auto error_code = PHYSFS_getLastErrorCode();
                if (error_code == PHYSFS_ERR_NOT_FOUND) {
                    throw PhysfsNotFound(file);
                }
                throw PhysfsRuntimeError(string_format("failed to stat. Reason [%s]", PHYSFS_getLastError()));
            }
            return stat;
        }

        py::bytes physfs_cat(std::string file) {
            if (!PHYSFS_isInit()) {
                throw PhysfsUninitialedError();
            }

            PHYSFS_File *f;
            f = PHYSFS_openRead(file.c_str());
            if (f == NULL) {
                auto error_code = PHYSFS_getLastErrorCode();
                if (error_code == PHYSFS_ERR_NOT_FOUND) {
                    throw PhysfsNotFound(file);
                }
                throw PhysfsRuntimeError(string_format("failed to open. Reason: [%s]", PHYSFS_getLastError()));
            }

            std::stringstream output;
            while (1)
            {
                char buffer[1024];
                PHYSFS_sint64 rc;
                PHYSFS_sint64 i;
                rc = PHYSFS_readBytes(f, buffer, sizeof (buffer));
                if (rc == -1 ) {
                    PHYSFS_close(f);
                    throw PhysfsRuntimeError(string_format("Error condition in reading. Reason: [%s]", PHYSFS_getLastError()));
                }

                output.write(buffer, rc);
                if (rc < sizeof (buffer))
                {
                    if (!PHYSFS_eof(f))
                    {
                        PHYSFS_close(f);
                        throw PhysfsRuntimeError(string_format("Error condition in reading. Reason: [%s]", PHYSFS_getLastError()));
                    }
                    PHYSFS_close(f);
                    break;
                }
            }
            return py::bytes(output.str());
        }


        void register_physfs(py::module_ &m) {
            m.doc() = R"pbdoc(
                physfs - PhysicsFS is a library to provide abstract access to various archives.
                -----------------------
                .. currentmodule:: physfs
            )pbdoc";

            m.def("init", &physfs_init, R"pbdoc(
                Initialize the PhysicsFS library.

            * This must be called before any other PhysicsFS function.
            *
            * This should be called prior to any attempts to change your process's
            *  current working directory.

            )pbdoc");

            m.def("deinit", &physfs_deinit, R"pbdoc(
                Deinitialize the PhysicsFS library.

            * This closes any files opened via PhysicsFS, blanks the search/write paths,
            *  frees memory, and invalidates all of your file handles.
            *
            * Note that this call can FAIL if there's a file open for writing that
            *  refuses to close (for example, the underlying operating system was
            *  buffering writes to network filesystem, and the fileserver has crashed,
            *  or a hard drive has failed, etc). It is usually best to close all write
            *  handles yourself before calling this function, so that you can gracefully
            *  handle a specific failure.
            *
            * Once successfully deinitialized, PHYSFS_init() can be called again to
            *  restart the subsystem. All default API states are restored at this
            *  point, with the exception of any custom allocator you might have
            *  specified, which survives between initializations.

            )pbdoc");

            m.def("mount", &physfs_mount, R"pbdoc(
                Add an archive or directory to the search path.

            * \param source directory or archive to add to the path, in
            *                   platform-dependent notation.
            * \param mountPoint Location in the interpolated tree that this archive
            *                   will be "mounted", in platform-independent notation.
            *                   NULL or "" is equivalent to "/".
            * \param appendToPath nonzero to append to search path, zero to prepend.
            )pbdoc", py::arg("source").none(false), py::arg("mountPoint") = "/", py::arg("appendToPath") = py::bool_(0));

            m.def("mount_memory", &physfs_mount_memory, R"pbdoc(
                Add an archive, contained in a memory buffer, to the search path.

            *   \param buf Address of the memory buffer containing the archive data.
            *   \param bufName Filename that can represent this stream.
            *   \param mountPoint Location in the interpolated tree that this archive
            *                     will be "mounted", in platform-independent notation.
            *                     NULL or "" is equivalent to "/".
            *   \param appendToPath nonzero to append to search path, zero to prepend.
            )pbdoc", py::arg("buf").none(false), py::arg("bufName").none(false), py::arg("mountPoint") = "/", py::arg("appendToPath") = py::bool_(0));

            m.def("unmount", &physfs_unmount, R"pbdoc(
                Remove a directory or archive from the search path.

            * This must be a (case-sensitive) match to a dir or archive already in the
            *  search path, specified in platform-dependent notation.
            *
            * This call will fail (and fail to remove from the path) if the element still
            *  has files open in it.
            *
            * \param oldSource dir/archive to remove.

            )pbdoc", py::arg("oldSource").none(false));

            m.def("ls", &enumerate_fs, R"pbdoc(
                Get a file listing of a search path's directory.
            )pbdoc", py::arg("dir") = "/");

            m.def("stat", &physfs_stat, R"pbdoc(
                Get various information about a directory or a file.
            )pbdoc", py::arg("file").none(false));

            m.def("cat", &physfs_cat, R"pbdoc(
                Read bytes from a PhysicsFS filehandle
            )pbdoc", py::arg("file").none(false));

            m.def("read", &physfs_cat, R"pbdoc(
                Read bytes from a PhysicsFS filehandle, read is alias of cat.
            )pbdoc", py::arg("file").none(false));

            py::class_<PHYSFS_Stat>(m, "PHYSFS_Stat", py::module_local())
            .def_readwrite("filetype", &PHYSFS_Stat::filetype)
            .def_readwrite("filesize", &PHYSFS_Stat::filesize)
            .def_readwrite("modtime", &PHYSFS_Stat::modtime)
            .def_readwrite("createtime", &PHYSFS_Stat::createtime)
            .def_readwrite("accesstime", &PHYSFS_Stat::accesstime)
            ;

            py::enum_<PHYSFS_FileType>(m, "PHYSFS_FileType", py::module_local())
                .value("PHYSFS_FILETYPE_REGULAR", PHYSFS_FileType::PHYSFS_FILETYPE_REGULAR)
                .value("PHYSFS_FILETYPE_DIRECTORY", PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY)
                .value("PHYSFS_FILETYPE_SYMLINK", PHYSFS_FileType::PHYSFS_FILETYPE_SYMLINK)
                .value("PHYSFS_FILETYPE_OTHER", PHYSFS_FileType::PHYSFS_FILETYPE_OTHER)
            ;


            py::register_exception<PhysfsRuntimeError>(m, "PhysfsRuntimeError");
            py::register_exception<PhysfsNotFound>(m, "PhysfsNotFound");
            py::register_exception<PhysfsUninitialedError>(m, "PhysfsUninitialedError");
        }
    }
}
