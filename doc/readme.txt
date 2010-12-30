-----------------
Project Structure
-----------------

doc
	The project's documentation.

gfx
	Graphics asset sources.

"src\client"
	Source code for the main People's Note client project.

"src\client\src\rapidxml"
	Our custom version of RapidXML. The official version contains a known crash bug.

"src\MobileTest"
	Those unit tests for People's Note that need to run in a WinCE environment.

"src\Test"
	Those unit tests for People's Note that can run in a Win32 environment.

"src\CallDump"
	Library used for profiling People's Note.

"src\HTMLayout"
	Headers and binaries for the HTMLayout library used by People's Note.

"src\Evernote API"
	Win32/C++ port of the Evernote API and Apache Thrift.

"src\Thrift Generate"
	Win32/C++ Thirft code generator

------------
Requirements
------------

Visual Studio 2008
Windows Mobile 6.5 Developer Tool Kit
Boost 1.41.0
Inkscape 0.48

----------------
Evernote Support
----------------

In order to enable this, you need to get a developer API key from Evernote and set it in "src\client\src\API Key.h". Without it, the program will build and run, but won't be able to connect to Evernote's servers.

-----------------------
Evernote API and Thrift
-----------------------

To use the API outside of People's Note, you first have to build the "Evernote API" library. Choose the build target as "Win 32" or "Windows Mobile", whichever is appropriate for you. Build the library and copy "Evernote API.lib" with the "include" folder to your application. Evernote API does not require any of the Boost libraries. See NoteStore.cpp and UserStore.cpp for examples of using the API.

To use our Win32 C++ Thrift port for another API, you need the code generator from "src\Thrift Generate". The Thrift code generator itself requires Cygwin to run on Windows, but the generated API does not. Let me know, if you have problems with it, find it useful, or make improvements.
