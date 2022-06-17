#ifndef ETERMINALDISPLAY_HPP
#define ETERMINALDISPLAY_HPP

#include "system/iprocessfactory.hpp"
#include "terminal/terminaldisplay.hpp"
#include "terminal/terminalemulator.hpp"
#include <eepp/config.hpp>
#include <eepp/graphics/font.hpp>
#include <eepp/system/clock.hpp>
#include <eepp/window/inputevent.hpp>
#include <eepp/window/keycodes.hpp>
#include <eepp/window/window.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

using namespace Hexe::Terminal;
using namespace EE;
using namespace EE::Window;
using namespace EE::System;

enum class ShortcutAction { PASTE };

enum TerminalOptions {
	OPTION_NONE = 0,
	OPTION_COLOR_EMOJI = 1 << 0,
	OPTION_NO_BOXDRAWING = 1 << 1,
	OPTION_PASTE_CRLF = 1 << 2
};

struct TerminalConfig {
	int options;
};

struct TerminalKey {
	Keycode keysym;
	Uint32 mask;
	const char* string;
	int appkey;
	int appcursor;
};

struct TerminalScancode {
	Scancode scancode;
	Uint32 mask;
	std::string string;
	int appkey;
	int appcursor;
};

struct TerminalShortcut {
	Keycode keysym;
	Uint32 mask;
	ShortcutAction action;
	int appkey;
	int appcursor;
};

struct TerminalKeyMapEntry {
	Uint32 mask;
	std::string string;
	int appkey;
	int appcursor;
};

struct TerminalKeyMapShortcut {
	Uint32 mask;
	ShortcutAction action;
	int appkey;
	int appcursor;
};

class TerminalKeyMap {
  private:
	std::unordered_map<Keycode, std::vector<TerminalKeyMapEntry>> m_keyMap;
	std::unordered_map<Scancode, std::vector<TerminalKeyMapEntry>> m_platformKeyMap;
	std::unordered_map<Keycode, std::vector<TerminalKeyMapShortcut>> m_shortcuts;

  public:
	TerminalKeyMap( const TerminalKey keys[], size_t keysLen, const TerminalScancode platformKeys[],
					size_t platformKeysLen, const TerminalShortcut shortcuts[],
					size_t shortcutsLen );

	inline const std::unordered_map<Keycode, std::vector<TerminalKeyMapEntry>>& KeyMap() const {
		return m_keyMap;
	}

	inline const std::unordered_map<Scancode, std::vector<TerminalKeyMapEntry>>&
	PlatformKeyMap() const {
		return m_platformKeyMap;
	}

	inline const std::unordered_map<Keycode, std::vector<TerminalKeyMapShortcut>>&
	Shortcuts() const {
		return m_shortcuts;
	}
};

constexpr int TerminalKeyModFlags_Any = 0xFFFFFFFF;

extern TerminalKeyMap terminalKeyMap;

static const Scancode asciiScancodeTable[] = {
	SCANCODE_A, SCANCODE_B, SCANCODE_C,			  SCANCODE_D,	  SCANCODE_E,			SCANCODE_F,
	SCANCODE_G, SCANCODE_H, SCANCODE_I,			  SCANCODE_J,	  SCANCODE_K,			SCANCODE_L,
	SCANCODE_M, SCANCODE_N, SCANCODE_O,			  SCANCODE_P,	  SCANCODE_Q,			SCANCODE_R,
	SCANCODE_S, SCANCODE_T, SCANCODE_U,			  SCANCODE_V,	  SCANCODE_W,			SCANCODE_X,
	SCANCODE_Y, SCANCODE_Z, SCANCODE_LEFTBRACKET, SCANCODE_SLASH, SCANCODE_RIGHTBRACKET };

class ETerminalDisplay : public TerminalDisplay {
  public:
	static std::shared_ptr<ETerminalDisplay>
	Create( EE::Window::Window* window, Font* font,
			std::shared_ptr<Hexe::Terminal::TerminalEmulator>&& terminalEmulator,
			TerminalConfig* config = 0 );

	static std::shared_ptr<ETerminalDisplay>
	Create( EE::Window::Window* window, Font* font, int columns, int rows,
			const std::string& program, const std::vector<std::string>& args,
			const std::string& workingDir, uint32_t options = 0,
			Hexe::System::IProcessFactory* processFactory = nullptr );

	virtual void ResetColors();
	virtual int ResetColor( int index, const char* name );

	virtual void SetTitle( const char* title );
	virtual void SetIconTitle( const char* title );

	virtual void SetClipboard( const char* text );
	virtual const char* GetClipboard() const;

	virtual bool DrawBegin( int columns, int rows );
	virtual void DrawLine( Line line, int x1, int y, int x2 );
	virtual void DrawCursor( int cx, int cy, Hexe::Terminal::Glyph g, int ox, int oy,
							 Hexe::Terminal::Glyph og );
	virtual void DrawEnd();

	virtual void Update();

	void Action( ShortcutAction action );

	bool HasTerminated() const;

	void Draw( const Rectf& contentArea );

	void Draw( Vector2i pos, const Sizei& clip_rect, bool hasFocus );

	virtual void onTextInput( const Uint32& chr );

	virtual void onKeyDown( const Keycode& keyCode, const Uint32& chr, const Uint32& mod,
							const Scancode& scancode );

	Float getFontSize() const;
	void setFontSize( Float FontSize );

  protected:
	EE::Window::Window* mWindow;
	std::vector<Hexe::Terminal::Glyph> m_buffer;
	std::vector<std::pair<Color, std::string>> m_colors;
	std::shared_ptr<Hexe::Terminal::TerminalEmulator> m_terminal;
	mutable std::string mClipboard;

	Font* mFont;
	Float mFontSize{ 12 };
	int m_columns{ 0 };
	int m_rows{ 0 };
	bool m_dirty;
	bool m_checkDirty;
	int m_cursorx;
	int m_cursory;
	Hexe::Terminal::Glyph m_cursorg;
	bool m_useBoxDrawing;
	bool m_useColorEmoji;
	bool m_pasteNewlineFix;
	Clock mClock;

	ETerminalDisplay( EE::Window::Window* window, Font* font, int columns, int rows,
					  TerminalConfig* config );
};

#endif // ETERMINALDISPLAY_HPP
