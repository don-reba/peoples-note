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
