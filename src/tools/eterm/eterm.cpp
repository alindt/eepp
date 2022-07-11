#include <eepp/ee.hpp>
#include <eterm/terminal/terminaldisplay.hpp>

EE::Window::Window* win = NULL;
std::shared_ptr<TerminalDisplay> terminal = nullptr;

void inputCallback( InputEvent* event ) {
	if ( !terminal || event->Type == InputEvent::EventsSent )
		return;

	switch ( event->Type ) {
		case InputEvent::MouseMotion: {
			terminal->onMouseMove( win->getInput()->getMousePos(),
									 win->getInput()->getPressTrigger() );
			break;
		}
		case InputEvent::MouseButtonDown: {
			terminal->onMouseDown( win->getInput()->getMousePos(),
								   win->getInput()->getPressTrigger() );
#if EE_PLATFORM == EE_PLATFORM_ANDROID
			win->startTextInput();
#endif
			break;
		}
		case InputEvent::MouseButtonUp: {
			terminal->onMouseUp( win->getInput()->getMousePos(),
								 win->getInput()->getReleaseTrigger() );

			if ( win->getInput()->getDoubleClickTrigger() ) {
				terminal->onMouseDoubleClick( win->getInput()->getMousePos(),
											  win->getInput()->getDoubleClickTrigger() );
			}

			break;
		}
		case InputEvent::Window: {
			switch ( event->window.type ) {
				case InputEvent::WindowKeyboardFocusLost:
				case InputEvent::WindowKeyboardFocusGain: {
					terminal->setFocus( win->hasFocus() );
					break;
				}
			}
			break;
		}
		case InputEvent::KeyUp: {
			break;
		}
		case InputEvent::KeyDown: {
			terminal->onKeyDown( event->key.keysym.sym, event->key.keysym.unicode,
								 event->key.keysym.mod, event->key.keysym.scancode );
#if EE_PLATFORM == EE_PLATFORM_ANDROID
			if ( event->key.keysym.sym == KEY_RETURN ||
				 event->key.keysym.scancode == SCANCODE_RETURN ) {
				win->startTextInput();
			}
#endif
			break;
		}
		case InputEvent::TextInput: {
			terminal->onTextInput( event->text.text );
			break;
		}
		case InputEvent::VideoExpose:
			terminal->setFocus( win->hasFocus() );
			terminal->invalidate();
			break;
		case InputEvent::VideoResize: {
			terminal->setPosition( { 0, 0 } );
			terminal->setSize( win->getSize().asFloat() );
			break;
		}
	}
}

void mainLoop() {
	win->getInput()->update();

	if ( terminal )
		terminal->update();

	if ( terminal && terminal->isDirty() ) {
		win->clear();
		terminal->draw();
		win->display();
	} else {
		win->getInput()->waitEvent( Milliseconds( win->hasFocus() ? 16 : 100 ) );
	}
}

EE_MAIN_FUNC int main( int, char*[] ) {
#ifdef EE_DEBUG
	Log::instance()->setConsoleOutput( true );
	Log::instance()->setLiveWrite( true );
#endif
	DisplayManager* displayManager = Engine::instance()->getDisplayManager();
	Display* currentDisplay = displayManager->getDisplayIndex( 0 );

	std::string resPath = Sys::getProcessPath();
#if EE_PLATFORM == EE_PLATFORM_MACOSX
	if ( String::contains( resPath, "ecode.app" ) ) {
		resPath = FileSystem::getCurrentWorkingDirectory();
		FileSystem::dirAddSlashAtEnd( resPath );
	}
#elif EE_PLATFORM == EE_PLATFORM_LINUX
	if ( String::contains( resPath, ".mount_" ) ) {
		resPath = FileSystem::getCurrentWorkingDirectory();
		FileSystem::dirAddSlashAtEnd( resPath );
	}
#elif EE_PLATFORM == EE_PLATFORM_EMSCRIPTEN
	resPath += "eterm/";
#endif
	resPath += "assets";
	FileSystem::dirAddSlashAtEnd( resPath );

	displayManager->enableScreenSaver();
	displayManager->enableMouseFocusClickThrough();
	displayManager->disableBypassCompositor();

	Sizei winSize( 1280, 720 );
	win = Engine::instance()->createWindow(
		WindowSettings( winSize.getWidth(), winSize.getHeight(), "eterm", WindowStyle::Default,
						WindowBackend::Default, 32, resPath + "icon/ee.png",
						currentDisplay->getPixelDensity() ),
		ContextSettings( true ) );

	if ( win->isOpen() ) {
		win->setClearColor( RGB( 0, 0, 0 ) );

		FontTrueType* fontMono = FontTrueType::New( "monospace" );
		fontMono->loadFromFile( resPath + "fonts/DejaVuSansMonoNerdFontComplete.ttf" );
		fontMono->setEnableEmojiFallback( false );

		if ( FileSystem::fileExists( resPath + "fonts/NotoColorEmoji.ttf" ) ) {
			FontTrueType::New( "emoji-color" )
				->loadFromFile( resPath + "fonts/NotoColorEmoji.ttf" );
		} else if ( FileSystem::fileExists( resPath + "fonts/NotoEmoji-Regular.ttf" ) ) {
			FontTrueType::New( "emoji-font" )
				->loadFromFile( resPath + "fonts/NotoEmoji-Regular.ttf" );
		}

		if ( !terminal || terminal->hasTerminated() ) {
			terminal = TerminalDisplay::create( win, fontMono, PixelDensity::dpToPx( 11 ),
												win->getSize().asFloat() );
			terminal->pushEventCallback( [&]( const TerminalDisplay::Event& event ) {
				if ( event.type == TerminalDisplay::EventType::TITLE )
					win->setTitle( "eterm - " + event.eventData );
			} );

			win->startTextInput();
		}

		win->getInput()->pushCallback( &inputCallback );

		win->runMainLoop( &mainLoop );
	}

	terminal.reset();

	Engine::destroySingleton();

	MemoryManager::showResults();

	return EXIT_SUCCESS;
}