#ifndef EE_UICPROGRESSBAR_HPP
#define EE_UICPROGRESSBAR_HPP

#include "cuicontrolanim.hpp"
#include "cuitextbox.hpp"
#include "../graphics/cscrollparallax.hpp"

namespace EE { namespace UI {

class cUIProgressBar : public cUIControlAnim {
	public:
		class CreateParams : public cUITextBox::CreateParams {
			public:
				inline CreateParams() :
					cUITextBox::CreateParams(),
					DisplayPercent( false ),
					VerticalExpand( false ),
					MovementSpeed( 64.f, 0.f )
				{
				}

				inline ~CreateParams() {}

				bool DisplayPercent;
				bool VerticalExpand;
				eeVector2f MovementSpeed;
				eeRectf FillerMargin;
		};

		cUIProgressBar( const cUIProgressBar::CreateParams& Params );

		~cUIProgressBar();

		virtual void SetTheme( cUITheme * Theme );

		virtual void Progress( eeFloat Val );

		const eeFloat& Progress() const;

		virtual void TotalSteps( const eeFloat& Steps );

		const eeFloat& TotalSteps() const;

		virtual void Draw();

		void MovementSpeed( const eeVector2f& Speed );

		const eeVector2f& MovementSpeed() const;

		void VerticalExpand( const bool& VerticalExpand );

		const bool& VerticalExpand() const;

		void FillerMargin( const eeRectf& margin );

		const eeRectf& FillerMargin() const;

		void DisplayPercent( const bool& DisplayPercent );

		const bool& DisplayPercent() const;
		
		cUITextBox * TextBox() const;
	protected:
		bool				mVerticalExpand;
		eeVector2f			mSpeed;
		eeRectf 			mFillerMargin;
		bool				mDisplayPercent;

		eeFloat				mProgress;
		eeFloat				mTotalSteps;

		cScrollParallax *	mParallax;

		cUITextBox * 		mTextBox;

		virtual Uint32 OnValueChange();

		virtual void OnSizeChange();
		
		void UpdateTextBox();

};

}}

#endif
