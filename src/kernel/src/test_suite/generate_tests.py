#!/bin/env python3

"""
A Python port of the old test suite written by Jonathan Dönszelmann
(@jonay2000) and Victor Roest (@nullx76) in shell for the course_os
project of 2020. It was ported by Valentijn van de Beek
(@ValentijnvdBeek) for the 2021 edition of the course.
"""
import shutil
import random
import glob
import pathlib
import sys

from jinja2 import Environment, FileSystemLoader

TESTDIR = "src/test_suite/"
GENDIR = "build/generated/"
SEARCHDIR = "src/**/test/"


class Test:
    """
    This class defines a wrapper around the C processor style
    TEST_CREATE macro.


    Args:
       test: C source code of the test and anything before the
          next test.

    Attributes:
      name: Name of the specific test
      code: C code corresponding to the test
      additional: Code which is after the test, which due to
          splitting was appended to this test case. It would
          be nicer in the future to deal with this in the
          group class instead.

    Example:
        TEST_CREATE(example, {
               int a = 2 + 2;
               ASSERT_EQ(a, 5);
         });
         int useful_help(const bool t);

        TEST_CREATE($name, { $code });
        $additional
    """
    name: str = ""
    code: str = ""
    additional: str = ""

    def __init__(self, test: str):
        name_partitioned = test.partition(',')
        self.name = name_partitioned[0][1:]

        # Get the argument after the first comma, take out the brace
        # and split into two parts. The part before the test and the
        # part after.
        code_partitioned = (name_partitioned[2].strip())[2:].rpartition("})")
        self.code = code_partitioned[0][:-1]  # Take out newline
        self.additional = code_partitioned[2].strip()

    def __str__(self) -> str:
        return self.name


class TestGroup:
    """
    A TestGroup is a logical set of tests which are all executed in
    sequential order at the same time. Currently, this is only done
    for files where each files defines a singular and unique group.

    Attributes:
       name: Identifier for the test group, currently the module
         plus the file name
       preamble: Piece of code before the first test, typically headers
         variables and prototypes
       tests: List of all the tests defined for this logical group

    Args:
       name: Name of of the group, currently the module plus the file
        name
       test_file: UTF-8 plaintext of the test file.
    """
    name: str = ""
    preamble: str = ""
    tests: list[Test] = []

    def __init__(self, name: str, test_file: str):
        self.name = name
        self.tests = []

        # Split the file into the test cases
        split = test_file.split("TEST_CREATE")

        self.preamble = split[0].strip()
        for test in split[1:]:
            self.tests.append(Test(test))

    def __str__(self) -> str:
        return self.name

    def get_test_names(self) -> list[str]:
        """ Returns a list of all the test names """
        return (test.name for test in self.tests)

    def get_test_code(self) -> list[str]:
        """ Returns a list of all the test code """
        return (test.code for test in self.tests)


def generate_groups() -> list[TestGroup]:
    """
    Find all the groups, or files, in test directories throughout
    the project.

    Returns:
       A list of TestGroups containing all the tests in each test
       for the entire project.
    """
    test_groups = []

    for directory in glob.glob(SEARCHDIR, recursive=True):
        for file in pathlib.Path(directory).rglob('*.c'):
            with open(file, 'r') as f:
                p = pathlib.Path(f.name)
                # Add the name of the module to the name of the
                # TestGroup. For example: fs/tmpfs/inodes/corruption.c
                # -> fs_tmpfs_inodes_corruption.c
                name = '_'.join(p.parts[1:-2]) + "_" + p.stem
                group = TestGroup(name, f.read())
                test_groups.append(group)

    return test_groups


def get_tests(groups: list[TestGroup]) -> list[Test]:
    """
    Get all the tests defined in each of the groups.
    """
    tests = []
    for group in groups:
        tests.extend(group.tests)
    return tests


if __name__ == "__main__":
    argument = 0
    try:
        argument = int(sys.argv[1])
        random.seed(argument)
    except IndexError:
        pass

    if argument == -1:
        import os
        os.chdir(os.environ['MESON_SOURCE_ROOT'])

    env = Environment(loader=FileSystemLoader(TESTDIR))

    groups = generate_groups()
    # Shuffle the groups to find more obscure bugs, should
    # probably be offloaded to the kernel.
    random.shuffle(groups)
    tests = get_tests(groups)

    # Remove all the prior generated tests to ensure a clean state
    try:
        shutil.rmtree(GENDIR)
    except FileNotFoundError:
        pass

    pathlib.Path(GENDIR).mkdir(parents=True)

    for group in groups:
        name = GENDIR + group.name + ".c"
        # Write each unit into a C file
        with open(name, 'w') as f:
            template = env.get_template("test_group.tmpl")
            f.write(template.render(group=group))
        print(name)

    template = env.get_template("test_main.tmpl")
    test_content = template.render(groups=groups)

    with open(GENDIR + "test.c", "w") as f:
        f.write(test_content)
    print(GENDIR + "test.c")
