echo Executing post-build step...

set PROJECT_DIR=%1
set OUTPUT_DIR=%2

echo copying DLLs to output directory...

xcopy "%PROJECT_DIR%bin\*" "%OUTPUT_DIR%" /D /Y /I