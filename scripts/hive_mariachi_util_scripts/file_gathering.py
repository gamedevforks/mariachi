#!/usr/bin/python
# -*- coding: Cp1252 -*-

# Hive Mariachi Engine
# Copyright (C) 2008 Hive Solutions Lda.
#
# This file is part of Hive Mariachi Engine.
#
# Hive Mariachi Engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hive Mariachi Engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Hive Mariachi Engine. If not, see <http://www.gnu.org/licenses/>.

# __author__    = Lu�s Martinho <lmartinho@hive.pt>
# __version__   = 1.0.0
# __revision__  = $LastChangedRevision$
# __date__      = $LastChangedDate$
# __copyright__ = Copyright (c) 2008 Hive Solutions Lda.
# __license__   = GNU General Public License (GPL), Version 3

import os.path
import re

SOURCE_FILE_EXTENSIONS = ["cpp", "c", "mm"]
SOURCE_DIRS = ["src/hive_mariachi", "src/hive_mariachi_runner", "lib/libbullet/src", "lib/liblua/src"]
base_path = "none"

path_names = []

def compare(str1, str2):
    str1AtRoot = str1.find("/") == -1
    str2AtRoot = str2.find("/") == -1

    if str1AtRoot and not str2AtRoot:
        return -1
    if not str1AtRoot and str2AtRoot:
        return 1
    else:
        return cmp(str1, str2)

def visit(arg, dirname, names):
    valid_names = [valid_name for valid_name in names if valid_name.split(".")[-1] in SOURCE_FILE_EXTENSIONS]
    for valid_name in valid_names:
        total = dirname + "/" + valid_name
        total = total.replace(base_path + "/", "")
        total = total.replace("\\", "/")

        # iterates over all the source directories
        # to remove the them from the path string
        for value in SOURCE_DIRS:
            # removes the source directory from
            # the source path
            total = total.replace(value, "")

        # in case the path starts with a slash
        if total[0] == "/":
            # removes the slash
            total = total[1:]

        path_names.append(total + " \\")

for source_dir in SOURCE_DIRS:
    # resets the path names
    path_names = []

    # sets the global variable
    base_path = source_dir

    # walks the source dir
    os.path.walk(source_dir, visit, None)

    # sorts the path names
    path_names.sort(compare)

    # creates the file name for the output
    filename = source_dir.replace("/", ".")
    filename += ".txt"

    # opens the file
    file = open(filename, "w+")

    # determines the length of path names
    path_names_size = len(path_names)

    # prints out the path names
    for path_name_index in range(path_names_size):
        # retrieves the current path name
        path_name = path_names[path_name_index]

        # in case it's the last path
        if path_name_index == path_names_size - 1:
            # removes the appending slash
            file.write(path_name.replace("\\", ""))
        # otherwise
        else:
            file.write(path_name)

        file.write("\n")

    # adds new line at end of file
    file.write("\n")

    # closes the file
    file.close()
