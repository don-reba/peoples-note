@echo off

pushd ..

set dst=Codeplex
set opt=/s /xf *.swp *.swo /njh /njs /nfl /ndl
set winCEopt=/xd "Windows Mobile 6 Professional SDK (ARMV4I)" "Windows Mobile 5.0 Pocket PC SDK (ARMV4I)" /xf *.user *.aps
set win32opt=/xd Debug Release /xf *.user

robocopy "src\client"       "%dst%\src\client"       %opt% %winCEopt%
robocopy "src\MobileTest"   "%dst%\src\MobileTest"   %opt% %winCEopt%
robocopy "src\Test"         "%dst%\src\Test"         %opt% %win32opt%
robocopy "src\CallDump"     "%dst%\src\CallDump"     %opt% %winCEopt%
robocopy "src\Evernote API" "%dst%\src\Evernote API" %opt% %winCEopt%
robocopy "src\HTMLayout"    "%dst%\src\HTMLayout"    %opt%

xcopy/q/y "src\Evernote client.sln" "%dst%\src"

move/y "%dst%\src\client\src\API Key (placeholder).h" "%dst%\src\client\src\API Key.h"

robocopy "gfx" "%dst%\gfx" *.svg %opt%
robocopy "gfx\icons" "%dst%\gfx\icons" %opt%

mkdir "%dst%\doc"
xcopy/q/y "doc\readme.txt"       "%dst%\doc"
xcopy/q/y "doc\user stories.txt" "%dst%\doc"

popd
