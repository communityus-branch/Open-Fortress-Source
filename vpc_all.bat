@echo off

devtools\bin\vpc.exe /ofd +everything /mksln everything.sln

if errorlevel 1 (
	pause
)

exit