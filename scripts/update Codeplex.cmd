@echo off

pushd ..

set dst=Codeplex
set opt=/s /xf *.swp *.swo *.exe /njh /njs /nfl /ndl

rmdir/s/q %dst%
mkdir %dst%

robocopy "src\client" "%dst%\src\client" %opt% /xd "Windows Mobile 6 Professional SDK (ARMV4I)" /xf *.user

mkdir "%dst%\doc"
xcopy/q/y "doc\thoughts.txt"     "%dst%\doc"
xcopy/q/y "doc\user stories.txt" "%dst%\doc"

popd
