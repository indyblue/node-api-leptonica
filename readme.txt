https://nodejs.org/api/addons.html

Usage: node-gyp <command> [options]
where <command> is one of:
  - build - Invokes `make` and builds the module
  - clean - Removes any generated build files and the "out" dir
  - configure - Generates a Makefile for the current module
  - rebuild - Runs "clean", "configure" and "build" all at once
  - install - Install node development files for the specified node version.
  - list - Prints a listing of the currently installed node development files
  - remove - Removes the node development files for the specified version

A version of the node-gyp utility is bundled and distributed with Node.js as part of npm. This version is not made directly available for developers to use and is intended only to support the ability to use the npm install command to compile and install addons. Developers who wish to use node-gyp directly can install it using the command npm install -g node-gyp. See the node-gyp installation instructions for more information, including platform-specific requirements.

- Once the binding.gyp file has been created, use node-gyp configure to generate the appropriate project build files for the current platform
- Next, invoke the node-gyp build command to generate the compiled addon.node file. This will be put into the build/Release/ directory.


