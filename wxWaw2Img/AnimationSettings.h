/*
 *      AnimationSettings.h
 */
#ifndef _ANIMATION_SETTINGS_H_
#define _ANIMATION_SETTINGS_H_

class AnimationSettings
{
	public:

		AnimationSettings();

	public:

		const wxColour& GetBorderColour() const;
		const wxColour& GetFillColour() const;
		wxUint16 GetBorderWidth() const;
		bool HasBitmap() const;
		const wxFileName& GetBitmapFilename() const;

	public:

		wxColour& GetBorderColour();
		wxColour&	GetFillColour();
		wxUint16&	GetBorderWidth();
		wxFileName& GetBitmapFilename();

	protected:

		wxColour   m_clrBorder;
		wxColour   m_clrFill;
		wxUint16   m_nBorderWidth;
		wxFileName m_progressBitmap;
};

#endif

