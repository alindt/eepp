#ifndef EECSPRITE_H
#define EECSPRITE_H

#include <eepp/graphics/base.hpp>
#include <eepp/graphics/ctexturefactory.hpp>
#include <eepp/graphics/csubtexture.hpp>
#include <eepp/graphics/ctextureatlas.hpp>

namespace EE { namespace Graphics {

/** @brief A Sprite controller class, can hold and control sprites animations. */
class EE_API cSprite {
	public:
		typedef cb::Callback2< void, Uint32, cSprite * > SpriteCallback;

		enum SpriteEvents {
			SPRITE_EVENT_LAST_FRAME,
			SPRITE_EVENT_FIRST_FRAME,
			SPRITE_EVENT_END_ANIM_TO
		};

		cSprite();

		cSprite( const std::string& name, const std::string& extension = "", cTextureAtlas * SearchInTextureAtlas = NULL );

		cSprite( cSubTexture * SubTexture );

		cSprite( const Uint32& TexId, const eeFloat& DestWidth = 0, const eeFloat& DestHeight = 0, const eeFloat& offSetX = 0, const eeFloat& offSetY = 0, const eeRecti& TexSector = eeRecti(0,0,0,0) );

		virtual ~cSprite();

		cSprite& operator =( const cSprite& Other );

		/** Set the x axis position */
		void X( const eeFloat& X );

		/** Get the x axis position */
		eeFloat X() const;

		/** Set the y axis position */
		void Y( const eeFloat& Y );

		/** Get the y axis position */
		eeFloat Y() const;

		/** Set the Angle for the rendered sprite */
		void Angle( const eeFloat& Angle );

		/** Get the Angle for the rendered sprite */
		eeFloat Angle() const;

		/** Set the Scale for the rendered sprite */
		void Scale( const eeFloat& Scale );

		/** Get the Scale for the rendered sprite */
		eeFloat Scale() const;

		/** Set the Frame Number Sprite Width
		* @param FrameNum If the Frame Number is 0 use the Current Frame Number
		* @param SubFrame If the Sub Frame Number is 0 use the Current Sub Frame Number
		*/
		void Width( const eeFloat& Width, const eeUint& FrameNum, const eeUint& SubFrame);

		/** Set the current SubTexture Width */
		void Width( const eeFloat& Width );

		/** Get the Frame Number Sprite Width */
		eeFloat Width( const eeUint& FrameNum, const eeUint& SubFrame );

		/** Get the current Frame Width */
		eeFloat Width();

		/** Set the Frame Number Sprite Height
		* @param FrameNum If the Frame Number is 0 use the Current Frame Number
		* @param SubFrame If the Sub Frame Number is 0 use the Current Sub Frame Number
		*/
		void Height( const eeFloat& Height, const eeUint& FrameNum, const eeUint& SubFrame );

		/** Set the current Frame Width */
		void Height( const eeFloat& Height );

		/** Get the Frame Number Sprite Height */
		eeFloat Height( const eeUint& FrameNum, const eeUint& SubFrame );

		/** Get the current Frame Width */
		eeFloat Height();

		/** Set the sprite animation speed ( AnimSpeed equals to Animation Frames per Second ) */
		void AnimSpeed( const eeFloat& AnimSpeed );

		/** Get the sprite animation speed ( AnimSpeed equals to Animation Frames per Second ) */
		eeFloat AnimSpeed() const;

		/** @return If the animation is paused */
		bool AnimPaused() const;

		/** Set the animation paused or not */
		void AnimPaused( const bool& Pause );

		/** Set the sprite color */
		void Color( const eeColorA& Color);

		/** Get the sprite color */
		const eeColorA& Color() const;

		/** Set the sprite Color Alpha */
		void Alpha( const Uint8& Alpha );

		/** Get the sprite Color Alpha */
		const Uint8& Alpha() const;

		/** Get if the sprite it's scaled from the center */
		bool ScaleCentered() const;

		/** Set if the sprite it's scaled centered or scaled from the Left - Top position of the sprite ( default True ) */
		void ScaleCentered( const bool& ScaleCentered );

		/** Set the Current Frame */
		void CurrentFrame( eeUint CurFrame );

		/** Get the Current Frame */
		const eeUint& CurrentFrame() const;

		/** Get the Exact Current FrameData
		* @return The eeFloat fpoint of the current frame, the exact position of the interpolation.
		*/
		const eeFloat& ExactCurrentFrame() const;

		/** Set the exact current FrameData */
		void ExactCurrentFrame( const eeFloat& CurrentFrame );

		/** Set the Current Sub Frame */
		void CurrentSubFrame( const eeUint &CurSubFrame );

		/** Get the Current Sub Frame */
		const eeUint& CurrentSubFrame() const;

		/** Set the Render Type */
		void RenderType( const EE_RENDER_MODE& Effect );

		/** Get the Render Type */
		const EE_RENDER_MODE& RenderType() const;

		/** Set the Blend Mode */
		void BlendMode( const EE_BLEND_MODE& Blend );

		/** Get the Blend Mode */
		const EE_BLEND_MODE& BlendMode() const;

		/** Reset the sprite as a new one. */
		void Reset();

		/** @return The AABB (axis-aligned bounding box) */
		eeAABB GetAABB();

		/** Set the sprite position */
		void Position( const eeFloat& x, const eeFloat& y );

		/** Set the sprite position from a Vector */
		void Position( const eeVector2f& NewPos );

		/** @return The Position of the sprite */
		const eeVector2f Position() const;

		/** Update the colors of every vertex rendered of the sprite ( this will override the default color )
		* @param Color0 The Left - Top vertex color
		* @param Color1 The Left - Bottom vertex color
		* @param Color2 The Right - Bottom vertex color
		* @param Color3 The Right - Top vertex color
		*/
		void UpdateVertexColors( const eeColorA& Color0, const eeColorA& Color1, const eeColorA& Color2, const eeColorA& Color3 );

		/** This will disable the vertex colors */
		void DisableVertexColors();

		/** Creates an static sprite (no animation)
		* @param SubTexture The sprite SubTexture
		* @return True if success
		*/
		bool CreateStatic( cSubTexture * SubTexture );

		/** Creates an static sprite (no animation)
		* @param TexId The internal Texture Id
		* @param DestWidth The default destination width of the sprite
		* @param DestHeight The default destination height of the sprite
		* @param offSetX The default offset on axis x added to the x position
		* @param offSetY The default offset on axis y added to the y position
		* @param TexSector The texture sector to be rendered ( default all the texture )
		* @return True if success
		*/
		bool CreateStatic( const Uint32& TexId, const eeFloat& DestWidth = 0, const eeFloat& DestHeight = 0, const eeFloat& offSetX = 0, const eeFloat& offSetY = 0, const eeRecti& TexSector = eeRecti(0,0,0,0) );

		/** Creates an animated sprite
		* @param SubFramesNum The number of subframes of the sprite
		*/
		void CreateAnimation( const eeUint& SubFramesNum = 1 );

		/** Add a frame to the sprite (on the current sub frame)
		* @param TexId The internal Texture Id
		* @param DestWidth The destination width of the frame
		* @param DestHeight The destination height of the frame
		* @param offSetX The offset on axis x added to the x position of the frame
		* @param offSetY The offset on axis y added to the y position of the frame
		* @param TexSector The texture sector to be rendered ( default all the texture )
		* @return The frame position or 0 if fails
		*/
		eeUint AddFrame( const Uint32& TexId, const eeFloat& DestWidth = 0, const eeFloat& DestHeight = 0, const eeFloat& offSetX = 0, const eeFloat& offSetY = 0, const eeRecti& TexSector = eeRecti(0,0,0,0) );

		/** Add a frame to the sprite (on the current sub frame)
		* @param SubTexture The SubTexture used in the frame
		* @return The frame position or 0 if fails
		*/
		eeUint AddFrame( cSubTexture * SubTexture );

		/** Add a vector of SubTexture as an animation.
		* @param SubTextures The Frames
		*/
		bool AddFrames( const std::vector<cSubTexture*> SubTextures );

		/** @see cTextureAtlasManager::GetSubTexturesByPattern */
		bool AddFramesByPattern( const std::string& name, const std::string& extension = "", cTextureAtlas * SearchInTextureAtlas = NULL );

		bool AddFramesByPatternId( const Uint32& SubTextureId, const std::string& extension, cTextureAtlas * SearchInTextureAtlas );

		/** Add a frame on an specific subframe to the sprite
		* @param TexId The internal Texture Id
		* @param NumFrame The Frame Number
		* @param NumSubFrame The Sub Frame Number
		* @param DestWidth The destination width of the frame
		* @param DestHeight The destination height of the frame
		* @param offSetX The offset on axis x added to the x position of the frame
		* @param offSetY The offset on axis y added to the y position of the frame
		* @param TexSector The texture sector to be rendered ( default all the texture )
		* @return True if success
		*/
		bool AddSubFrame( const Uint32& TexId, const eeUint& NumFrame, const eeUint& NumSubFrame, const eeFloat& DestWidth = 0, const eeFloat& DestHeight = 0, const Int32& offSetX = 0, const Int32& offSetY = 0, const eeRecti& TexSector = eeRecti(0,0,0,0) );

		/** Add a frame on an specific subframe to the sprite
		* @param SubTexture The SubTexture used in the frame
		* @param NumFrame The Frame Number
		* @param NumSubFrame The Sub Frame Number
		* @return True if success
		*/
		bool AddSubFrame( cSubTexture * SubTexture, const eeUint& NumFrame, const eeUint& NumSubFrame );

		/** Draw the sprite to the screen */
		void Draw();

		/** Draw the sprite to the screen forcing the Blend Mode and the Render Type
		* @param Blend The Blend Mode
		* @param Effect The Render Type
		*/
		void Draw( const EE_BLEND_MODE& Blend, const EE_RENDER_MODE& Effect );

		/** Draw the sprite to the screen forcing the Blend Mode
		* @param Blend The Blend Mode
		*/
		void Draw( const EE_BLEND_MODE& Blend );

		/** Draw the sprite to the screen forcing the Render Type
		* @param Effect The Render Type
		*/
		void Draw( const EE_RENDER_MODE& Effect );

		/** Set the number of repeations of the animation. Any number below 0 the animation will loop. */
		void SetRepeations( const int& Repeations );

		/** Set if the class auto-animate the sprite ( default it's active ) */
		void AutoAnimate( const bool& Autoanim );

		/** Get if the class is auto-animated */
		bool AutoAnimate() const;

		/** @return The four vertex position of the Sprite */
		eeQuad2f GetQuad();

		/** @return The OffsetX of the current frame */
		Int32 OffsetX();

		/** Set the OffsetX of the current frame */
		void OffsetX( const Int32& offsetx );

		/** @return The OffsetY of the current frame */
		Int32 OffsetY();

		/** Set the OffsetY of the current frame */
		void OffsetY( const Int32& offsety );

		/** Set the Offset of the current frame */
		void Offset( const eeVector2i& offset );

		/** Reverse the animation from last frame to first mFrames. */
		void ReverseAnim( const bool& Reverse );

		/** @return If the animation is reversed */
		bool ReverseAnim() const;

		/** @return The current last frame */
		eeUint GetEndFrame();

		/** @return The number of frames */
		Uint32 GetNumFrames();

		/** Will set Reverse active and set the first frame as the last frame */
		void SetReverseFromStart();

		/** @return The Current SubTexture */
		cSubTexture * GetCurrentSubTexture();

		/** @return The SubTexture Frame from the current sub frame */
		cSubTexture * GetSubTexture( const eeUint& frame );

		/** @return The SubTexture Frame from the SubFrame */
		cSubTexture * GetSubTexture( const eeUint& frame, const eeUint& SubFrame );

		/** Start playing from
		** @param GoTo Frame that goes from 1 to Number of Frames
		*/
		void GoToAndPlay( Uint32 GoTo );

		/** Go to a frame and stop
		** @param GoTo Frame that goes from 1 to Number of Frames
		*/
		void GoToAndStop( Uint32 GoTo );

		/** Animate to frame and when reach the frame stops */
		void AnimToFrameAndStop( Uint32 GoTo );

		/** Set the sprite events callback */
		void SetEventsCallback( const SpriteCallback& Cb );

		/** Removes the current callback */
		void ClearCallback();

		/** Creates a copy of the current sprite and return it */
		cSprite * Copy();

		/** Update the sprite animation */
		void Update( const eeFloat& ElapsedTime );

		/** Update the sprite animation using the current elapsed time provided by cEngine */
		void Update();
	protected:
		enum SpriteFlags {
			SPRITE_FLAG_AUTO_ANIM				= ( 1 << 0 ),
			SPRITE_FLAG_SCALE_CENTERED			= ( 1 << 1 ),
			SPRITE_FLAG_REVERSE_ANIM			= ( 1 << 2 ),
			SPRITE_FLAG_ANIM_PAUSED				= ( 1 << 3 ),
			SPRITE_FLAG_ANIM_TO_FRAME_AND_STOP	= ( 1 << 4 ),
			SPRITE_FLAG_EVENTS_ENABLED			= ( 1 << 5 )
		};

		Uint32				mFlags;
		eeVector2f			mPos;
		eeFloat				mAngle;
		eeFloat				mScale;
		eeFloat				mAnimSpeed;

		eeColorA			mColor;
		eeColorA *			mVertexColors;

		eeInt				mRepeations; //!< Number of repetions of the animation, default -1 that equals to loop.

		EE_BLEND_MODE	mBlend;
		EE_RENDER_MODE		mEffect;

		eeUint				mCurrentFrame;
		eeFloat				mfCurrentFrame;
		eeUint				mCurrentSubFrame;
		eeUint				mSubFrames;
		eeUint				mAnimTo;

		SpriteCallback		mCb;

		class cFrame {
			public:
				std::vector<cSubTexture *> Spr;
		};
		std::vector<cFrame> mFrames;

		eeUint FramePos();

		void ClearFrame();

		eeVector2f GetRotationCenter( const eeRectf& DestRECT );

		eeUint GetFrame( const eeUint& FrameNum );

		eeUint GetSubFrame( const eeUint& SubFrame );

		void FireEvent( const Uint32& Event );
};

}}

#endif