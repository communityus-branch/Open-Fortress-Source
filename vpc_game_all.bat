@echo off

devtools\bin\vpc.exe /ofd +game /mksln openfortress_all.sln /2013

if errorlevel 1 (
	pause
)

exit