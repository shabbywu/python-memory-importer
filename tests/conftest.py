import pytest
import sys
from memory_importer import physfs
from memory_importer import PhysfsImporter


@pytest.fixture(autouse=True)
def _physfs():
    try:
        physfs.init()
    except:
        ...
    yield
    physfs.deinit()



@pytest.fixture(autouse=True, scope="session")
def _importer():
    i = PhysfsImporter()
    sys.meta_path.insert(3, i)
    yield
    sys.meta_path.remove(i)


@pytest.fixture()
def basedir():
    value = __file__.rsplit("/", 1)[0]
    if value == __file__:
        value = ""
    else:
        value += "/"
    return value