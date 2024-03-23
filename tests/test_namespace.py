import sys
from memory_importer import *

i = PhysfsImporter("assets/py.zip")
sys.meta_path.insert(3, i)


from module import namespace

assert "part3" not in namespace.__dict__, "lazy part will not auto import"

from module.namespace import part3
assert "part3" in namespace.__dict__, "import submodule should add ref in parent"
assert namespace.part3 is part3, "ref should eq"
assert part3.flag() == "part3", "part3 is loaded"