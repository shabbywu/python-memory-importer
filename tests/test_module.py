from memory_importer import PhysfsImporter


def test(basedir):
    i = PhysfsImporter(basedir + "assets/py.zip")
    import module

    assert module.part1.flag() == "part1", "part1 is loaded"
    assert "part2" not in module.__dict__, "lazy part will not auto import"
    assert "namespace" not in module.__dict__, "lazy part will not auto import"

    from module import part2

    assert "part2" in module.__dict__, "import submodule should add ref in parent"
    assert module.part2 is part2, "ref should eq"
    assert part2.flag() == "part2", "part2 is loaded"


if __name__ == "__main__":
    test()
