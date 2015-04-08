/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/imagjpeg.cpp
// Purpose:     wxImage JPEG handler
// Author:      Vaclav Slavik
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "StdWx.h"
#include <wxCueFile/wxMozJpegImageHandler.h>

#if wxUSE_IMAGE
// A hack based on one from tif_jpeg.c to overcome the problem on Windows
// of rpcndr.h defining boolean with a different type to the jpeg headers.
//
// This hack is only necessary for an external jpeg library, the builtin one
// usually used on Windows doesn't use the type boolean, so always works.
//
#ifdef wxHACK_BOOLEAN
#define HAVE_BOOLEAN
#define boolean wxHACK_BOOLEAN
#endif

#define DONT_USE_EXTERN_C

namespace mozjpg
{
    #include <jpeglib.h>
}

#ifndef HAVE_WXJPEG_BOOLEAN
typedef mozjpg::boolean wxjpeg_boolean;
#endif

#include "wx/filefn.h"
#include "wx/wfstream.h"

//-----------------------------------------------------------------------------
// wxMozJpegHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxMozJpegHandler, wxImageHandler )

#if wxUSE_STREAMS

//------------- JPEG Data Source Manager

namespace
{
    static const size_t JPEG_IO_BUFFER_SIZE = 2048;
    typedef struct
    {
        struct mozjpg::jpeg_source_mgr pub;   /* public fields */

        mozjpg::JOCTET* buffer;               /* start of buffer */
        wxInputStream *stream;
    } wx_source_mgr;

    typedef wx_source_mgr * wx_src_ptr;

    void wx_init_source( mozjpg::j_decompress_ptr WXUNUSED( cinfo ) )
    {}

    wxjpeg_boolean wx_fill_input_buffer( mozjpg::j_decompress_ptr cinfo )
    {
        wx_src_ptr src = (wx_src_ptr)cinfo->src;

        src->pub.next_input_byte = src->buffer;
        src->pub.bytes_in_buffer = src->stream->Read( src->buffer, JPEG_IO_BUFFER_SIZE ).LastRead( );

        if (src->pub.bytes_in_buffer == 0) // check for end-of-stream
        {
            // Insert a fake EOI marker
            src->buffer[0] = 0xFF;
            src->buffer[1] = JPEG_EOI;
            src->pub.bytes_in_buffer = 2;
        }
        return TRUE;
    }

    void wx_skip_input_data( mozjpg::j_decompress_ptr cinfo, long num_bytes )
    {
        if (num_bytes > 0)
        {
            wx_src_ptr src = (wx_src_ptr)cinfo->src;

            while (num_bytes > (long)src->pub.bytes_in_buffer)
            {
                num_bytes -= (long)src->pub.bytes_in_buffer;
                src->pub.fill_input_buffer( cinfo );
            }
            src->pub.next_input_byte += (size_t)num_bytes;
            src->pub.bytes_in_buffer -= (size_t)num_bytes;
        }
    }

    void wx_term_source( mozjpg::j_decompress_ptr cinfo )
    {
        wx_src_ptr src = (wx_src_ptr)cinfo->src;

        if (src->pub.bytes_in_buffer > 0)
            src->stream->SeekI( -(long)src->pub.bytes_in_buffer, wxFromCurrent );
        delete[] src->buffer;
    }

    // JPEG error manager:

    struct wx_error_mgr: public mozjpg::jpeg_error_mgr
    {
        jmp_buf setjmp_buffer;    /* for return to caller */
    };

    /*
    * Here's the routine that will replace the standard error_exit method:
    */

    void wx_error_exit( mozjpg::j_common_ptr cinfo )
    {
        /* cinfo->err really points to a wx_error_mgr struct, so coerce pointer */
        wx_error_mgr * const jerr = (wx_error_mgr *)cinfo->err;

        /* Always display the message. */
        /* We could postpone this until after returning, if we chose. */
        (*cinfo->err->output_message) (cinfo);

        /* Return control to the setjmp point */
        longjmp( jerr->setjmp_buffer, 1 );
    }

    /*
    * This will replace the standard output_message method when the user
    * wants us to be silent (verbose==false). We must have such method instead of
    * simply using NULL for cinfo->err->output_message because it's called
    * unconditionally from within libjpeg when there's "garbage input".
    */
    void wx_ignore_message( mozjpg::j_common_ptr WXUNUSED( cinfo ) )
    {}

    void wx_jpeg_io_src( mozjpg::j_decompress_ptr cinfo, wxInputStream& infile )
    {
        wx_src_ptr src;

        if (cinfo->src == NULL)
        {    /* first time for this JPEG object? */
            cinfo->src = (struct mozjpg::jpeg_source_mgr *)
                (*cinfo->mem->alloc_small) ((mozjpg::j_common_ptr)cinfo, JPOOL_PERMANENT,
                sizeof(wx_source_mgr));
        }
        src = (wx_src_ptr)cinfo->src;
        src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
        src->buffer = new mozjpg::JOCTET[JPEG_IO_BUFFER_SIZE];
        src->pub.next_input_byte = NULL; /* until buffer loaded */
        src->stream = &infile;

        src->pub.init_source = wx_init_source;
        src->pub.fill_input_buffer = wx_fill_input_buffer;
        src->pub.skip_input_data = wx_skip_input_data;
        src->pub.resync_to_restart = mozjpg::jpeg_resync_to_restart; /* use default method */
        src->pub.term_source = wx_term_source;
    }

    inline void wx_cmyk_to_rgb( unsigned char* rgb, const unsigned char* cmyk )
    {
        int k = 255 - cmyk[3];
        int k2 = cmyk[3];
        int c;

        c = k + k2 * (255 - cmyk[0]) / 255;
        rgb[0] = (unsigned char)((c > 255) ? 0 : (255 - c));

        c = k + k2 * (255 - cmyk[1]) / 255;
        rgb[1] = (unsigned char)((c > 255) ? 0 : (255 - c));

        c = k + k2 * (255 - cmyk[2]) / 255;
        rgb[2] = (unsigned char)((c > 255) ? 0 : (255 - c));
    }
}

// temporarily disable the warning C4611 (interaction between '_setjmp' and
// C++ object destruction is non-portable) - I don't see any dtors here
#ifdef __VISUALC__
#pragma warning(disable:4611)
#endif /* VC++ */

bool wxMozJpegHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED( index ) )
{
    wxCHECK_MSG( image, false, "NULL image pointer" );

    struct mozjpg::jpeg_decompress_struct cinfo;
    wx_error_mgr jerr;
    unsigned char *ptr;

    // save this before calling Destroy()
    const unsigned maxWidth = image->GetOptionInt( wxIMAGE_OPTION_MAX_WIDTH ),
        maxHeight = image->GetOptionInt( wxIMAGE_OPTION_MAX_HEIGHT );
    image->Destroy( );

    cinfo.err = mozjpg::jpeg_std_error( &jerr );
    jerr.error_exit = wx_error_exit;

    if (!verbose)
        cinfo.err->output_message = wx_ignore_message;

    /* Establish the setjmp return context for wx_error_exit to use. */
    if (setjmp( jerr.setjmp_buffer ))
    {
        /* If we get here, the JPEG code has signaled an error.
        * We need to clean up the JPEG object, close the input file, and return.
        */
        if (verbose)
        {
            wxLogError( _( "JPEG: Couldn't load - file is probably corrupted." ) );
        }
        (cinfo.src->term_source)(&cinfo);
        jpeg_destroy_decompress( &cinfo );
        if (image->IsOk( )) image->Destroy( );
        return false;
    }

    mozjpg::jpeg_CreateDecompress( &cinfo, JPEG_LIB_VERSION, sizeof(struct mozjpg::jpeg_decompress_struct) );
    wx_jpeg_io_src( &cinfo, stream );
    mozjpg::jpeg_read_header( &cinfo, TRUE );

    int bytesPerPixel;
    if ((cinfo.out_color_space == mozjpg::JCS_CMYK) || (cinfo.out_color_space == mozjpg::JCS_YCCK))
    {
        cinfo.out_color_space = mozjpg::JCS_CMYK;
        bytesPerPixel = 4;
    }
    else // all the rest is treated as RGB
    {
        cinfo.out_color_space = mozjpg::JCS_RGB;
        bytesPerPixel = 3;
    }

    // scale the picture to fit in the specified max size if necessary
    if (maxWidth > 0 || maxHeight > 0)
    {
        unsigned& scale = cinfo.scale_denom;
        while ((maxWidth && (cinfo.image_width / scale > maxWidth)) ||
                (maxHeight && (cinfo.image_height / scale > maxHeight)))
        {
            scale *= 2;
        }
    }

    mozjpg::jpeg_start_decompress( &cinfo );

    image->Create( cinfo.output_width, cinfo.output_height );
    if (!image->IsOk( ))
    {
        mozjpg::jpeg_finish_decompress( &cinfo );
        mozjpg::jpeg_destroy_decompress( &cinfo );
        return false;
    }
    image->SetMask( false );
    ptr = image->GetData( );

    unsigned stride = cinfo.output_width * bytesPerPixel;
    mozjpg::JSAMPARRAY tempbuf = (*cinfo.mem->alloc_sarray)
        ((mozjpg::j_common_ptr)&cinfo, JPOOL_IMAGE, stride, 1);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        mozjpg::jpeg_read_scanlines( &cinfo, tempbuf, 1 );
        if (cinfo.out_color_space == mozjpg::JCS_RGB)
        {
            memcpy( ptr, tempbuf[0], stride );
            ptr += stride;
        }
        else // CMYK
        {
            const unsigned char* inptr = (const unsigned char*)tempbuf[0];
            for (size_t i = 0; i < cinfo.output_width; i++)
            {
                wx_cmyk_to_rgb( ptr, inptr );
                ptr += 3;
                inptr += 4;
            }
        }
    }

    // set up resolution if available: it's part of optional JFIF APP0 chunk
    if (cinfo.saw_JFIF_marker)
    {
        image->SetOption( wxIMAGE_OPTION_RESOLUTIONX, cinfo.X_density );
        image->SetOption( wxIMAGE_OPTION_RESOLUTIONY, cinfo.Y_density );

        // we use the same values for this option as libjpeg so we don't need
        // any conversion here
        image->SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT, cinfo.density_unit );
    }

    if (cinfo.image_width != cinfo.output_width || cinfo.image_height != cinfo.output_height)
    {
        // save the original image size
        image->SetOption( wxIMAGE_OPTION_ORIGINAL_WIDTH, cinfo.image_width );
        image->SetOption( wxIMAGE_OPTION_ORIGINAL_HEIGHT, cinfo.image_height );
    }

    mozjpg::jpeg_finish_decompress( &cinfo );
    mozjpg::jpeg_destroy_decompress( &cinfo );
    return true;
}

namespace
{
    typedef struct
    {
        struct mozjpg::jpeg_destination_mgr pub;

        wxOutputStream *stream;
        mozjpg::JOCTET * buffer;
    } wx_destination_mgr;

    typedef wx_destination_mgr * wx_dest_ptr;

    static const size_t OUTPUT_BUF_SIZE = 4096;    /* choose an efficiently fwrite'able size */

    void wx_init_destination( mozjpg::j_compress_ptr cinfo )
    {
        wx_dest_ptr dest = (wx_dest_ptr)cinfo->dest;

        /* Allocate the output buffer --- it will be released when done with image */
        dest->buffer = (mozjpg::JOCTET *)
            (*cinfo->mem->alloc_small) ((mozjpg::j_common_ptr)cinfo, JPOOL_IMAGE,
            OUTPUT_BUF_SIZE * sizeof(mozjpg::JOCTET));
        dest->pub.next_output_byte = dest->buffer;
        dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    }

    wxjpeg_boolean wx_empty_output_buffer( mozjpg::j_compress_ptr cinfo )
    {
        wx_dest_ptr dest = (wx_dest_ptr)cinfo->dest;

        dest->stream->Write( dest->buffer, OUTPUT_BUF_SIZE );
        dest->pub.next_output_byte = dest->buffer;
        dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
        return TRUE;
    }

    void wx_term_destination( mozjpg::j_compress_ptr cinfo )
    {
        wx_dest_ptr dest = (wx_dest_ptr)cinfo->dest;
        size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
        /* Write any data remaining in the buffer */
        if (datacount > 0)
            dest->stream->Write( dest->buffer, datacount );
    }

    void wx_jpeg_io_dest( mozjpg::j_compress_ptr cinfo, wxOutputStream& outfile )
    {
        wx_dest_ptr dest;

        if (cinfo->dest == NULL)
        {    /* first time for this JPEG object? */
            cinfo->dest = (struct mozjpg::jpeg_destination_mgr *)
                (*cinfo->mem->alloc_small) ((mozjpg::j_common_ptr)cinfo, JPOOL_PERMANENT,
                sizeof(wx_destination_mgr));
        }

        dest = (wx_dest_ptr)cinfo->dest;
        dest->pub.init_destination = wx_init_destination;
        dest->pub.empty_output_buffer = wx_empty_output_buffer;
        dest->pub.term_destination = wx_term_destination;
        dest->stream = &outfile;
    }

}

bool wxMozJpegHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    struct mozjpg::jpeg_compress_struct cinfo;
    wx_error_mgr jerr;
    mozjpg::JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
    mozjpg::JSAMPLE *image_buffer;
    int stride;                /* physical row width in image buffer */

    cinfo.err = mozjpg::jpeg_std_error( &jerr );
    jerr.error_exit = wx_error_exit;

    if (!verbose)
        cinfo.err->output_message = wx_ignore_message;

    /* Establish the setjmp return context for wx_error_exit to use. */
    if (setjmp( jerr.setjmp_buffer ))
    {
        /* If we get here, the JPEG code has signaled an error.
        * We need to clean up the JPEG object, close the input file, and return.
        */
        if (verbose)
        {
            wxLogError( _( "JPEG: Couldn't save image." ) );
        }
        jpeg_destroy_compress( &cinfo );
        return false;
    }

    mozjpg::jpeg_CreateCompress( &cinfo, JPEG_LIB_VERSION, sizeof(struct mozjpg::jpeg_compress_struct) );

    wx_jpeg_io_dest( &cinfo, stream );

    cinfo.image_width = image->GetWidth( );
    cinfo.image_height = image->GetHeight( );
    cinfo.input_components = 3;
    cinfo.in_color_space = mozjpg::JCS_RGB;
    mozjpg::jpeg_set_defaults( &cinfo );

    // TODO: 3rd parameter is force_baseline, what value should this be?
    // Code says: "If force_baseline is TRUE, the computed quantization table entries
    // are limited to 1..255 for JPEG baseline compatibility."
    // 'Quality' is a number between 0 (terrible) and 100 (very good).
    // The default (in jcparam.c, jpeg_set_defaults) is 75,
    // and force_baseline is TRUE.
    if (image->HasOption( wxIMAGE_OPTION_QUALITY ))
        mozjpg::jpeg_set_quality( &cinfo, image->GetOptionInt( wxIMAGE_OPTION_QUALITY ), TRUE );

    // set the resolution fields in the output file
    int resX, resY;
    wxImageResolution res = GetResolutionFromOptions( *image, &resX, &resY );
    if (res != wxIMAGE_RESOLUTION_NONE)
    {
        cinfo.X_density = resX;
        cinfo.Y_density = resY;

        // it so happens that wxIMAGE_RESOLUTION_INCHES/CM values are the same
        // ones as used by libjpeg, so we can assign them directly
        cinfo.density_unit = res;
    }

    mozjpg::jpeg_start_compress( &cinfo, TRUE );

    stride = cinfo.image_width * 3;    /* JSAMPLEs per row in image_buffer */
    image_buffer = image->GetData( );
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * stride];
        mozjpg::jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }
    mozjpg::jpeg_finish_compress( &cinfo );
    mozjpg::jpeg_destroy_compress( &cinfo );

    return true;
}

#ifdef __VISUALC__
#pragma warning(default:4611)
#endif /* VC++ */

bool wxMozJpegHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[2];

    if (!stream.Read( hdr, WXSIZEOF( hdr ) ))     // it's ok to modify the stream position here
        return false;

    return hdr[0] == 0xFF && hdr[1] == 0xD8;
}

#endif   // wxUSE_STREAMS

/*static*/ wxVersionInfo wxMozJpegHandler::GetLibraryVersionInfo( )
{
    return wxVersionInfo( "mozjpeg", JPEG_LIB_VERSION / 10, JPEG_LIB_VERSION % 10 );
}

#endif   // wxUSE_LIBJPEG
