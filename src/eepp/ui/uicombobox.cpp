#include <eepp/graphics/textureregion.hpp>
#include <eepp/ui/uicombobox.hpp>

namespace EE { namespace UI {

UIComboBox* UIComboBox::New() {
	return eeNew( UIComboBox, () );
}

UIComboBox::UIComboBox() : UIWidget( "combobox" ), mDropDownList( NULL ), mButton( NULL ) {
	mDropDownList = UIDropDownList::NewWithTag( "combobox::dropdownlist" );
	mDropDownList->setParent( this );
	mDropDownList->setFriendControl( this );
	mDropDownList->setVisible( true );
	mDropDownList->setEnabled( true );
	mDropDownList->setAllowEditing( true );
	mDropDownList->setFreeEditing( true );
	mDropDownList->setTextSelection( true );
	mDropDownList->addEventListener( Event::OnPaddingChange,
									 [this]( const Event* ) { onPaddingChange(); } );
	mDropDownList->addEventListener( Event::OnSizeChange, [&]( const Event* ) { onSizeChange(); } );

	mButton = UIWidget::NewWithTag( "combobox::button" );
	mButton->setParent( this );
	mButton->setVisible( true );
	mButton->setEnabled( true );
	mButton->addEventListener( Event::OnSizeChange, [&]( const Event* ) { onSizeChange(); } );

	applyDefaultTheme();
}

UIComboBox::~UIComboBox() {}

Uint32 UIComboBox::getType() const {
	return UI_TYPE_COMBOBOX;
}

bool UIComboBox::isType( const Uint32& type ) const {
	return UIComboBox::getType() == type ? true : UIWidget::isType( type );
}

void UIComboBox::setTheme( UITheme* Theme ) {
	UIWidget::setTheme( Theme );

	mDropDownList->setThemeSkin( Theme, "combobox" );
	mDropDownList->onThemeLoaded();

	mButton->setThemeSkin( Theme, "combobox_button" );

	if ( NULL != mDropDownList->getSkin() ) {
		setInternalHeight( mDropDownList->getSkinSize().getHeight() );
	}

	if ( NULL != mButton->getSkin() ) {
		mButton->setSize( mButton->getSkinSize() );
	}

	updateControls();

	onThemeLoaded();
}

UIListBox* UIComboBox::getListBox() {
	return mDropDownList->getListBox();
}

InputTextBuffer* UIComboBox::getInputTextBuffer() {
	return mDropDownList->getInputTextBuffer();
}

const String& UIComboBox::getText() {
	return mDropDownList->getText();
}

void UIComboBox::loadFromXmlNode( const pugi::xml_node& node ) {
	beginAttributesTransaction();

	UIWidget::loadFromXmlNode( node );

	if ( NULL != mDropDownList )
		mDropDownList->loadFromXmlNode( node );

	endAttributesTransaction();

	updateControls();
}

Uint32 UIComboBox::onMessage( const NodeMessage* Msg ) {
	if ( Msg->getMsg() == NodeMessage::Click && Msg->getSender() == mButton &&
		 ( Msg->getFlags() & EE_BUTTON_LMASK && NULL != mDropDownList ) ) {
		mDropDownList->showList();
	}

	return UIWidget::onMessage( Msg );
}

void UIComboBox::updateControls() {
	if ( NULL != mDropDownList ) {
		if ( ( mFlags & UI_AUTO_SIZE ) ||
			 getSize().getHeight() < mDropDownList->getSize().getHeight() ) {
			onAutoSize();
		}

		mDropDownList->setPosition( 0, 0 );
		mDropDownList->setSize( getSize().getWidth() - mButton->getSize().getWidth(),
								mDropDownList->getSize().getHeight() );
		mDropDownList->getListBox()->setSize( getSize().getWidth(),
											  mDropDownList->getListBox()->getSize().getHeight() );
		mDropDownList->centerVertical();
	}

	if ( NULL != mButton ) {
		mButton->setPosition( getSize().getWidth() - mButton->getSize().getWidth(), 0 );
		mButton->centerVertical();
	}
}

void UIComboBox::onSizeChange() {
	updateControls();

	UIWidget::onSizeChange();
}

void UIComboBox::onPositionChange() {
	updateControls();

	UIWidget::onPositionChange();
}

void UIComboBox::onPaddingChange() {
	updateControls();

	UIWidget::onPaddingChange();
}

void UIComboBox::onAutoSize() {
	if ( NULL != mDropDownList )
		setInternalHeight( mDropDownList->getSize().getHeight() + getPadding().Top +
						   getPadding().Bottom );
}

bool UIComboBox::applyProperty( const StyleSheetProperty& attribute ) {
	if ( !checkPropertyDefinition( attribute ) )
		return false;

	switch ( attribute.getPropertyDefinition()->getPropertyId() ) {
		case PropertyId::Color:
		case PropertyId::ShadowColor:
		case PropertyId::SelectionColor:
		case PropertyId::SelectionBackColor:
		case PropertyId::FontFamily:
		case PropertyId::FontSize:
		case PropertyId::FontStyle:
		case PropertyId::Wordwrap:
		case PropertyId::TextStrokeWidth:
		case PropertyId::TextStrokeColor:
		case PropertyId::TextSelection:
		case PropertyId::TextAlign:
		case PropertyId::Text:
		case PropertyId::AllowEditing:
		case PropertyId::MaxLength:
		case PropertyId::FreeEditing:
		case PropertyId::Numeric:
		case PropertyId::AllowFloat:
		case PropertyId::Hint:
		case PropertyId::HintColor:
		case PropertyId::HintShadowColor:
		case PropertyId::HintFontSize:
		case PropertyId::HintFontFamily:
		case PropertyId::HintFontStyle:
		case PropertyId::HintStrokeWidth:
		case PropertyId::HintStrokeColor:
		case PropertyId::PopUpToRoot:
		case PropertyId::MaxVisibleItems:
		case PropertyId::SelectedIndex:
		case PropertyId::SelectedText:
		case PropertyId::ScrollBarStyle:
		case PropertyId::RowHeight:
		case PropertyId::VScrollMode:
		case PropertyId::HScrollMode:
			return mDropDownList->applyProperty( attribute );
		default:
			return UIWidget::applyProperty( attribute );
	}

	return true;
}

std::string UIComboBox::getPropertyString( const PropertyDefinition* propertyDef,
										   const Uint32& propertyIndex ) {
	if ( NULL == propertyDef )
		return "";

	switch ( propertyDef->getPropertyId() ) {
		case PropertyId::Color:
		case PropertyId::ShadowColor:
		case PropertyId::SelectionColor:
		case PropertyId::SelectionBackColor:
		case PropertyId::FontFamily:
		case PropertyId::FontSize:
		case PropertyId::FontStyle:
		case PropertyId::Wordwrap:
		case PropertyId::TextStrokeWidth:
		case PropertyId::TextStrokeColor:
		case PropertyId::TextSelection:
		case PropertyId::TextAlign:
		case PropertyId::Text:
		case PropertyId::AllowEditing:
		case PropertyId::MaxLength:
		case PropertyId::FreeEditing:
		case PropertyId::Numeric:
		case PropertyId::AllowFloat:
		case PropertyId::Hint:
		case PropertyId::HintColor:
		case PropertyId::HintShadowColor:
		case PropertyId::HintFontSize:
		case PropertyId::HintFontFamily:
		case PropertyId::HintFontStyle:
		case PropertyId::HintStrokeWidth:
		case PropertyId::HintStrokeColor:
		case PropertyId::PopUpToRoot:
		case PropertyId::MaxVisibleItems:
		case PropertyId::SelectedIndex:
		case PropertyId::SelectedText:
		case PropertyId::ScrollBarStyle:
		case PropertyId::RowHeight:
		case PropertyId::VScrollMode:
		case PropertyId::HScrollMode:
			return mDropDownList->getPropertyString( propertyDef, propertyIndex );
		default:
			return UIWidget::getPropertyString( propertyDef, propertyIndex );
	}
}

}} // namespace EE::UI
