def pytest_addoption(parser):
    parser.addoption("--config", action="store", default="BAD_CONFIG")