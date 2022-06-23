#
# dr-scan.cmake
#
# ReplayGain and DR14 Matroska container scanner.
# Scanning whole audio track and chapters.
#
# Required tools: ffmpeg, ffprobe, cue2mkc.
#
# This is part of cue2mkc project.
#
# Execute this script by typing:
# cmake -D "CUE2MKC=<..>" -D "FFMPEG=<..>" -D "CUE2MKC_MKA=<..>" [-D "CUE2MKC_DST=<..>"] -P dr-scan.cmake
#
# Produced tags:
#
# - REPLAYGAIN_GAIN [chapter, album]
# - REPLAYGAIN_PEAK [chapter, album]
# - DR14 [chapter, album]
# - DR14_ALBUM [album]
# - DRSCaN_TIMESTAMP [album]
#
# Input variables:
#
# - CUE2MKC_MKA       - path (full or relative) to MKA file, required
# - CUE2MKC_CHAPTERS  - JSON file witch chapters, required, chapters from CUE2MKC_MKA are ignored
# - CUE2MKC_DST       - path to output JSON file with tags, if not defined CUE2MKC_DST=CUE2MKC_MKA(ext->json)
#
# - CUE2MKC - (full) path to cue2mkc executable, required, may be specified via environment variable
# - FFMPEG -  (full) path to ffmpeg executable, required, may be specified via environment variable
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

IF(DEFINED CUE2MKC_DST)
    CMAKE_PATH(SET CUE2MKC_DST_NORMALIZED ${CUE2MKC_DST})
ELSE()
    CMAKE_PATH(SET CUE2MKC_DST_NORMALIZED ${CUE2MKC_MKA_NORMALIZED})
    CMAKE_PATH(REPLACE_EXTENSION CUE2MKC_DST_NORMALIZED LAST_ONLY ".json")
ENDIF()
CMAKE_PATH(IS_RELATIVE CUE2MKC_DST_NORMALIZED CUE2MKC_DST_IS_RELATIVE)

IF(CUE2MKC_MKA_IS_RELATIVE)
    MESSAGE(STATUS "File: ${CUE2MKC_MKA_NORMALIZED}")
ELSE()
    CMAKE_PATH(GET CUE2MKC_MKA_NORMALIZED FILENAME CUE2MKC_MKA_FILENAME)
    MESSAGE(STATUS "File: ${CUE2MKC_MKA_FILENAME}")
ENDIF()

IF(DEFINED CUE2MKC_CHAPTERS)
    CMAKE_PATH(SET CUE2MKC_CHAPTERS_NORMALIZED ${CUE2MKC_CHAPTERS})
    CMAKE_PATH(IS_RELATIVE CUE2MKC_CHAPTERS_NORMALIZED CUE2MKC_CHAPTERS_IS_RELATIVE)
ELSE()
    MESSAGE(FATAL_ERROR "Required variable CUE2MKC_CHAPTERS is not defined")
ENDIF()

# cue2mkc executable
IF(DEFINED CUE2MKC)
    CMAKE_PATH(SET CUE2MKC_NORMALIZED ${CUE2MKC})
ELSEIF(DEFINED ENV{CUE2MKC})
    CMAKE_PATH(SET CUE2MKC_NORMALIZED $ENV{CUE2MKC})
ELSE()
    MESSAGE(FATAL_ERROR "Required variable CUE2MKC is not defined")
ENDIF()

IF(DEFINED FFMPEG)
    MESSAGE(DEBUG "FFMPEG set to ${FFMPEG}")
ELSEIF(DEFINED ENV{FFMPEG})
    CMAKE_PATH(SET FFMPEG $ENV{FFMPEG})
    MESSAGE(DEBUG "FFMPEG set to ${FFMPEG} via environment variable")
ELSE()
    MESSAGE(FATAL_ERROR "Required variable FFMPEG is not defined")
ENDIF()

# ---------------------------------------------------------------

STRING(TIMESTAMP DRSCANTS UTC)

SET(FFMPEG_FILTER "[0:a:0]replaygain,drmeter=length=1,ebur128[outa]")
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
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
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
                    MESSAGE(DEBUG "IL ${L}")
                    SET(EBU128_KEY integrated)
                    CONTINUE()
                ENDIF()
                STRING(FIND ${L} "Loudness range:" EBUR128_POS)
                IF(EBUR128_POS GREATER 0)
                    MESSAGE(DEBUG "LR ${L}")
                    SET(EBU128_KEY range)
                    CONTINUE()
                ENDIF()
                IF(${L} MATCHES ${FFMPEG_REGEXP_EBUR128_VALUE})
                    MESSAGE(DEBUG "EBUR128-VAL ${L}")
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
            MESSAGE(DEBUG "EBUR128 ${L}")
            SET(EBUR128_SUMMARY ON) # begining of summary
            CONTINUE()
        ENDIF()

        # RG
        IF(${L} MATCHES ${FFMPEG_REGEXP_TRACK_GAIN})
            MESSAGE(DEBUG "TG ${L}")
            IF(${CMAKE_MATCH_1} LESS_EQUAL -24)
                CONTINUE()
            ENDIF()
            STRING(JSON RESULT SET ${RESULT} "REPLAYGAIN_GAIN" "\"${CMAKE_MATCH_1} dB\"")
            CONTINUE()
        ENDIF()
        IF(${L} MATCHES ${FFMPEG_REGEXP_TRACK_PEAK})
            MESSAGE(DEBUG "TP ${L}")
            IF(${CMAKE_MATCH_1} LESS_EQUAL 0)
                CONTINUE()
            ENDIF()
            STRING(JSON RESULT SET ${RESULT} "REPLAYGAIN_PEAK" "\"${CMAKE_MATCH_1}\"")
            CONTINUE()
        ENDIF()

        # DR14
        IF(${L} MATCHES ${FFMPEG_REGEXP_OVERALL_DR})
            MESSAGE(DEBUG "DR14 ${L}")
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

SET(CUE2MKC_OBJ "{\"chapters\": []}")
SET(CUE2MKC_CUR_CHAPTER 0)

MESSAGE(STATUS "Scanning chapters")
MESSAGE(TRACE "Reading chapters file: ${CUE2MKC_CHAPTERS_NORMALIZED}")

IF(CUE2MKC_CHAPTERS_IS_RELATIVE)
    CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${CUE2MKC_CHAPTERS_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_CHAPTERS_NORMALIZED_PATH)
    FILE(READ ${CUE2MKC_CHAPTERS_NORMALIZED_PATH} FFPROBE_OUT)
ELSE()
    FILE(READ ${CUE2MKC_CHAPTERS_NORMALIZED} FFPROBE_OUT)
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
            -filter_complex ${FFMPEG_FILTER}

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
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    process_ffmpeg_output(${FFMPEG_ERR})
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
ENDFOREACH()

# ---------------------------------------------------------------

MESSAGE(STATUS "Scanning album")
EXECUTE_PROCESS(
    COMMAND ${FFMPEG}
        -hide_banner -nostdin -nostats
        -loglevel repeat+level+info
        -threads 1

        -bitexact
        -i ${CUE2MKC_MKA_NORMALIZED}

        -filter_complex_threads 1
        -filter_complex ${FFMPEG_FILTER}

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
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

process_ffmpeg_output(${FFMPEG_ERR})
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
        STRING(JSON FFMPEG_SCAN_RESULT SET ${FFMPEG_SCAN_RESULT} DR14_ALBUM "\"${JSON_VALUE}\"")
ENDIF()
STRING(JSON FFMPEG_SCAN_RESULT SET ${FFMPEG_SCAN_RESULT} DRSCAN_TIMESTAMP "\"${DRSCANTS}\"")
STRING(JSON CUE2MKC_OBJ SET ${CUE2MKC_OBJ} album ${FFMPEG_SCAN_RESULT})

# ---------------------------------------------------------------

MESSAGE(STATUS "Save JSON file")
IF(CUE2MKC_DST_IS_RELATIVE)
    CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${CUE2MKC_DST_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_DST_NORMALIZED_PATH)
    FILE(WRITE ${CUE2MKC_DST_NORMALIZED_PATH} ${CUE2MKC_OBJ})
ELSE()
    FILE(WRITE ${CUE2MKC_DST_NORMALIZED} ${CUE2MKC_OBJ})
ENDIF()

MESSAGE(STATUS "We're done")
