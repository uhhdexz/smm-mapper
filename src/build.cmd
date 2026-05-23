@echo off
setlocal

cd /d "%~dp0.."

where cl >nul 2>nul
if errorlevel 1 (
    echo Run this from an x64 Visual Studio developer command prompt.
    exit /b 1
)

set "OUT=Work\build"
set "WINOUT=%OUT%\windows"
mkdir "%WINOUT%" 2>nul

set "EFI_CL=/nologo /c /O1 /Oi /GS- /GR- /EHs-c- /Zl /W4"
set "EFI_LD=/nologo /dll /nodefaultlib /machine:x64 /subsystem:EFI_BOOT_SERVICE_DRIVER /fixed:no /dynamicbase:no /nxcompat:no"

cl %EFI_CL% /Fo:%OUT%\DxeBridge.obj src\DxeBridge.c || exit /b 1
link %EFI_LD% /entry:DxeBridgeEntry /out:%OUT%\DxeBridge.efi %OUT%\DxeBridge.obj || exit /b 1

cl %EFI_CL% /Fo:%OUT%\SmmHost.obj src\SmmHost.c || exit /b 1
link %EFI_LD% /entry:SmmHostEntry /out:%OUT%\SmmHost.efi %OUT%\SmmHost.obj || exit /b 1

cl %EFI_CL% /Fo:%OUT%\Payload.obj src\Payload.c || exit /b 1
link %EFI_LD% /entry:PayloadEntry /out:%OUT%\Payload.efi %OUT%\Payload.obj || exit /b 1

set "SDKVERSION=%WindowsSDKVersion:\=%"
if "%SDKVERSION%"=="" set "SDKVERSION=10.0.26100.0"
if "%WindowsSdkDir%"=="" set "WindowsSdkDir=C:\Program Files (x86)\Windows Kits\10\"
set "SDKINC=%WindowsSdkDir%Include\%SDKVERSION%"
set "SDKLIB=%WindowsSdkDir%Lib\%SDKVERSION%"

cl /nologo /W4 /O2 /DUNICODE /D_UNICODE /Fo:%WINOUT%\SmmClient.obj /Fe:%WINOUT%\SmmClient.exe src\windows\SmmClient.c Advapi32.lib || exit /b 1

cl /nologo /c /W4 /GS- /kernel /Gz /D_AMD64_=1 /DWINVER=0x0A00 /D_WIN32_WINNT=0x0A00 /DNTDDI_VERSION=0x0A00000B /DWINNT=1 /D_WIN64 /D_NTDRIVER_ /I"%SDKINC%\km" /I"%SDKINC%\shared" /I"%SDKINC%\km\crt" /Fo:%WINOUT%\SmmClientDrv.obj src\windows\SmmClientDrv.c || exit /b 1
link /nologo /driver /subsystem:native /machine:x64 /entry:DriverEntry /out:%WINOUT%\SmmClient.sys %WINOUT%\SmmClientDrv.obj "%SDKLIB%\km\x64\ntoskrnl.lib" "%SDKLIB%\km\x64\hal.lib" "%SDKLIB%\km\x64\BufferOverflowK.lib" || exit /b 1

echo Built:
echo   %OUT%\DxeBridge.efi
echo   %OUT%\SmmHost.efi
echo   %OUT%\Payload.efi
echo   %WINOUT%\SmmClient.exe
echo   %WINOUT%\SmmClient.sys