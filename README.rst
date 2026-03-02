The project as a whole is distributed under the GNU General Public License (GPL) v. 3,
due to its dependency on Qt.

However, files from the **core** can be reused under BSD License.

.. image:: gui.png
   :alt: GUI main window
   :align: center


********
Building
********

Obtaining sources
=================

| git clone https://github.com/konstaty/litert-examples.git
| cd litert-examples
| git submodule update --init --checkout


Installing dependencies
=======================


Configuring & Building
======================

| mkdir build
| cmake -B build .
| cmake --build build --parallel 4


Usage
=====

Place images into **./images** directory.

Place TFLite models into **./models** directory.

Start apllication: ``./build/lrex``

Use the **Process** and **Next** buttons to run inference on the current image and the next image, respectivelly.



LiteRT API Reference: https://ai.google.dev/edge/api