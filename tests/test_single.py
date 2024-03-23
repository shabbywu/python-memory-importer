from memory_importer import PhysfsImporter


def test(basedir):
    i = PhysfsImporter(str(basedir / "assets" / "py.zip"))
    import single
    assert single.flag() == "single"



if __name__ == "__main__":
    test()
