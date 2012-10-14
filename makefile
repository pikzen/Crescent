# Getting ANY DirectX code to compile with g++ or clang is a pain in the ass.
# Actually, I nver got it to work, as a lot of DX internal code depends on VC.exe #defines.
# One day, maybe.

OPTIONS = "-Wall"
INCLUDE = "-IC:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include -IC:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/include"