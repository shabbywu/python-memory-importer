from memory_importer import PhysfsImporter





def test(basedir):
    i = PhysfsImporter(str(basedir / "assets" / "py.zip"))
    from module import namespace

    assert "part3" not in namespace.__dict__, "lazy part will not auto import"

    from module.namespace import part3

    assert "part3" in namespace.__dict__, "import submodule should add ref in parent"
    assert namespace.part3 is part3, "ref should eq"
    assert part3.flag() == "part3", "part3 is loaded"


if __name__ == "__main__":
    test()
