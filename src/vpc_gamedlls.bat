@echo off

devtools\bin\vpc.exe /ofd +gamedlls /mksln jaykinbacon.sln /2013

if errorlevel 1 (
	pause
)

exit