$(BUILD)
    g++
    -g
    -lstdc++
    -std=c++17
    -o
    sample
    sandbox\sample.cpp

$(RUN)
    sample.exe