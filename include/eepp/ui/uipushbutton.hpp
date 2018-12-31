#ifndef EE_UICUIPUSHBUTTON_HPP
#define EE_UICUIPUSHBUTTON_HPP

#include <eepp/ui/uiwidget.hpp>
#include <eepp/ui/uitextview.hpp>
#include <eepp/ui/uiimage.hpp>

namespace EE { namespace UI {

class EE_API UIPushButton : public UIWidget {
	public:
		static UIPushButton * New();

		UIPushButton();

		virtual ~UIPushButton();

		virtual Uint32 getType() const;

		virtual bool isType( const Uint32& type ) const;

		virtual void setTheme( UITheme * Theme );

		virtual UIPushButton * setIcon( Drawable * icon );

		virtual UIImage * getIcon() const;

		virtual UIPushButton * setText( const String& text );

		virtual const String& getText();

		void setIconHorizontalMargin( Int32 margin );

		const Int32& getIconHorizontalMargin() const;

		UITextView * getTextBox() const;

		void setFont( Font * font );

		Font * getFont();

		const Color& getFontColor() const;

		void setFontColor( const Color& color );

		const Color& getFontOverColor() const;

		void setFontOverColor( const Color& color );

		const Color& getFontShadowColor() const;

		void setFontShadowColor( const Color& color );

		Uint32 getCharacterSize();

		void setCharacterSize( const Uint32& characterSize );

		const Uint32& getFontStyle() const;

		UIPushButton * setFontStyle( const Uint32& fontStyle );

		const Float & getOutlineThickness() const;

		UIPushButton * setOutlineThickness( const Float& outlineThickness );

		const Color& getOutlineColor() const;

		UIPushButton * setOutlineColor( const Color& outlineColor );

		UIFontStyleConfig getStyleConfig() const;

		void setIconMinimumSize( const Sizei& minIconSize );

		void setStyleConfig(const UIPushButtonStyleConfig & styleConfig);

		virtual bool setAttribute( const NodeAttribute& attribute, const Uint32& state = UIState::StateFlagNormal );
	protected:
		UIPushButtonStyleConfig mStyleConfig;
		UIImage * 	mIcon;
		UITextView * 	mTextBox;

		UIPushButton( const std::string& tag );

		virtual void onSizeChange();

		virtual void onAlphaChange();

		virtual void onStateChange();

		virtual void onAlignChange();

		virtual void onThemeLoaded();

		virtual void onAutoSize();

		virtual void onPaddingChange();

		virtual Uint32 onKeyDown( const KeyEvent& Event );

		virtual Uint32 onKeyUp( const KeyEvent& Event );

		void autoIconHorizontalMargin();
};

}}

#endif

