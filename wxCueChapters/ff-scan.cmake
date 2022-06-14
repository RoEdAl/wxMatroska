#
# ff-scan.cmake
#
# ReplayGain and DR14 Matroska container scanner.
# Scanning whole audio track and chapters.
#
# Required tools: ffmpeg, ffprobe, cue2mkc.
#
# This is part of cue2mkc project.
#
# Execute this script by typing:
# cmake -D "CUE2MKC=<...>" -D "CUE2MKC_MKA=<..>" <-D "CUE2MKC_WORKDIR=<..>"> <-D "CUE2MKC_DST=<..>"> -P ff-scan.cmake
#
# Appended/replaced tags:
#
# - REPLAYGAIN_GAIN [chapter, track]
# - REPLAYGAIN_PEAK [chapter, track]
# - DR14 [chapter, track]
# - DR14_ALBUM [track]
#
# Input variables:
#
# - CUE2MKC_MKA               - path (full or relative) to MKA file, required
# - CUE2MKC_CHAPTERS          - JSON file witch chapters, if not specified then try read chapters from CUE2MKC_MKA
# - CUE2MKC_WORKDIR           - working directory, required if CUE2MKC_MKA is relative
# - CUE2MKC_DST               - path to output file (MKA or JSON), if not defined CUE2MKC_DST=CUE2MKC_MKA
# - CUE2MKC_MESSAGE_LOG_LEVEL - value of CMAKE_MESSAGE_LOG_LEVEL passed to subscripts (default WARNING)
#
# - CUE2MKC - (full) path to cue2mkc executable, required
# - FFMPEG -  (full) path to ffmpeg executable (optional, trying to guess via FIND_PROGRAM)
# - FFPROBE - (full) path to ffprobe executable (part of ffmpeg - optional, trying to guess via FIND_PROGRAM)
#
# Ffmpeg filters: [http://ffmpeg.org/ffmpeg-filters.html]
#
# - replaygain
# - drmeter
# - ebur128
#
# See also:
#
# - loudgain - http://github.com/Moonbase59/loudgain
# - r128gain - http://github.com/Moonbase59/loudgain
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

# check CUE2MKC_MKA
IF(NOT DEFINED CUE2MKC_MKA)
    MESSAGE(FATAL_ERROR "Required variable CUE2MKC_MKA is not defined")
ENDIF()

CMAKE_PATH(SET CUE2MKC_MKA_NORMALIZED ${CUE2MKC_MKA})
CMAKE_PATH(IS_RELATIVE CUE2MKC_MKA_NORMALIZED CUE2MKC_MKA_IS_RELATIVE)

# check CUE2MKC_WORKDIR
IF(CUE2MKC_MKA_IS_RELATIVE)
    IF(NOT DEFINED CUE2MKC_WORKDIR)
        MESSAGE(FATAL_ERROR "Required variable CUE2MKC_WORKDIR is not defined")
    ENDIF()
    CMAKE_PATH(SET CUE2MKC_WORKDIR_NORMALIZED ${CUE2MKC_WORKDIR})
ELSE()
    IF(DEFINED CUE2MKC_WORKDIR)
        CMAKE_PATH(SET CUE2MKC_WORKDIR_NORMALIZED ${CUE2MKC_WORKDIR})
    ELSEIF(DEFINED ENV{TMP})
        CMAKE_PATH(SET CUE2MKC_WORKDIR_NORMALIZED $ENV{TMP})
    ELSE()
        MESSAGE(FATAL_ERROR "Environment variable TMP is not defined")
    ENDIF()
ENDIF()

IF(NOT IS_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED})
    MESSAGE(FATAL_ERROR "Nonexistent directory - ${CUE2MKC_WORKDIR_NORMALIZED}")
ENDIF()

IF(DEFINED CUE2MKC_DST)
    CMAKE_PATH(SET CUE2MKC_DST_NORMALIZED ${CUE2MKC_DST})
ELSE()
    CMAKE_PATH(SET CUE2MKC_DST_NORMALIZED ${CUE2MKC_MKA_NORMALIZED})
ENDIF()
CMAKE_PATH(IS_RELATIVE CUE2MKC_DST_NORMALIZED CUE2MKC_DST_IS_RELATIVE)

IF(CUE2MKC_MKA_IS_RELATIVE)
    MESSAGE(STATUS "File: ${CUE2MKC_MKA_NORMALIZED}")
ELSE()
    CMAKE_PATH(GET CUE2MKC_MKA_NORMALIZED FILENAME CUE2MKC_MKA_FILENAME)
    MESSAGE(STATUS "File: ${CUE2MKC_MKA_FILENAME}")
ENDIF()

CMAKE_PATH(GET CUE2MKC_DST_NORMALIZED EXTENSION CUE2MKC_DST_EXT)
STRING(TOLOWER ${CUE2MKC_DST_EXT} CUE2MKC_DST_EXT)
IF(CUE2MKC_DST_EXT STREQUAL  ".json")
    SET(CUE2MKC_DST_IS_JSON TRUE)
ELSE()
    UNSET(CUE2MKC_DST_IS_JSON)
ENDIF()

IF(DEFINED CUE2MKC_CHAPTERS)
    CMAKE_PATH(SET CUE2MKC_CHAPTERS_NORMALIZED ${CUE2MKC_CHAPTERS})
    CMAKE_PATH(IS_RELATIVE CUE2MKC_CHAPTERS_NORMALIZED CUE2MKC_CHAPTERS_IS_RELATIVE)
ENDIF()

IF(DEFINED FFSTEM)
    SET(TMP_STEM ${FFSTEM})
ELSE()
    STRING(RANDOM LENGTH 10 TMP_SEQ)
    SET(TMP_STEM "ffscan-${TMP_SEQ}")
ENDIF()

# cue2mkc executable
IF(DEFINED CUE2MKC)
    CMAKE_PATH(SET CUE2MKC_NORMALIZED ${CUE2MKC})
ELSE()
    MESSAGE(FATAL_ERROR "Required variable CUE2MKC is not defined")
ENDIF()

# Find ffmpeg executable
IF (NOT DEFINED FFMPEG)
    FIND_PROGRAM(CMAKE_FFMPEG ffmpeg
        HINTS
            ENV LocalAppData
            ENV ProgramW6432
            ENV ProgramFiles
            ENV "ProgramFiles(x86)"
            "$ENV{SystemDrive}/Program Files"
            "$ENV{SystemDrive}/Program Files (x86)"
        PATH_SUFFIXES ffmpeg/bin
        NO_CACHE
        REQUIRED
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    CMAKE_PATH(NATIVE_PATH CMAKE_FFMPEG FFMPEG)
ELSEIF(NOT EXISTS ${FFMPEG})
    MESSAGE(FATAL_ERROR "ffmpeg not found - ${FFMPEG}")
ENDIF()

# Find ffprobe executable
IF (NOT DEFINED FFPROBE)
    FIND_PROGRAM(CMAKE_FFPROBE ffprobe
        HINTS
            ENV LocalAppData
            ENV ProgramW6432
            ENV ProgramFiles
            ENV "ProgramFiles(x86)"
            "$ENV{SystemDrive}/Program Files"
            "$ENV{SystemDrive}/Program Files (x86)"
        PATH_SUFFIXES ffmpeg/bin
        NO_CACHE
        REQUIRED
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    CMAKE_PATH(NATIVE_PATH CMAKE_FFPROBE FFPROBE)
ELSEIF(NOT EXISTS ${FFPROBE})
    MESSAGE(FATAL_ERROR "ffprobe not found - ${FFPROBE}")
ENDIF()

# ---------------------------------------------------------------

STRING(TIMESTAMP FFSCANTS UTC)

IF(NOT CUE2MKC_DST_IS_JSON)

    CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${TMP_STEM} OUTPUT_VARIABLE TMP_ATTACHMENTS_DIR)
    FILE(MAKE_DIRECTORY ${TMP_ATTACHMENTS_DIR})

    SET(ATTACHMENT_LIST "")
    SET(TAG_LIST "")

    MESSAGE(STATUS "Getting image attachments")
    EXECUTE_PROCESS(
        # ffprobe
        COMMAND ${FFPROBE}
            -hide_banner
            -loglevel repeat+level+fatal
            -show_entries "stream=index : stream_tags"
            -select_streams v:m:filename
            -of json=compact=1
            ${CUE2MKC_MKA_NORMALIZED}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE FFPROBE_OUT
        ERROR_VARIABLE FFPROBE_ERR
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )

    STRING(JSON FFPROBE_STREAMS_LEN LENGTH ${FFPROBE_OUT} streams)
    MATH(EXPR FFPROBE_STREAMS_LEN1 "${FFPROBE_STREAMS_LEN}-1")

    STRING(JSON FFPROBE_STREAMS GET ${FFPROBE_OUT} streams)

    FOREACH(S RANGE ${FFPROBE_STREAMS_LEN1})
        STRING(JSON FFPROBE_STREAM GET ${FFPROBE_STREAMS} ${S})
        STRING(JSON FFPROBE_STREAM_IDX GET ${FFPROBE_STREAM} index)
    
        STRING(JSON FFPROBE_TAGS GET ${FFPROBE_STREAM} tags)
        STRING(JSON FFPROBE_TAGS_LEN LENGTH ${FFPROBE_TAGS})
        MESSAGE(STATUS "[${S}] attachment #${FFPROBE_STREAM_IDX} (${FFPROBE_STREAMS_LEN})")
        MATH(EXPR FFPROBE_TAGS_LEN "${FFPROBE_TAGS_LEN}-1")

        UNSET(FFPROBE_TAG_FILENAME)	
        FOREACH(T RANGE ${FFPROBE_TAGS_LEN})
            STRING(JSON FFPROBE_TAG_NAME MEMBER ${FFPROBE_TAGS} ${T})
            STRING(JSON FFPROBE_TAG_VALUE GET ${FFPROBE_TAGS} ${FFPROBE_TAG_NAME})
            IF(${FFPROBE_TAG_NAME} STREQUAL filename)
                SET(FFPROBE_TAG_FILENAME ${FFPROBE_TAG_VALUE})
                LIST(APPEND ATTACHMENT_LIST ${FFPROBE_TAG_FILENAME})
                MESSAGE(STATUS "[${S}] filename: ${FFPROBE_TAG_FILENAME}")
            ELSE()
                IF(FFPROBE_TAG_VALUE MATCHES " +")
                    LIST(APPEND TAG_LIST "${FFPROBE_STREAM_IDX} \"${FFPROBE_TAG_NAME}=${FFPROBE_TAG_VALUE}\"")
                ELSE()
                    LIST(APPEND TAG_LIST "${FFPROBE_STREAM_IDX} ${FFPROBE_TAG_NAME}=${FFPROBE_TAG_VALUE}")
                ENDIF()
                MESSAGE(STATUS "[${S}] ${FFPROBE_TAG_NAME}: ${FFPROBE_TAG_VALUE}")
            ENDIF()
        ENDFOREACH()
    
        IF(NOT DEFINED FFPROBE_TAG_FILENAME)
            MESSAGE(STATUS "Could not find filename of attachment #${S}")
            CONTINUE()
        ENDIF()
    
        MESSAGE(STATUS "[${S}] extracting")
        EXECUTE_PROCESS(
            COMMAND ${FFMPEG}
                -y -hide_banner -nostdin -nostats
                -loglevel repeat+level+warning

                -bitexact
                -i ${CUE2MKC_MKA_NORMALIZED}

                -map 0:${FFPROBE_STREAM_IDX}
                -map_metadata -1
                -map_chapters -1
            
                -c copy
            
                -bitexact
                "${TMP_STEM}/${FFPROBE_TAG_FILENAME}"
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
        )
    ENDFOREACH()

    # ---------------------------------------------------------------

    MESSAGE(STATUS "Getting other attachments")
    EXECUTE_PROCESS(
        # ffprobe
        COMMAND ${FFPROBE}
            # global options
            -hide_banner
            -loglevel repeat+level+fatal
            -show_entries "stream=index : stream_tags"
            -select_streams t
            -of json=compact=1
            ${CUE2MKC_MKA_NORMALIZED}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE FFPROBE_OUT
        ERROR_VARIABLE FFPROBE_ERR
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )

    STRING(JSON FFPROBE_STREAMS_LEN LENGTH ${FFPROBE_OUT} streams)
    MATH(EXPR FFPROBE_STREAMS_LEN1 "${FFPROBE_STREAMS_LEN}-1")

    STRING(JSON FFPROBE_STREAMS GET ${FFPROBE_OUT} streams)

    FOREACH(S RANGE ${FFPROBE_STREAMS_LEN1})
        STRING(JSON FFPROBE_STREAM GET ${FFPROBE_STREAMS} ${S})
        STRING(JSON FFPROBE_STREAM_IDX GET ${FFPROBE_STREAM} index)
    
        STRING(JSON FFPROBE_TAGS GET ${FFPROBE_STREAM} tags)
        STRING(JSON FFPROBE_TAGS_LEN LENGTH ${FFPROBE_TAGS})
        MESSAGE(STATUS "[${S}] attachment #${FFPROBE_STREAM_IDX} (${FFPROBE_STREAMS_LEN})")
        MATH(EXPR FFPROBE_TAGS_LEN "${FFPROBE_TAGS_LEN}-1")

        UNSET(FFPROBE_TAG_FILENAME)	
        FOREACH(T RANGE ${FFPROBE_TAGS_LEN})
            STRING(JSON FFPROBE_TAG_NAME MEMBER ${FFPROBE_TAGS} ${T})
            STRING(JSON FFPROBE_TAG_VALUE GET ${FFPROBE_TAGS} ${FFPROBE_TAG_NAME})
            IF(${FFPROBE_TAG_NAME} STREQUAL filename)
                SET(FFPROBE_TAG_FILENAME ${FFPROBE_TAG_VALUE})
                LIST(APPEND ATTACHMENT_LIST ${FFPROBE_TAG_FILENAME})
                MESSAGE(STATUS "[${S}] filename: ${FFPROBE_TAG_FILENAME}")
            ELSE()
                IF(FFPROBE_TAG_VALUE MATCHES " +")
                    LIST(APPEND TAG_LIST "${FFPROBE_STREAM_IDX} \"${FFPROBE_TAG_NAME}=${FFPROBE_TAG_VALUE}\"")
                ELSE()
                    LIST(APPEND TAG_LIST "${FFPROBE_STREAM_IDX} ${FFPROBE_TAG_NAME}=${FFPROBE_TAG_VALUE}")
                ENDIF()
                MESSAGE(STATUS "[${S}] ${FFPROBE_TAG_NAME}: ${FFPROBE_TAG_VALUE}")
            ENDIF()
        ENDFOREACH()
    
        IF(NOT DEFINED FFPROBE_TAG_FILENAME)
            MESSAGE(STATUS "Could not find filename of attachment #${S}")
            CONTINUE()
        ENDIF()
    
        MESSAGE(STATUS "[${S}] extracting")
        EXECUTE_PROCESS(
            COMMAND ${FFMPEG}
                -y -hide_banner -nostdin -nostats
                -loglevel repeat+level+warning
                -threads 1
            
                -dump_attachment:${FFPROBE_STREAM_IDX} "${TMP_STEM}/${FFPROBE_TAG_FILENAME}"

                -bitexact
                -i ${CUE2MKC_MKA_NORMALIZED}

                -map 0:a:0
                -map_chapters -1
                -map_metadata -1
            
                -c copy
                -bitexact
                -f null NUL
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
        )
    ENDFOREACH()

    LIST(TRANSFORM TAG_LIST PREPEND "        -metadata:s:")
    LIST(JOIN TAG_LIST "\n" TAG_STR)

    LIST(TRANSFORM ATTACHMENT_LIST PREPEND "        \"" REGEX " +")
    LIST(TRANSFORM ATTACHMENT_LIST APPEND "\"" REGEX " +")
    LIST(TRANSFORM ATTACHMENT_LIST PREPEND "        -attach ${TMP_STEM}/")
    LIST(JOIN ATTACHMENT_LIST "\n" ATTACHMENT_STR)
ENDIF()

# ---------------------------------------------------------------

SET(FFMPEG_REGEXP_TRACK_GAIN "\\[info\\] track_gain = ([+-][0-9]+\.[0-9]+) +dB")
SET(FFMPEG_REGEXP_TRACK_PEAK "\\[info\\] track_peak = ([0-9]+\.[0-9]+)")
SET(FFMPEG_REGEXP_OVERALL_DR "\\[info\\] Overall DR: ([0-9]+\.[0-9]+)")
SET(FFMPEG_REGEXP_EBUR128_SUMMARY "\\[info\\] Summary:")
SET(FFMPEG_REGEXP_EBUR128_VALUE "([A-Za-z]+( [A-Za-z]+)?): +([+-]?[0-9]+\.[0-9]+) +LU(FS)")
SET(EBUR128_EMPTY_OBJ "{\"integrated\": {},\"range\": {}}")

FUNCTION(calc_rg2_loudness val)
    EXECUTE_PROCESS(
        COMMAND ${CUE2MKC_NORMALIZED} --calc-rg2-loudness ${val}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE CUE2MKC_OUT
        ERROR_VARIABLE CUE2MKC_ERR
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )
    SET(RG2_LOUDNESS ${CUE2MKC_OUT} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(process_ffmpeg_output ffmpeg_out)
    STRING(REGEX MATCHALL "[^\r\n]+" FFMPEG_LINES ${ffmpeg_out})
    SET(RESULT "{}")
    SET(EBUR128_OBJ ${EBUR128_EMPTY_OBJ})

    UNSET(EBUR128_KEY)
    UNSET(EBUR128_POS)
    UNSET(EBUR128_SUMMARY)

    FOREACH(L IN LISTS FFMPEG_LINES)
        IF(DEFINED EBUR128_SUMMARY)
            IF(${L} MATCHES "\\[info\\]")
                UNSET(EBUR128_KEY)
                UNSET(EBUR128_POS)
                UNSET(EBUR128_SUMMARY) # end of summary
            ELSE()
                # EBUR128 analysis
                STRING(FIND ${L} "Integrated loudness:" EBUR128_POS)
                IF(EBUR128_POS GREATER 0)
                    SET(EBU128_KEY integrated)
                    CONTINUE()
                ENDIF()
                STRING(FIND ${L} "Loudness range:" EBUR128_POS)
                IF(EBUR128_POS GREATER 0)
                    SET(EBU128_KEY range)
                    CONTINUE()
                ENDIF()
                IF(${L} MATCHES ${FFMPEG_REGEXP_EBUR128_VALUE})
                    IF(DEFINED EBU128_KEY)
                        STRING(JSON EBUR128_OBJ SET ${EBUR128_OBJ} ${EBU128_KEY} ${CMAKE_MATCH_1} "\"${CMAKE_MATCH_3}\"")
                    ELSE()
                        MESSAGE(STATUS "[ebur128] No key: ${CMAKE_MATCH_1}: ${CMAKE_MATCH_3}")
                    ENDIF()
                    CONTINUE()
                ENDIF()
                CONTINUE()
            ENDIF()
        ENDIF()

        IF(NOT ${L} MATCHES "\\[info\\]")
            CONTINUE()
        ENDIF()
        IF(${L} MATCHES ${FFMPEG_REGEXP_EBUR128_SUMMARY})
            SET(EBUR128_SUMMARY ON) # begining of summary
            CONTINUE()
        ENDIF()

        # RG
        IF(${L} MATCHES ${FFMPEG_REGEXP_TRACK_GAIN})
            IF(${CMAKE_MATCH_1} LESS_EQUAL -24)
                CONTINUE()
            ENDIF()
            STRING(JSON RESULT SET ${RESULT} "REPLAYGAIN_GAIN" "\"${CMAKE_MATCH_1} dB\"")
            CONTINUE()
        ENDIF()
        IF(${L} MATCHES ${FFMPEG_REGEXP_TRACK_PEAK})
            IF(${CMAKE_MATCH_1} LESS_EQUAL 0)
                CONTINUE()
            ENDIF()
            STRING(JSON RESULT SET ${RESULT} "REPLAYGAIN_PEAK" "\"${CMAKE_MATCH_1}\"")
            CONTINUE()
        ENDIF()

        # DR14
        IF(${L} MATCHES ${FFMPEG_REGEXP_OVERALL_DR})
            STRING(JSON RESULT SET ${RESULT} "DR14" "\"${CMAKE_MATCH_1}\"")
            CONTINUE()
        ENDIF()
    ENDFOREACH()
    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${EBUR128_OBJ} integrated "I")
    IF(NOT JSON_VALUE_ERR)
        calc_rg2_loudness(${JSON_VALUE})
        STRING(JSON RESULT SET ${RESULT} "REPLAYGAIN_GAIN" "\"${RG2_LOUDNESS} dB\"")
    ENDIF()
    SET(FFMPEG_SCAN_RESULT ${RESULT} PARENT_SCOPE)
ENDFUNCTION()

IF(CUE2MKC_DST_IS_JSON)
    SET(CUE2MKC_OBJ "{\"chapters\": []}")
    SET(CUE2MKC_CUR_CHAPTER 0)
ELSE()
    SET(FFMAP "")
ENDIF()

MESSAGE(STATUS "Scanning chapters")
IF(DEFINED CUE2MKC_CHAPTERS)
    MESSAGE(TRACE "Reading chapters file: ${CUE2MKC_CHAPTERS_NORMALIZED}")
    IF(CUE2MKC_CHAPTERS_IS_RELATIVE)
        CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${CUE2MKC_CHAPTERS_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_CHAPTERS_NORMALIZED_PATH)
        FILE(READ ${CUE2MKC_CHAPTERS_NORMALIZED_PATH} FFPROBE_OUT)
    ELSE()
        FILE(READ ${CUE2MKC_CHAPTERS_NORMALIZED} FFPROBE_OUT)
    ENDIF()
ELSE()
    MESSAGE(TRACE "Getting chapters from MKA - ffprobe")
    EXECUTE_PROCESS(
        COMMAND ${FFPROBE}
            -hide_banner
            -loglevel repeat+level+fatal
            -show_chapters
            -of json=compact=1
            ${CUE2MKC_MKA_NORMALIZED}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE FFPROBE_OUT
        ERROR_VARIABLE FFPROBE_ERR
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )
ENDIF()

STRING(JSON FFPROBE_CHAPTERS GET ${FFPROBE_OUT} chapters)
STRING(JSON FFPROBE_CHAPTERS_LEN LENGTH ${FFPROBE_OUT} chapters)
MATH(EXPR FFPROBE_CHAPTERS_LEN1 "${FFPROBE_CHAPTERS_LEN}-1")

FOREACH(C RANGE ${FFPROBE_CHAPTERS_LEN1})
    STRING(JSON FFPROBE_CHAPTER GET ${FFPROBE_CHAPTERS} ${C})
    
    STRING(JSON FFPROBE_CHAPTER_ID ERROR_VARIABLE FFPROBE_CHAPTER_ID_ERR GET ${FFPROBE_CHAPTER} id)
    IF(FFPROBE_CHAPTER_ID_ERR) # id isn't mandatory
        MESSAGE(TRACE "[${C}] No chapter ID")
        SET(FFPROBE_CHAPTER_ID "??")
    ENDIF()
    STRING(JSON FFPROBE_CHAPTER_START_TIME GET ${FFPROBE_CHAPTER} start_time)
    STRING(JSON FFPROBE_CHAPTER_END_TIME GET ${FFPROBE_CHAPTER} end_time)
    
    MESSAGE(STATUS "[${C}] chapter ${FFPROBE_CHAPTER_ID}/${FFPROBE_CHAPTERS_LEN}: [${FFPROBE_CHAPTER_START_TIME}—${FFPROBE_CHAPTER_END_TIME}]")
    
    EXECUTE_PROCESS(
        COMMAND ${FFMPEG}
            -hide_banner -nostdin -nostats
            -loglevel repeat+level+info
            -threads 1
            
            -ss ${FFPROBE_CHAPTER_START_TIME}
            -to ${FFPROBE_CHAPTER_END_TIME}

            -bitexact
            -i ${CUE2MKC_MKA_NORMALIZED}

            -filter_complex_threads 1
            -filter_complex "[0:a:0]replaygain,drmeter=length=1,ebur128[outa]"

            -map [outa]
            -map_metadata -1
            -map_chapters -1
            
            -f null
            -bitexact
            NUL
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE FFMPEG_OUT
        ERROR_VARIABLE FFMPEG_ERR
#		ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )
    
    process_ffmpeg_output(${FFMPEG_ERR})
    IF(CUE2MKC_DST_IS_JSON)
        STRING(JSON CUE2MKC_OBJ SET ${CUE2MKC_OBJ} chapters ${CUE2MKC_CUR_CHAPTER} ${FFMPEG_SCAN_RESULT})
        MATH(EXPR CUE2MKC_CUR_CHAPTER "${CUE2MKC_CUR_CHAPTER}+1")

        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_GAIN")
        IF(NOT JSON_VALUE_ERR)
            MESSAGE(STATUS "[${C}] RG Gain: ${JSON_VALUE}")
        ENDIF()

        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_PEAK")
        IF(NOT JSON_VALUE_ERR)
            MESSAGE(STATUS "[${C}] RG Peak: ${JSON_VALUE}")
        ENDIF()

        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "DR14")
        IF(NOT JSON_VALUE_ERR)
            MESSAGE(STATUS "[${C}] DR14: ${JSON_VALUE}")
        ENDIF()
    ELSE()
        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_GAIN")
        IF(NOT JSON_VALUE_ERR)
            LIST(APPEND FFMAP ":c:${C} \"REPLAYGAIN_GAIN=${JSON_VALUE} dB\"")
            MESSAGE(STATUS "[${C}] RG Gain: ${JSON_VALUE}")
        ENDIF()

        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_PEAK")
        IF(NOT JSON_VALUE_ERR)
            LIST(APPEND FFMAP ":c:${C} REPLAYGAIN_PEAK=${JSON_VALUE}")
            MESSAGE(STATUS "[${C}] RG Peak: ${JSON_VALUE}")
        ENDIF()

        STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "DR14")
        IF(NOT JSON_VALUE_ERR)
            LIST(APPEND FFMAP ":c:${C} DR14=${JSON_VALUE}")
            MESSAGE(STATUS "[${C}] DR14: ${JSON_VALUE}")
        ENDIF()
    ENDIF()
ENDFOREACH()

# ---------------------------------------------------------------

MESSAGE(STATUS "Scanning whole track")
EXECUTE_PROCESS(
    COMMAND ${FFMPEG}
        -hide_banner -nostdin -nostats
        -loglevel repeat+level+info
        -threads 1

        -bitexact
        -i ${CUE2MKC_MKA_NORMALIZED}

        -filter_complex_threads 1
        -filter_complex "[0:a:0]replaygain,drmeter=length=1,ebur128[outa]"

        -map [outa]
        -map_chapters -1
        -map_metadata -1
        
        -f null
        -bitexact
        NUL
    ENCODING UTF-8
    COMMAND_ECHO NONE
    OUTPUT_VARIABLE FFMPEG_OUT
    ERROR_VARIABLE FFMPEG_ERR
#	ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
    WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
)

process_ffmpeg_output(${FFMPEG_ERR})
IF(CUE2MKC_DST_IS_JSON)
    STRING(JSON CUE2MKC_OBJ SET ${CUE2MKC_OBJ} album ${FFMPEG_SCAN_RESULT})

    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_GAIN")
    IF(NOT JSON_VALUE_ERR)
        MESSAGE(STATUS "[A] RG Gain: ${JSON_VALUE}")
    ENDIF()

    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_PEAK")
    IF(NOT JSON_VALUE_ERR)
        MESSAGE(STATUS "[A] RG Peak: ${JSON_VALUE}")
    ENDIF()

    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "DR14")
    IF(NOT JSON_VALUE_ERR)
            MESSAGE(STATUS "[A] DR14: ${JSON_VALUE}")
    ENDIF()
ELSE()
    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_GAIN")
    IF(NOT JSON_VALUE_ERR)
        LIST(APPEND FFMAP " \"REPLAYGAIN_GAIN=${JSON_VALUE} dB\"")
        MESSAGE(STATUS "[A] RG Gain: ${JSON_VALUE}")
    ENDIF()

    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "REPLAYGAIN_PEAK")
    IF(NOT JSON_VALUE_ERR)
        LIST(APPEND FFMAP " REPLAYGAIN_PEAK=${JSON_VALUE}")
        MESSAGE(STATUS "[A] RG Peak: ${JSON_VALUE}")
    ENDIF()

    STRING(JSON JSON_VALUE ERROR_VARIABLE JSON_VALUE_ERR GET ${FFMPEG_SCAN_RESULT} "DR14")
    IF(NOT JSON_VALUE_ERR)
            LIST(APPEND FFMAP " DR14=${JSON_VALUE}")
            LIST(APPEND FFMAP " DR14_ALBUM=${JSON_VALUE}")
            MESSAGE(STATUS "[A] DR14: ${JSON_VALUE}")
    ENDIF()
ENDIF()

# ---------------------------------------------------------------

IF(CUE2MKC_DST_IS_JSON)
    MESSAGE(STATUS "Save JSON file")
    IF(CUE2MKC_DST_IS_RELATIVE)
        CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${CUE2MKC_DST_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_DST_NORMALIZED_PATH)
        FILE(WRITE ${CUE2MKC_DST_NORMALIZED_PATH} ${CUE2MKC_OBJ})
    ELSE()
        FILE(WRITE ${CUE2MKC_DST_NORMALIZED} ${CUE2MKC_OBJ})
    ENDIF()
ELSE()
    MESSAGE(STATUS "Creating temporary script")

    CMAKE_PATH(SET TMP_CMAKE_SCRIPT "${TMP_STEM}.cmake")
    CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${TMP_CMAKE_SCRIPT} OUTPUT_VARIABLE TMP_CMAKE_SCRIPT_PATH)

    CMAKE_PATH(SET TMP_MKA "${TMP_STEM}.mka")
    CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${TMP_MKA} OUTPUT_VARIABLE TMP_MKA_PATH)

    LIST(TRANSFORM FFMAP PREPEND "        -metadata")
    LIST(JOIN FFMAP "\n" FFMAP_STR)

    SET(SCRIPT_CNT "")
    STRING(APPEND SCRIPT_CNT [=[

    CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

    MESSAGE(STATUS "Recreating MKA container")

    EXECUTE_PROCESS(
        COMMAND ${FFMPEG}
            # global options
            -y -hide_banner -nostdin -nostats
            -loglevel repeat+level+warning
            -threads 1

            -bitexact
            -i ${CUE2MKC_MKA}

    ]=])
    STRING(APPEND SCRIPT_CNT ${ATTACHMENT_STR})
    STRING(PREPEND FFMAP_STR [=[
            -map 0:a
            -map_chapters 0
            -map_metadata 0

            -metadata "FFSCANTS=${FFSCANTS}"

    ]=])
    STRING(APPEND SCRIPT_CNT ${FFMAP_STR})
    STRING(APPEND SCRIPT_CNT ${TAG_STR})
    STRING(APPEND SCRIPT_CNT [=[
        
            -c copy
        
            -bitexact
            -cluster_time_limit 1000
            -default_mode infer_no_subs

            ${TMP_MKA}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        COMMAND_ERROR_IS_FATAL ANY
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR}
    )
    ]=])

    FILE(WRITE ${TMP_CMAKE_SCRIPT_PATH} ${SCRIPT_CNT})
#    FILE(CONFIGURE OUTPUT ${TMP_CMAKE_SCRIPT_PATH}
#        CONTENT ${SCRIPT_CNT}
#        @ONLY
#    )

    IF(NOT DEFINED CUE2MKC_MESSAGE_LOG_LEVEL)
        SET(CUE2MKC_MESSAGE_LOG_LEVEL WARNING)
    ENDIF()

    MESSAGE(STATUS "Executing temporary script")
    EXECUTE_PROCESS(
        COMMAND 
            ${CMAKE_COMMAND}
                --log-context
                -D "CMAKE_MESSAGE_CONTEXT=${CMAKE_MESSAGE_CONTEXT}-tmp"
                --log-level=${CUE2MKC_MESSAGE_LOG_LEVEL}
                -D "FFMPEG=${FFMPEG}"
                -D "CUE2MKC_MKA=${CUE2MKC_MKA_NORMALIZED}"
                -D "TMP_MKA=${TMP_MKA}"
                -D "CUE2MKC_WORKDIR=${CUE2MKC_WORKDIR_NORMALIZED}"
                -D "FFSCANTS=${FFSCANTS}"
                -P ${TMP_CMAKE_SCRIPT_PATH}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        COMMAND_ERROR_IS_FATAL ANY
        WORKING_DIRECTORY ${CUE2MKC_WORKDIR_NORMALIZED}
    )

    MESSAGE(STATUS "Removing temporary files/dirs")
    FILE(REMOVE ${TMP_CMAKE_SCRIPT_PATH})
    FILE(REMOVE_RECURSE ${TMP_ATTACHMENTS_DIR})

    MESSAGE(STATUS "Moving/replacing recreated MKA file")
    IF(CUE2MKC_DST_IS_RELATIVE)
        CMAKE_PATH(APPEND CUE2MKC_WORKDIR_NORMALIZED ${CUE2MKC_DST_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_DST_NORMALIZED_PATH)
        FILE(RENAME ${TMP_MKA_PATH} ${CUE2MKC_DST_NORMALIZED_PATH})
    ELSE()
        FILE(RENAME ${TMP_MKA_PATH} ${CUE2MKC_DST_NORMALIZED})
    ENDIF()
ENDIF()

MESSAGE(STATUS "We're done")
