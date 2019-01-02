setlocal

echo "Setting visual studio variables"

@echo on

echo "Setting PATH and other variables"
set cur_dir=%CD%
set PATH=%PATH%;%PYTHON_PATH%

echo %cur_dir%
python Configure.py %CONFIGURE_OPTIONS% || goto :error

msbuild softhsm2.sln /p:Configuration="Release" /p:Platform="%MSBUILD_PLATFORM%" /p:PlatformToolset=v140 /target:Build || goto :error

cd %cur_dir%

echo "Testing build"

cd %from_dir%
cryptotest.exe || goto :error
datamgrtest.exe || goto :error
handlemgrtest.exe || goto :error
objstoretest.exe || goto :error
p11test.exe || goto :error
sessionmgrtest.exe || goto :error
slotmgrtest.exe || goto :error

echo "Preparing output package"
copy %from_dir%\softhsm2.dll %RELEASE_DIR% || goto :error
copy %from_dir%\softhsm2-dump-file.exe %RELEASE_DIR% || goto :error
copy %from_dir%\softhsm2-keyconv.exe %RELEASE_DIR% || goto :error
copy %from_dir%\softhsm2-util.exe %RELEASE_DIR% || goto :error
copy %cur_dir%\src\lib\common\softhsm2.conf.in %RELEASE_DIR%\softhsm2.conf || goto :error

dir %RELEASE_DIR%

@echo *** BUILD SUCCESSFUL ***
endlocal
@exit /b 0


:error
@echo *** BUILD FAILED ***
endlocal
@exit /b 1
