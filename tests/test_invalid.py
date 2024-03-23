from memory_importer import PhysfsImporter


def test(basedir):
    i = PhysfsImporter(basedir + "assets/py.zip")
    assert i.find_spec("Readme", None, None) is None, "Readme's ext is '.md'"
