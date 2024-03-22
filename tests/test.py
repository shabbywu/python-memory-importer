from pathlib import Path
import physfs

rootdir = Path(__file__).parent
assert physfs.__version__ == '0.1.0'

physfs.init()
assert physfs.ls("/") == []

physfs.mount(str(rootdir / "./example.zip"))
assert physfs.ls("/") == ["example"]
assert physfs.cat("example/flag") == b"OqP$IwK7eiZyTWk4Xi3jYECi^IXLdZw1u@H5wOo4\n"

physfs.unmount(str(rootdir / "./example.zip"))
assert physfs.ls("/") == []

physfs.deinit()

E = None
try:
    physfs.ls()
except RuntimeError as e:
    E = e
assert E is not None
