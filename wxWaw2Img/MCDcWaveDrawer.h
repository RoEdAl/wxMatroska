/*
 *      MCDcWaveDrawer.h
 */

#ifndef _MC_DC_WAVE_DRAWER_H_
#define _MC_DC_WAVE_DRAWER_H_

class McDcWaveDrawer:
	public ArrayWaveDrawer
{
	protected:

		wxScopedPtr< wxDC > m_pDc;

		McDcWaveDrawer( wxUint16 );
		void Initialize( wxDC* );

	public:

		virtual bool Save( const wxFileName& ) = 0;
};

#endif

