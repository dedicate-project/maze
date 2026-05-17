Maze Game Library
=================

.. image:: https://circleci.com/gh/dedicate-project/maze/tree/main.svg?style=shield
    :target: https://circleci.com/gh/dedicate-project/maze/tree/main

.. image:: https://github.com/dedicate-project/maze/actions/workflows/codeql.yml/badge.svg?branch=main
   :target: https://github.com/dedicate-project/maze/actions/workflows/codeql.yml?branch=main


This is a maze game written in C++17. It uses a custom `Maze` class to represent the maze
(tile-based, 2d) and a player object to move through the maze. The player needs to maintain a food
level which is spent during movement (resource constraint) and can be replenished by picking up food
in the maze. The maze size as well as its difficulty (0.0 - 1.0, defines how many walls are placed
and how much food is available) can easily be set via parameters.

Usage
-----

The project pulls in Catch2 as a git submodule for the unit tests. Make sure submodules are
initialised before configuring:

.. code-block:: bash

    git submodule update --init --recursive

To build the project, navigate to the root directory of the project and run the following commands:

.. code-block:: bash

    cmake -S . -B build
    cmake --build build -j

This will create the ``maze-bin`` executable in the ``build/bin`` directory.

To run the unit test suite:

.. code-block:: bash

    ctest --test-dir build --output-on-failure

If you don't want to build the tests, pass ``-DMAZE_BUILD_TESTS=OFF`` at configure time.

Library
-------

The core functionality of the Maze game is provided as a library, which can be imported into other
CMake projects. The library is called ``maze`` and can be linked to other projects by adding the
following lines to their CMakeLists.txt file:

.. code-block:: cmake

    add_subdirectory(<path to maze project folder>)
    target_link_libraries(myproject maze)

License
-------

This code is released under the GPL-3 license.
