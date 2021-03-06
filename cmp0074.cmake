#
# CMP0074 NEW
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.23)

MESSAGE("SCRIPT: ${SCRIPT_FILE}")

FILE(STRINGS ${SCRIPT_FILE} SCRIPT_CNT)
LIST(INSERT SCRIPT_CNT 1 "cmake_policy(SET CMP0074 NEW)")
LIST(POP_FRONT SCRIPT_CNT)
LIST(PREPEND SCRIPT_CNT "CMAKE_MINIMUM_REQUIRED(VERSION 3.23)")
LIST(JOIN SCRIPT_CNT "\n" SCRIPT_SCR)
FILE(WRITE ${SCRIPT_FILE} ${SCRIPT_SCR})
