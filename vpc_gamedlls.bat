@echo off

devtools\bin\vpc.exe /ofd +gamedlls /mksln openfortress.sln /2013

if errorlevel 1 (
	pause
)

exit