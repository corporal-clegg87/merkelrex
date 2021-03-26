@echo off
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
) else (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
)
set compilerflags=-g --std=c++11 src/OrderBookEntry.cpp src/MerkelMain.cpp src/main.cpp src/CSVReader.cpp src/OrderBook.cpp src/Wallet.cpp src/TradingBot.cpp src/Bot.cpp
set linkerflags=/OUT:bin\main.exe
g++ %compilerflags% src\*.cpp  %linkerflags%
del bin\*.ilk *.obj *.pdb