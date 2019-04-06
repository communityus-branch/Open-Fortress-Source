@echo off

devtools\bin\vpc.exe /ofd +game /mksln openfortress.sln /2013

if errorlevel 1 (
	pause
)

exit