def pytest_addoption(parser):
    parser.addoption(
        "--cmd", action="store", default=".", help="path to test binary"
    )
