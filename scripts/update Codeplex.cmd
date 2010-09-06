@echo off

pushd ..

set dst=Codeplex
set opt=/s /xf *.swp *.swo *.exe /njh /njs /nfl /ndl
set winCEopt=/xd "Windows Mobile 6 Professional SDK (ARMV4I)" /xf *.user
set win32opt=/xd Debug Release

rmdir/s/q %dst%
mkdir %dst%

robocopy "src\client"       "%dst%\src\client"       %opt% %winCEopt%
robocopy "src\MobileTest"   "%dst%\src\MobileTest"   %opt% %winCEopt%
robocopy "src\Test"         "%dst%\src\Test"         %opt% %win32opt%
robocopy "src\CallDump"     "%dst%\src\CallDump"     %opt% %winCEopt%
robocopy "src\Evernote API" "%dst%\src\Evernote API" %opt% %winCEopt%
robocopy "src\HTMLayout"    "%dst%\src\HTMLayout"    %opt%

xcopy/q/y "src\Evernote client.sln" "%dst%\src"

robocopy "gfx" "%dst%\gfx" *.svg %opt%
robocopy "gfx\icons" "%dst%\gfx\icons" %opt%

mkdir "%dst%\doc"
xcopy/q/y "doc\readme.txt"       "%dst%\doc"
xcopy/q/y "doc\user stories.txt" "%dst%\doc"

popd
