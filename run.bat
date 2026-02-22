@echo off
echo Starting SecureFileMatcher Server...
cd src
start "SecureFileMatcher Server" SecureFileMatcher.exe

echo Opening GUI...
cd ..
cd web
start index.html

echo Done! The server is running in the pop-up window. Do not close it until you are finished.
pause
