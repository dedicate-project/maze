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

To build the project, navigate to the root directory of the project and run the following commands:

.. code-block:: bash

    mkdir build
    cd build
    cmake ..
    make

This will create the ``maze-bin`` executable in the ``bin`` directory of the build directory.

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
