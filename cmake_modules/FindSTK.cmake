# Find STK

FIND_PATH(
    STK_INCLUDE_DIR
    NAMES Stk.h
    HINTS $ENV{STK_DIR}/include
    PATHS /usr/local/include/stk
          /usr/local/include
          /usr/include/stk
          /usr/include
)

FIND_LIBRARY(
    STK_LIBRARY
    NAMES stk libstk
    HINTS $ENV{STK_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
)

SET(STK_FOUND "NO")

IF( STK_INCLUDE_DIR AND STK_LIBRARY )
    SET(STK_FOUND "YES")
ENDIF()
