/*
        LogarithmicScale.h
 */

#ifndef _LOGARITTHMIC_SCALE_H_
#define _LOGARITTHMIC_SCALE_H_

class LogarithmicScale
{
public:

	LogarithmicScale():
		m_fLogBase( 10 )
	{
		init();
	}

	LogarithmicScale( wxFloat32 fLogBase ):
		m_fLogBase( fLogBase )
	{
		wxASSERT( fLogBase > 1.0f );
		init();
	}

	LogarithmicScale( const LogarithmicScale& ls ):
		m_fLogBase( ls.m_fLogBase ), m_fLogLogBase( ls.m_fLogLogBase )
	{}

	LogarithmicScale& operator =( const LogarithmicScale& ls )
	{
		m_fLogBase	  = ls.m_fLogBase;
		m_fLogLogBase = ls.m_fLogLogBase;
		return *this;
	}

	wxFloat32 GetLogarithmBase() const
	{
		return m_fLogBase;
	}

	wxFloat32 operator ()( wxFloat32 fValue ) const
	{
		return log( fValue * ( m_fLogBase - 1.0f ) + 1.0f ) / m_fLogLogBase;
	}

protected:

	wxFloat32 m_fLogBase;
	wxFloat32 m_fLogLogBase;

private:

	void init()
	{
		m_fLogLogBase = log( m_fLogBase );
	}
};

#endif

