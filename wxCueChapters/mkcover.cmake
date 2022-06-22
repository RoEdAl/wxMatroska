#
# mkcover.cmake
#
# Required tools: magick (ImageMagick), mutool (MuPDF).
# This is part of cue2mkc project.
#
# Input variables:
#
# - CUE2MKC_SRC_IMG    - path to image or PDF file, required
# - CUE2MKC_DST_IMG    - path to converted file (JPEG or WEBP), if not specified CUE2MKC_DST_IMG=CUE2MKC_SRC_IMG(ext->jpg)
# - CUE2MKC_STEM       - prefix for temporary files (optional)
#
# - IMAGICK - (full) path to magick executable, part of ImageMagick (optional, trying to guess via FIND_PROGRAM)
# - MUTOOL  - (full) path to mutool executable, part of MuPDF (optional, trying to guess via FIND_PROGRAM)
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

IF (NOT DEFINED IMAGICK)
    FIND_PROGRAM(CMAKE_IMAGICK magick
        HINTS
            ENV LocalAppData
            ENV ProgramW6432
            ENV ProgramFiles
            ENV "ProgramFiles(x86)"
            "$ENV{SystemDrive}/Program Files"
            "$ENV{SystemDrive}/Program Files (x86)"
        PATH_SUFFIXES ImageMagick
        NO_CACHE
        REQUIRED
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    CMAKE_PATH(NATIVE_PATH CMAKE_IMAGICK IMAGICK)
ELSEIF(NOT EXISTS ${IMAGICK})
    MESSAGE(FATAL_ERROR "ImageMagick not found - ${IMAGICK}")
ENDIF()

IF (NOT DEFINED MUTOOL)
    FIND_PROGRAM(CMAKE_MUTOOL mutool
        HINTS
            ENV LocalAppData
            ENV ProgramW6432
            ENV ProgramFiles
            ENV "ProgramFiles(x86)"
            "$ENV{SystemDrive}/Program Files"
            "$ENV{SystemDrive}/Program Files (x86)"
        PATH_SUFFIXES mupdf
        NO_CACHE
        REQUIRED
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    CMAKE_PATH(NATIVE_PATH CMAKE_MUTOOL MUTOOL)
ELSEIF(NOT EXISTS ${IMAGICK})
    MESSAGE(FATAL_ERROR "MuPDF not found - ${MUTOOL}")
ENDIF()

FUNCTION(check_aspect2 SrcImgPath)
    EXECUTE_PROCESS(
        COMMAND ${IMAGICK} identify
            -format "%h %w"
            ${SrcImgPath}
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE IDENTIFY_OUT
        ERROR_VARIABLE IDENTIFY_ERR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    STRING(REGEX REPLACE " +" ";"  IMG_DIMENSIONS ${IDENTIFY_OUT})
    LIST(POP_FRONT IMG_DIMENSIONS IMG_HEIGHT)
    LIST(POP_FRONT IMG_DIMENSIONS IMG_WIDTH)
    MATH(EXPR IMG_ASPECT "${IMG_WIDTH}*100/${IMG_HEIGHT}")
    MATH(EXPR IMG_ASPECT_DIFF "${IMG_ASPECT}-200")
    MESSAGE(DEBUG "W: ${IMG_WIDTH} H: ${IMG_HEIGHT} A: ${IMG_ASPECT} D: ${IMG_ASPECT_DIFF}")
    IF(IMG_ASPECT_DIFF GREATER_EQUAL -20 AND IMG_ASPECT_DIFF LESS_EQUAL 20)
        SET(IMG_ASPECT2 ON PARENT_SCOPE)
    ELSE()
        SET(IMG_ASPECT2 OFF PARENT_SCOPE)
    ENDIF()
    SET(IMG_HEIGHT2 ${IMG_HEIGHT} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(render_first_pdf_page SrcPdfPath DstImgPath)
    EXECUTE_PROCESS(
        COMMAND ${MUTOOL} draw
            -q
            -r 169
            -o ${DstImgPath}
            ${SrcPdfPath} 1
        ENCODING UTF-8
        COMMAND_ECHO NONE
        OUTPUT_VARIABLE IDENTIFY_OUT
        ERROR_VARIABLE IDENTIFY_ERR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
ENDFUNCTION()

FUNCTION(make_jpeg_cover SrcImgPath DstImgPath)
    check_aspect2(${SrcImgPath})
    IF(IMG_ASPECT2)
        EXECUTE_PROCESS(
            COMMAND ${IMAGICK} convert
                -units PixelsPerCentimeter
                ${SrcImgPath}
                -gravity East
				-crop "${IMG_HEIGHT2}x${IMG_HEIGHT2}+0+0" +repage
                -adaptive-resize "646400@>"
                -define jpeg:extent=96kb
                -interlace JPEG
                -strip
                -density "67x67"
                ${DstImgPath}
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
        )
    ELSE()
        EXECUTE_PROCESS(
            COMMAND ${IMAGICK} convert
                -units PixelsPerCentimeter
                ${SrcImgPath}
                -adaptive-resize "646400@>"
                -define jpeg:extent=96kb
                -interlace JPEG
                -strip
                -density "67x67"
                ${DstImgPath}
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
        )
    ENDIF()
ENDFUNCTION()

FUNCTION(make_webp_cover SrcImgPath DstImgPath)
    check_aspect2(${SrcImgPath})
    IF(IMG_ASPECT2)
        EXECUTE_PROCESS(
            COMMAND ${IMAGICK} convert
                ${SrcImgPath}
                -gravity East
				-crop "${IMG_HEIGHT2}x${IMG_HEIGHT2}+0+0" +repage
                -adaptive-resize "646400@>"
                -define webp:method=6
                -define webp:thread-level=0
                -define webp:target-size=65536
                -strip
                ${DstImgPath}
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
        )
    ELSE()
        EXECUTE_PROCESS(
            COMMAND ${IMAGICK} convert
                ${SrcImgPath}
                -adaptive-resize "646400@>"
                -define webp:method=6
                -define webp:thread-level=0
                -define webp:target-size=65536
                -strip
                ${DstImgPath}
            ENCODING UTF-8
            COMMAND_ECHO NONE
            COMMAND_ERROR_IS_FATAL ANY
        )
    ENDIF()
ENDFUNCTION()

IF(DEFINED CUE2MKC_STEM)
    SET(TMP_STEM $CUE2MKC_STEM})
ELSE()
    STRING(RANDOM LENGTH 10 TMP_SEQ)
    SET(TMP_STEM "mkcover-${TMP_SEQ}")
ENDIF()

# src
CMAKE_PATH(SET CUE2MKC_SRC_IMG_NORMALIZED "${CUE2MKC_SRC_IMG}")
CMAKE_PATH(GET CUE2MKC_SRC_IMG_NORMALIZED EXTENSION LAST_ONLY CUE2MKC_SRC_EXT)
STRING(TOLOWER ${CUE2MKC_SRC_EXT} CUE2MKC_SRC_EXT)
CMAKE_PATH(IS_RELATIVE CUE2MKC_SRC_IMG_NORMALIZED CUE2MKC_SRC_IMG_IS_RELATIVE)
IF(CUE2MKC_DST_IMG_IS_RELATIVE)
    CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${CUE2MKC_SRC_IMG_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_SRC_IMG_NORMALIZED)
ENDIF()

# dst
IF(NOT DEFINED CUE2MKC_DST_IMG)
    CMAKE_PATH(SET CUE2MKC_DST_IMG_NORMALIZED ${CUE2MKC_SRC_IMG_NORMALIZED})
    CMAKE_PATH(REPLACE_EXTENSION CUE2MKC_DST_IMG_NORMALIZED LAST_ONLY ".jpg")
ELSE()
    CMAKE_PATH(SET CUE2MKC_DST_IMG_NORMALIZED "${CUE2MKC_DST_IMG}")
ENDIF()
CMAKE_PATH(IS_RELATIVE CUE2MKC_DST_IMG_NORMALIZED CUE2MKC_DST_IMG_IS_RELATIVE)
IF(CUE2MKC_DST_IMG_IS_RELATIVE)
    CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${CUE2MKC_DST_IMG_NORMALIZED} OUTPUT_VARIABLE CUE2MKC_DST_IMG_NORMALIZED)
ENDIF()
CMAKE_PATH(GET CUE2MKC_DST_IMG_NORMALIZED EXTENSION LAST_ONLY CUE2MKC_DST_IMG_EXT)
STRING(TOLOWER ${CUE2MKC_DST_IMG_EXT} CUE2MKC_DST_IMG_EXT)

IF(CUE2MKC_SRC_EXT STREQUAL ".pdf")
    CMAKE_PATH(SET DST_PNG_PATH "${TMP_STEM}.png")
    IF(CUE2MKC_SRC_IMG_IS_RELATIVE)
        CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${DST_PNG_PATH} OUTPUT_VARIABLE DST_PNG_PATH)
    ENDIF()
    MESSAGE(STATUS "Rendering first PDF page to PNG")
    render_first_pdf_page(${CUE2MKC_SRC_IMG_NORMALIZED} ${DST_PNG_PATH})
    IF(CUE2MKC_DST_IMG_EXT STREQUAL ".jpg" OR CUE2MKC_DST_IMG_EXT STREQUAL ".jpeg")
        MESSAGE(STATUS "Converting to JPEG")
        make_jpeg_cover(${DST_PNG_PATH} ${CUE2MKC_DST_IMG_NORMALIZED})
    ELSEIF(CUE2MKC_DST_IMG_EXT STREQUAL ".webp")
        MESSAGE(STATUS "Converting to WEBP")
        make_webp_cover(${DST_PNG_PATH} ${CUE2MKC_DST_IMG_NORMALIZED})
    ELSE()
        MESSAGE(FATAL_ERROR "Unsupported image file name extension - ${CUE2MKC_DST_IMG_EXT}")
    ENDIF()
    MESSAGE(DEBUG "Remove temporary PNG file")
    FILE(REMOVE ${DST_PNG_PATH})
ELSE()
    IF(CUE2MKC_DST_IMG_EXT STREQUAL ".jpg" OR CUE2MKC_DST_IMG_EXT STREQUAL ".jpeg")
        MESSAGE(STATUS "Converting to JPEG")
        make_jpeg_cover(${CUE2MKC_SRC_IMG_NORMALIZED} ${CUE2MKC_DST_IMG_NORMALIZED})
    ELSEIF(CUE2MKC_DST_IMG_EXT STREQUAL ".webp")
        MESSAGE(STATUS "Converting to WEBP")
        make_webp_cover(${CUE2MKC_SRC_IMG_NORMALIZED} ${CUE2MKC_DST_IMG_NORMALIZED})
    ELSE()
        MESSAGE(FATAL_ERROR "Unsupported image file name extension - ${CUE2MKC_DST_IMG_EXT}")
    ENDIF()
ENDIF()
