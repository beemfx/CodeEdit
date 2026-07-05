echo Rebuilding distribution...

call "..\ext\BuildDistribution\_Clean.bat" %1 %2
call "..\ext\BuildDistribution\_Build.bat" %1 %2
