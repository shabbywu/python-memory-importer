#pragma once
#include <pybind11/pybind11.h>
#include <physfs.h>
#include <iostream>
#include <sstream>
#include <string>


namespace py = pybind11;
void register_physfs(py::module_ &m);
