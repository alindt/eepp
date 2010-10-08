#include "cuiskinsimple.hpp"
#include "../graphics/cshapegroupmanager.hpp"

namespace EE { namespace UI {

cUISkinSimple::cUISkinSimple( const std::string& Name ) :
	cUISkin( Name )
{
	for ( Int32 i = 0; i < StateCount; i++ )
		mShape[ i ] = NULL;

	SetSkins();
}

cUISkinSimple::~cUISkinSimple() {
}

void cUISkinSimple::Draw( const eeFloat& X, const eeFloat& Y, const eeFloat& Width, const eeFloat& Height ) {
	cShape * tShape = mShape[ mCurState ];

	if ( NULL != tShape ) {
		tShape->DestWidth( Width );
		tShape->DestHeight( Height );

		tShape->Draw( X, Y, mColor[ mCurState ] );

		tShape->ResetDestWidthAndHeight();
	}
}

void cUISkinSimple::SetSkin( const Uint32& State ) {
	//eeASSERT ( State < StateCount );

	std::string Name( mName + "_" + cUISkin::GetSkinStateName( State ) );

	mShape[ State ] = cShapeGroupManager::instance()->GetShapeByName( Name );
}

cShape * cUISkinSimple::GetSkin( const Uint32& State ) const {
	//eeASSERT ( State < StateCount );

	return mShape[ State ];
}

void cUISkinSimple::SetState( const Uint32& State ) {
	//eeASSERT ( State < StateCount );

	if ( !Read32BitKey( &mColorDefault, State ) || NULL != mShape[ State ] ) {
		StateNormalToState( State );

		mCurState = State;
	} else
		StateBack( State );
}

void cUISkinSimple::StateNormalToState( const Uint32& State ) {
	if ( NULL == mShape[ State ] )
		mShape[ State ] = mShape[ StateNormal ];
}

}}