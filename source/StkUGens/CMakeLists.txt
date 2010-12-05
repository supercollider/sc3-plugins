file(GLOB STKSources stk-4.2.2/*cpp)
include_directories(stk-4.4.2/)
include_directories(stk-4.4.2/include)
add_library(StkUGens MODULE StkAll.cpp ${STKSources})
