from memory_importer import physfs


def test(basedir):
    with open(basedir + "assets/py.zip", mode="rb") as fh:
        buf = fh.read()
    physfs.mount_memory(buf, "py.zip")
    import single
    assert single.flag() == "single"
