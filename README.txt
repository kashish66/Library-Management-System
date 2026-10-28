LIBRARY MANAGEMENT SYSTEM — VS CODE PROJECT

This project runs locally from VS Code and opens its interface in Chrome.
It uses C++ for the backend and HTML/CSS/JavaScript for the interface.

FILES
- main.cpp
- httplib.h       (dependency to download)
- json.hpp        (dependency to download)
- books.json
- public/index.html

STEP 1 — DOWNLOAD THE TWO HEADER FILES
Download these files and save them in the same folder as main.cpp:
https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
Use Save As and keep the exact names httplib.h and json.hpp.

STEP 2 — INSTALL A C++ COMPILER (only once)
The VS Code C/C++ extension is not a compiler. If `g++ --version` fails,
install MSYS2 from https://www.msys2.org/
Open MSYS2 UCRT64 and run:
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
Add this Windows PATH entry:
C:\msys64\ucrt64\bin
Close and reopen VS Code.

STEP 3 — OPEN THIS FOLDER IN VS CODE
Use File > Open Folder and select the extracted LibraryManagementVSCode folder.

STEP 4 — COMPILE
Open Terminal > New Terminal and run:
g++ -std=c++17 main.cpp -o library_server.exe -lws2_32

STEP 5 — RUN
Run:
.\library_server.exe
Leave this terminal open. Open Chrome and go to:
http://localhost:8080

Book data is stored in books.json in this folder.
To stop the server, click the terminal and press Ctrl+C.

If compilation says a header file is missing, check that httplib.h and json.hpp
are beside main.cpp and have the correct filenames.
This project is for local educational use. It does not include authentication;
do not expose it to the public internet.
