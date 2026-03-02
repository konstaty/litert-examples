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

git clone https://github.com/konstaty/litert-examples.git
git submodule update --init --checkout

Installing dependencies
=======================


Configuring & Building
======================

mkdir build
cmake -B build .
cmake --build build --parallel 4



https://ai.google.dev/edge/api/litert/c/classlitert/1-1-tensor-buffer