@echo off
setlocal

REM Get current Unix timestamp
for /f "usebackq" %%i in (`powershell -Command "(Get-Date).ToUniversalTime().Subtract((Get-Date '1970-01-01')).TotalSeconds"`) do (
    set unix_timestamp=%%i
)

REM Convert Unix timestamp to hexadecimal
cmd /C exit %unix_timestamp%
set "HEX=0x%=ExitCode%"
echo Serial number: %HEX%


REM Call nrfjprog command with the hex variable
nrfjprog --memwr 0x10001080 --val %HEX%
nrfjprog --memrd 0x10001080

nrfjprog --memwr 0x10001084 --val 95
nrfjprog --memrd 0x10001084

set /p DUMMY=Hit ENTER to continue...
endlocal