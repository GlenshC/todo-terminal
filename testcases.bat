@echo off
setlocal EnableDelayedExpansion

:: ==== CONFIGURATION ====
set OFFSET_DAYS=5

:: ==== CALCULATE FUTURE DEADLINES ====
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS%).ToString('dd/MM/yyyy')"') do set D1=%%i
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS% + 2).ToString('dd/MM/yyyy')"') do set D2=%%i
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS% + 3).ToString('dd/MM/yyyy')"') do set D3=%%i
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS% + 4).ToString('dd/MM/yyyy')"') do set D4=%%i
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS% + 6).ToString('dd/MM/yyyy')"') do set D5=%%i
for /f %%i in ('powershell -command "(Get-Date).AddDays(%OFFSET_DAYS% + 7).ToString('dd/MM/yyyy')"') do set D6=%%i

:: ==== ADD TODOS WITH CORRECT PRIORITY VALUES ====
call todo add "Submit math assignment" -d "Calculus HW due next week" -D !D1! -p 1
call todo add "Study for CS midterm" -d "Review linked lists and hash tables" -D !D2! -p 2
call todo add "Buy groceries" -d "Milk, eggs, bread" -D !D3! -p 3
call todo add "Watch Netflix" -d "Continue Stranger Things" -p 4
call todo add "Read OpenGL tutorial" -d "Chapter 5: Shaders" -p 2
call todo add "Clean up download folder" -d "Remove unused files" -p 3
call todo add "Call mom" -d "Check in and catch up" -D !D4! -p 3
call todo add "Fix project build error" -d "Undefined reference in linker step" -D !D5! -p 1
call todo add "Plan weekend trip" -d "Look for hiking trails near the city" -p 3
call todo add "Scroll Instagram" -d "Just 5 minutes... (yeah right)" -p 4

echo.
echo All sample todos added with correct priorities and a deadline offset of %OFFSET_DAYS% days!
pause
