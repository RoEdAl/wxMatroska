/*
 *      LogarithmicScale.h
 */

#ifndef _LOGARITTHMIC_SCALE_H_
#define _LOGARITTHMIC_SCALE_H_

class LogarithmicScale
{
	public:

		LogarithmicScale():
			m_fLogBase( 10 )
		{
			init( false );
		}

		LogarithmicScale( wxFloat32 fLogBase ):
			m_fLogBase( fLogBase )
		{
			wxASSERT( fLogBase > 1.0f );
			init( false );
		}

		LogarithmicScale( wxFloat32 fLogBase, bool bInv ):
			m_fLogBase( fLogBase )
		{
			wxASSERT( fLogBase > 1.0f );
			init( false );
		}

		LogarithmicScale( const LogarithmicScale& ls ):
			m_fLogBase( ls.m_fLogBase ),
			m_fLogBase1( ls.m_fLogBase1 ),
			m_fLogLogBase( ls.m_fLogLogBase ),
			m_calc_fn( ls.m_calc_fn )
		{}

		LogarithmicScale& operator =( const LogarithmicScale& ls )
		{
			m_fLogBase	  = ls.m_fLogBase;
			m_fLogBase1	  = ls.m_fLogBase1;
			m_fLogLogBase = ls.m_fLogLogBase;
			m_calc_fn	  = ls.m_calc_fn;
			return *this;
		}

		LogarithmicScale operator !() const
		{
			return LogarithmicScale( m_fLogBase, m_fLogBase1, m_fLogLogBase, IsInverted() ? &LogarithmicScale::calc_fn : &LogarithmicScale::calc_fn_inv );
		}

		wxFloat32 GetLogarithmBase() const
		{
			return m_fLogBase;
		}

		bool IsInverted() const
		{
			return ( m_calc_fn == &LogarithmicScale::calc_fn_inv );
		}

		wxFloat32 operator ()( wxFloat32 fValue ) const
		{
			wxASSERT( fValue >= 0.0f && fValue <= 1.0f );
			return ( this->*m_calc_fn )( fValue );
		}

	protected:

		typedef wxFloat32 ( LogarithmicScale::* CALC_FN )( wxFloat32 ) const;

		wxFloat32 m_fLogBase;
		wxFloat32 m_fLogBase1;
		wxFloat32 m_fLogLogBase;
		CALC_FN	  m_calc_fn;

	private:

		void init( bool bInv )
		{
			m_fLogBase1	  = m_fLogBase - 1.0f;
			m_fLogLogBase = log( m_fLogBase );
			m_calc_fn	  = bInv ? &LogarithmicScale::calc_fn_inv : &LogarithmicScale::calc_fn;
		}

		LogarithmicScale( wxFloat32 fLogBase, wxFloat32 fLogBase1, wxFloat32 fLogLogBase, CALC_FN calc_fn ):
			m_fLogBase( fLogBase ),
			m_fLogBase1( fLogBase1 ),
			m_fLogLogBase( fLogLogBase ),
			m_calc_fn( calc_fn )
		{
			wxASSERT( fLogBase > 1.0f );
		}

	protected:

		wxFloat32 calc_fn( wxFloat32 fValue ) const
		{
			return log( fValue * m_fLogBase1 + 1.0f ) / m_fLogLogBase;
		}

		wxFloat32 calc_fn_inv( wxFloat32 fValue ) const
		{
			return 1.0f - ( log( ( 1.0f - fValue ) * m_fLogBase1 + 1.0f ) / m_fLogLogBase );
		}
};
#endif

