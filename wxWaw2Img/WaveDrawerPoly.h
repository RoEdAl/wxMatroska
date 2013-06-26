/*
 *      WaveDrawerPoly.h
 */
#ifndef _WAVE_DRAWER_POLY_H_
#define _WAVE_DRAWER_POLY_H_

class PolyWaveDrawer:
	public GraphicsContextWaveDrawer
{
	public:

		PolyWaveDrawer( wxUint64,
						wxGraphicsContext*,
						const wxRect2DInt&,
						const DrawerSettings&,
						const ChaptersArrayScopedPtr& );

	protected:

		virtual void ProcessInitializer();

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

	protected:

		Point2DDoubleArray m_points;

	protected:

		wxGraphicsPath build_path() const;

		wxGraphicsBrush ceate_logarithmic_brush(
			wxDouble, wxDouble, wxDouble, wxDouble,
			const wxColour &, const wxColour &,
			bool ) const;
		void build_paths( wxGraphicsPath&, wxGraphicsPath& ) const;
};

#endif

