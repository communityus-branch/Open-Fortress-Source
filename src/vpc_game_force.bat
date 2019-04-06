@echo off

devtools\bin\vpc.exe /ofd +game /mksln jaykinbacon.sln /2013 /f

if errorlevel 1 (
	pause
)

exit