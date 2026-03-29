g++ sudoku_solver.cpp -std=c++17 -O3 -march=native -flto -funroll-loops -fomit-frame-pointer -fno-exceptions -fno-rtti -fno-stack-protector -ffast-math -fstrict-aliasing -frename-registers -DNDEBUG -o run.exe Sudoku.cpp

run.exe
pause