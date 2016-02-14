# CG1
This is a project for the exercises of the course Computer Graphics.
****
## Build process
#### Windows
##### Setup
This project uses **Visual Studio Community 2015** for debugging and its
compiler and linker by using **cl.exe**. Editor is free choice.

**Note:** If you are using a different version of Visual Studio or a different
compiler & linker you need to adapt the .bat files in the *./build* directory.
Depending on your tools or setup you can discard *./build/startup.bat*.
##### Steps
* **Initialize** compiler with *./build/startup.bat*
    * This needs to be done every time you open a terminal in which you will
    build the project.
* **Build** with *./build/build.bat*.
* **Debug** with *./build/debug.bat*.

#### Mac OS X
Currently not tested.

#### Linux
Currently not tested.