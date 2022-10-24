#include <bitset>
#include <eepp/core/debug.hpp>
#include <eepp/core/memorymanager.hpp>
#include <eepp/core/string.hpp>
#include <eepp/system/lock.hpp>
#include <eepp/system/process.hpp>
#include <thirdparty/subprocess/subprocess.h>
#include <vector>

#ifdef EE_PLATFORM_POSIX
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#elif EE_PLATFORM == EE_PLATFORM_WIN
#include <windows.h>
#endif

namespace EE { namespace System {

#define PROCESS_PTR ( (struct subprocess_s*)mProcess )

Process::Process() {}

Process::Process( const std::string& command, const Uint32& options, const size_t& bufferSize ) :
	mBufferSize( bufferSize ) {
	create( command, options );
}

Process::~Process() {
	if ( mStdOutThread.joinable() )
		mStdOutThread.join();
	if ( mStdErrThread.joinable() )
		mStdErrThread.join();
	destroy();
	eeFree( mProcess );
}

bool Process::create( const std::string& command, const Uint32& options,
					  const std::map<std::string, std::string>& environment ) {
	if ( mProcess )
		return false;
	std::vector<std::string> cmdArr = String::split( command, " ", "", "\"", true );
	std::vector<const char*> strings;
	for ( size_t i = 0; i < cmdArr.size(); ++i )
		strings.push_back( cmdArr[i].c_str() );
	strings.push_back( NULL );
	mProcess = eeMalloc( sizeof( subprocess_s ) );
	memset( mProcess, 0, sizeof( subprocess_s ) );
	if ( !environment.empty() ) {
		std::vector<std::string> envArr;
		std::vector<const char*> envStrings;
		for ( const auto& pair : environment ) {
			envArr.push_back( String::format( "%s=%s", pair.first.c_str(), pair.second.c_str() ) );
			envStrings.push_back( envArr[envArr.size() - 1].c_str() );
		}
		envStrings.push_back( NULL );
		return 0 == subprocess_create_ex( strings.data(), options, envStrings.data(), PROCESS_PTR );
	}
	return 0 == subprocess_create( strings.data(), options, PROCESS_PTR );
}

size_t Process::readAllStdOut( std::string& buffer ) {
	size_t bytesRead = 0;
	size_t totalBytesRead = 0;
	const size_t chunkSize = mBufferSize;
	totalBytesRead = bytesRead = readStdOut( (char* const)buffer.c_str(), buffer.size() );
	while ( bytesRead != 0 && isAlive() && !mShuttingDown ) {
		bytesRead = readStdOut( (char* const)buffer.c_str() + bytesRead, chunkSize );
		if ( bytesRead ) {
			totalBytesRead += bytesRead;
			buffer.resize( totalBytesRead + chunkSize );
		}
	}
	return totalBytesRead;
}

size_t Process::readStdOut( std::string& buffer ) {
	return readStdOut( (char* const)buffer.c_str(), buffer.size() );
}

size_t Process::readStdOut( char* const buffer, const size_t& size ) {
	eeASSERT( mProcess != nullptr );
	return subprocess_read_stdout( PROCESS_PTR, buffer, size );
}

size_t Process::readAllStdErr( std::string& buffer ) {
	size_t bytesRead = 0;
	size_t totalBytesRead = 0;
	const size_t chunkSize = 4096;
	totalBytesRead = bytesRead = readStdErr( (char* const)buffer.c_str(), buffer.size() );
	while ( bytesRead != 0 && isAlive() && !mShuttingDown ) {
		bytesRead = readStdErr( (char* const)buffer.c_str() + bytesRead, chunkSize );
		if ( bytesRead ) {
			totalBytesRead += bytesRead;
			buffer.resize( totalBytesRead + chunkSize );
		}
	}
	return totalBytesRead;
}

size_t Process::readStdErr( std::string& buffer ) {
	return readStdErr( (char* const)buffer.c_str(), buffer.size() );
}

size_t Process::readStdErr( char* const buffer, const size_t& size ) {
	eeASSERT( mProcess != nullptr );
	return subprocess_read_stderr( PROCESS_PTR, buffer, size );
}

size_t Process::write( const char* buffer, const size_t& size ) {
	eeASSERT( mProcess != nullptr );
	Lock l( mStdInMutex );
	FILE* stdInFile = subprocess_stdin( PROCESS_PTR );
	return fwrite( buffer, sizeof( char ), size, stdInFile );
}

size_t Process::write( const std::string& buffer ) {
	return write( buffer.c_str(), buffer.size() );
}

bool Process::join( int* const returnCodeOut ) {
	eeASSERT( mProcess != nullptr );
	return 0 == subprocess_join( PROCESS_PTR, returnCodeOut );
}

bool Process::kill() {
	eeASSERT( mProcess != nullptr );
	return 0 == subprocess_terminate( PROCESS_PTR );
}

bool Process::destroy() {
	eeASSERT( mProcess != nullptr );
	return 0 == subprocess_destroy( PROCESS_PTR );
}

bool Process::isAlive() {
	eeASSERT( mProcess != nullptr );
	return 0 != subprocess_alive( PROCESS_PTR );
}

FILE* Process::getStdIn() const {
	eeASSERT( mProcess != nullptr );
	return subprocess_stdin( PROCESS_PTR );
}

FILE* Process::getStdOut() const {
	eeASSERT( mProcess != nullptr );
	return subprocess_stdout( PROCESS_PTR );
}

FILE* Process::getStdErr() const {
	eeASSERT( mProcess != nullptr );
	return subprocess_stderr( PROCESS_PTR );
}

void Process::startShutdown() {
	mShuttingDown = true;
}

void Process::startAsyncRead( ReadFn readStdOut, ReadFn readStdErr ) {
	eeASSERT( mProcess != nullptr );
	mReadStdOutFn = readStdOut;
	mReadStdErrFn = readStdErr;
#if EE_PLATFORM == EE_PLATFORM_WIN
	// TODO: Implement WaitForMultipleObjectsEx
	void* stdOutFd =
		SUBPROCESS_PTR_CAST( void*, _get_osfhandle( _fileno( PROCESS_PTR->stdout_file ) ) );
	void* stdErrFd =
		SUBPROCESS_PTR_CAST( void*, _get_osfhandle( _fileno( PROCESS_PTR->stderr_file ) ) );
	if ( stdOutFd ) {
		mStdOutThread = std::thread( [this, stdOutFd]() {
			DWORD n;
			std::unique_ptr<char[]> buffer( new char[mBufferSize] );
			while ( !mShuttingDown ) {
				BOOL bSuccess = ReadFile( stdOutFd, static_cast<CHAR*>( buffer.get() ),
										  static_cast<DWORD>( mBufferSize ), &n, nullptr );
				if ( !bSuccess || n == 0 )
					break;
				mReadStdOutFn( buffer.get(), static_cast<size_t>( n ) );
			}
		} );
	}
	if ( stdErrFd ) {
		mStdErrThread = std::thread( [this, stdErrFd]() {
			DWORD n;
			std::unique_ptr<char[]> buffer( new char[mBufferSize] );
			while ( !mShuttingDown ) {
				BOOL bSuccess = ReadFile( stdErrFd, static_cast<CHAR*>( buffer.get() ),
										  static_cast<DWORD>( mBufferSize ), &n, nullptr );
				if ( !bSuccess || n == 0 )
					break;
				mReadStdErrFn( buffer.get(), static_cast<size_t>( n ) );
			}
		} );
	}
#elif defined( EE_PLATFORM_POSIX )
	mStdOutThread = std::thread( [this] {
		auto stdOutFd = fileno( PROCESS_PTR->stdout_file );
		auto stdErrFd = PROCESS_PTR->stderr_file ? fileno( PROCESS_PTR->stderr_file ) : 0;
		std::vector<pollfd> pollfds;
		std::bitset<2> fdIsStdOut;
		if ( stdOutFd ) {
			fdIsStdOut.set( pollfds.size() );
			pollfds.emplace_back();
			pollfds.back().fd =
				fcntl( stdOutFd, F_SETFL, fcntl( stdOutFd, F_GETFL ) | O_NONBLOCK ) == 0 ? stdOutFd
																						 : -1;
			pollfds.back().events = POLLIN;
		}
		if ( stdErrFd ) {
			pollfds.emplace_back();
			pollfds.back().fd =
				fcntl( stdErrFd, F_SETFL, fcntl( stdErrFd, F_GETFL ) | O_NONBLOCK ) == 0 ? stdErrFd
																						 : -1;
			pollfds.back().events = POLLIN;
		}
		auto buffer = std::unique_ptr<char[]>( new char[mBufferSize] );
		bool anyOpen = !pollfds.empty();
		while ( anyOpen && !mShuttingDown &&
				( poll( pollfds.data(), static_cast<nfds_t>( pollfds.size() ), -1 ) > 0 ||
				  errno == EINTR ) ) {
			anyOpen = false;
			for ( size_t i = 0; i < pollfds.size(); ++i ) {
				if ( pollfds[i].fd >= 0 ) {
					if ( pollfds[i].revents & POLLIN ) {
						const ssize_t n = read( pollfds[i].fd, buffer.get(), mBufferSize );
						if ( n > 0 ) {
							if ( fdIsStdOut[i] )
								mReadStdOutFn( buffer.get(), static_cast<size_t>( n ) );
							else
								mReadStdErrFn( buffer.get(), static_cast<size_t>( n ) );
						} else if ( n < 0 && errno != EINTR && errno != EAGAIN &&
									errno != EWOULDBLOCK ) {
							pollfds[i].fd = -1;
							continue;
						}
					}
					if ( pollfds[i].revents & ( POLLERR | POLLHUP | POLLNVAL ) ) {
						pollfds[i].fd = -1;
						continue;
					}
					anyOpen = true;
				}
			}
		}
	} );
#endif
}

}} // namespace EE::System