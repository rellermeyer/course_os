#!/usr/bin/python

"""
Some docstring
"""
import random
import glob
import pathlib
import sys

from jinja2 import Template


class Test:
    name: str = ""
    code: str = ""

    def __init__(self, test: str):
        partitioned = test.partition(',')
        self.name = partitioned[0][1:]
        self.code = partitioned[2]

    def __str__(self) -> str:
        return self.name


class TestGroup:
    name: str = ""
    tests: list[Test] = []

    def __init__(self, name: str, test_file: str):
        self.tests = []
        for test in test_file.split("TEST_CREATE")[1:]:
            self.tests.append(Test(test))

    def __str__(self) -> str:
        return self.name

    def get_test_names(self) -> list[str]:
        return (test.name for test in self.tests)


def generate_groups() -> list[TestGroup]:
    test_groups = []

    for directory in glob.glob("src/**/test/", recursive=True):
        for file in pathlib.Path(directory).rglob('*.c'):
            with open(file, 'r') as f:
                test_groups.append(TestGroup(f.name, f.read()))
    return test_groups


def get_test_names(groups: list[TestGroup]) -> list[str]:
    test_names = []
    for group in groups:
        test_names.extend(group.get_test_names())
    return test_names


if __name__ == "__main__":
    groups = generate_groups()
    test_names = get_test_names(groups)
    random.seed(int(sys.argv[1]))

    test_content = ""
    with open("src/test/test.tmpl", "r") as f:
        template = Template(f.read())
        random.shuffle(test_names)
        test_content = template.render(tests=test_names)

    with open("src/test/test.c", "w") as f:
        f.write(test_content)
