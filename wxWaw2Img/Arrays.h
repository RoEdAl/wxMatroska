/*
 *      Arrays.h
 */
#ifndef _ARRAYS_H_
#define _ARRAYS_H_

typedef wxTimeSpan ChapterDesc; // for now

wxDECLARE_SCOPED_ARRAY( float, wxFloatArray )
WX_DEFINE_ARRAY( ChapterDesc, ChaptersArray );
WX_DEFINE_ARRAY( wxRect2DInt, wxRect2DIntArray );
WX_DEFINE_ARRAY( wxUint64, wxUint64Array );

typedef wxScopedPtr<ChaptersArray> ChaptersArrayScopedPtr;

#endif

