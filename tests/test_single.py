import sys
from memory_importer import *

i = PhysfsImporter("assets/py.zip")
# sys.meta_path.append(i)
sys.meta_path.insert(3, i)


import single
assert single.flag() == "single"
