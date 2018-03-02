/*
 *      MyMessageOutputStderr.h
 */

#ifndef _MY_MESSAGE_OUTPUT_STDERR_H_
#define _MY_MESSAGE_OUTPUT_STDERR_H_

class MyMessageOutputStderr:
	public wxMessageOutput
{
	public:

		MyMessageOutputStderr( FILE* fp = NULL );
		virtual void Output( const wxString& );

	protected:

		// return the string with "\n" appended if it doesn't already terminate
		// with it (in which case it's returned unchanged)
		static bool AppendLineFeedIfNeeded( const wxString& str )
		{
			return ( str.empty() || *str.rbegin() != '\n' );
		}

		FILE* m_fp;
};

#endif

