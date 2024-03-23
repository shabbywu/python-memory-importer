from memory_importer import physfs


def test(basedir):
    buf = (basedir / "assets" / "py.zip").read_bytes()
    physfs.mount_memory(buf, "py.zip")
    import single
    assert single.flag() == "single"
