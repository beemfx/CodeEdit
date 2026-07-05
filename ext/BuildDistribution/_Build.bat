set BUILD_PLATFORM=%~1
set BUILD_CONFIG=%~2
set BUILD_DIR=%BUILD_PLATFORM%_%BUILD_CONFIG%

echo Building distribution for %BUILD_DIR%...

if (%BUILD_CONFIG%)==(Debug) (
	echo Configuration %BUILD_CONFIG% is not for distribution, ignoring.
	exit 0
)

call :MakeDirs
call :CopyBinaries
call :CopyHeaders
call :CopyLicense

exit 0

:MakeDirs
	echo Creating output directories...
	if not exist "..\_Dist" mkdir "..\_Dist"
	if not exist "..\_Dist\Bin" mkdir "..\_Dist\Bin"
	if not exist "..\_Dist\Bin\%BUILD_DIR%" mkdir "..\_Dist\Bin\%BUILD_DIR%"
	if not exist "..\_Dist\include" mkdir "..\_Dist\include"
exit /b 0

:CopyBinaries
	echo Copying binaries...
	xcopy "..\_Build\Bin\%BUILD_DIR%\*.dll" "..\_Dist\Bin\%BUILD_DIR%" /Y
	xcopy "..\_Build\Bin\%BUILD_DIR%\*.lib" "..\_Dist\Bin\%BUILD_DIR%" /Y
exit /b 0

:CopyHeaders
	xcopy "..\lexilla\include\*.h" "..\_Dist\include" /Y
	xcopy "..\scintilla\include\*.h" "..\_Dist\include" /Y
exit /b 0

:CopyLicense
	xcopy "..\lexilla\License.txt" "..\_Dist" /Y
	xcopy "..\README.md" "..\_Dist" /Y
exit /b 0
