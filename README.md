<div align="center">
  💾⤵
</div>
<h1 align="center">
  Memory-Importer
</h1>

<p align="center">
    Memory-Importer is an experimental tool demonstrates that how to import python libraries from <b>MEMORY</b> under <a href="https://peps.python.org/pep-0451/">PEP 451</a>
</p>
<br/>

---
![github-stars][stars-badge]

| CI                 | status                                                                |
| ------------------ | --------------------------------------------------------------------- |
| Linux/macOS Travis | [![Travis-CI][travis-badge]][travis-link]                             |
| MSVC 2019          | [![AppVeyor][appveyor-badge]][appveyor-link]                          |
| pip builds         | [![Pip Actions Status][actions-pip-badge]][actions-pip-link]          |
| [`cibuildwheel`][] | [![Wheels Actions Status][actions-wheels-badge]][actions-wheels-link] |

[gitter-badge]:            https://badges.gitter.im/pybind/Lobby.svg
[stars-badge]:             https://img.shields.io/github/stars/shabbywu/python-memory-importer?style=social
[actions-badge]:           https://github.com/shabbywu/python-memory-importer/workflows/Tests/badge.svg
[actions-pip-link]:        https://github.com/shabbywu/python-memory-importer/actions?query=workflow%3A%22Pip
[actions-pip-badge]:       https://github.com/shabbywu/python-memory-importer/workflows/Pip/badge.svg
[actions-wheels-link]:     https://github.com/shabbywu/python-memory-importer/actions?query=workflow%3AWheels
[actions-wheels-badge]:    https://github.com/shabbywu/python-memory-importer/workflows/Wheels/badge.svg
[travis-link]:             https://travis-ci.org/shabbywu/python-memory-importer
[travis-badge]:            https://travis-ci.org/shabbywu/python-memory-importer.svg?branch=master&status=passed
[appveyor-link]:           https://ci.appveyor.com/project/shabbywu/python-memory-importer
<!-- TODO: get a real badge link for appveyor -->
[appveyor-badge]:          https://travis-ci.org/shabbywu/python-memory-importer.svg?branch=master&status=passed
[`cibuildwheel`]:          https://cibuildwheel.readthedocs.io


Installation
------------

- `pip install memory-importer`

Requirements
------------
CMake for building, and pybind11 and [physfs.py](https://github.com/shabbywu/physfs.py.git)

Features
------------

`memory-importer` implement a class called **PhysfsImporter** follow the `finder` and `loader` protocol which are proposed by [PEP-451](https://peps.python.org/pep-0451/).

After append `PhysfsImporter` into **sys.meta_path**, You can import any pure python code from an archive stored in memory( or dicks).

> PhysfsImporter use `physfs.py` to easily visit entry in achieve.


Test call
---------

Case 1: import from an archive stored in disk.

```python
import sys
from memory_importer import PhysfsImporter

# init importer and mount tests/assets/py.zip into memory
i = PhysfsImporter("tests/assets/py.zip")

# append it into sys.meta_path to notice python use it to import library
sys.meta_path.insert(0, i)

# test import and code
import single
assert str(single) == "<module 'single' (physfs://single.py)>"
assert single.flag() == "single"
```


Case 2: import from archive stored in memory.
```python
import sys
from memory_importer import PhysfsImporter, physfs

# init physfs
physfs.init()
# load archive into memory
with open("tests/assets/py.zip", mode="rb") as fh:
    archive = fh.read()

# mount archive into physfs
physfs.mount_memory(archive, "py.zip", "/")

# init importer
i = PhysfsImporter()

# append it into sys.meta_path to notice python use it to import library
sys.meta_path.insert(0, i)

# test import and code
import single
assert str(single) == "<module 'single' (physfs://single.py)>"
assert single.flag() == "single"
```

What can it be used for?
---------
1. Provide a better way to demonstrate how [PEP-451](https://peps.python.org/pep-0451/) work.
2. You can embed your pure python code with your cpp python library.
3. Waiting for you to discover...