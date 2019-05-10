#include <eepp/ee.hpp>
#include <args/args.hxx>

// Prints the response headers
void printResponseHeaders( Http::Response& response ) {
	Http::Response::FieldTable headers = response.getHeaders();

	std::cout << "HTTP/" << response.getMajorHttpVersion() << "." << response.getMinorHttpVersion() << " " << response.getStatus() << " " << Http::Response::statusToString( response.getStatus() ) << std::endl;

	for ( auto&& head : headers ) {
		std::cout << head.first << ": " << head.second << std::endl;
	}

	std::cout << std::endl;
}

EE_MAIN_FUNC int main (int argc, char * argv []) {
	args::ArgumentParser parser("HTTP request program example");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag resume(parser, "continue", "Resume getting a partially-downloaded file", {'c',"continue"});
	args::Flag compressed(parser, "compressed", "Request compressed response", {"compressed"});
	args::ValueFlag<std::string> postData(parser, "data", "HTTP POST data", {'d', "data"});
	args::ValueFlagList<std::string> headers(parser, "header", "Pass custom header(s) to server", {'H', "header"});
	args::Flag includeHead(parser, "include", "Include protocol response headers in the output", {'i',"include"});
	args::Flag insecure(parser, "insecure", "Allow insecure server connections when using SSL", {'k',"insecure"});
	args::Flag location(parser, "location", "Follow redirects", {'L',"location"});
	args::ValueFlag<unsigned int> maxRedirs(parser, "max-redirs",  "Maximum number of redirects allowed", {"max-redirs"});
	args::ValueFlag<std::string> output(parser, "file",  "Write to file instead of stdout", {'o', "output"});
	args::ValueFlag<std::string> proxy(parser, "proxy",  "[protocol://]host[:port] Use this proxy", {'x', "proxy"});
	args::Flag progress(parser, "progress", "Show current progress of a download", {'p',"progress"});
	args::Flag verbose(parser, "verbose", "Make the operation more talkative", {'v',"verbose"});
	args::ValueFlag<std::string> requestMethod(parser, "request", "Specify request command to use", {'X', "request"});
	args::Positional<std::string> url(parser, "URL", "The URL to request");

	try {
		parser.ParseCLI(argc, argv);
	} catch (const args::Help&) {
		std::cout << parser;
		return EXIT_SUCCESS;
	} catch (const args::ParseError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	} catch (args::ValidationError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	}

	{
		// Create a new HTTP client
		Http http;
		Http::Request request;

		if ( !url ) {
			// We'll work on en.wikipedia.org
			if ( SSLSocket::isSupported() ) {
				http.setHost("https://en.wikipedia.org");
			} else {
				http.setHost("http://en.wikipedia.org");
			}

			// Creates an async http request and set the path requested
			Http::Request asyncRequest( "/wiki/" + Version::getCodename() );

			http.sendAsyncRequest([]( const Http& http, Http::Request& request, Http::Response& response ) {
				std::cout << "Got response from request: " << http.getHostName() << request.getUri() << std::endl;

				if ( response.getStatus() == Http::Response::Ok ) {
					std::cout << response.getBody() << std::endl;
				} else {
					std::cout << "Error " << response.getStatus() << std::endl;
				}
			}, asyncRequest, Seconds( 5 ) );
		} else {
			// If the user provided the URL, creates an instance of URI to parse it.
			URI uri( url.Get() );

			// If no scheme provided asume HTTP
			if ( uri.getScheme().empty() ) {
				uri = URI( "http://" + url.Get() );
			}

			// Allow insecure connections if requested
			if ( insecure ) {
				request.setValidateCertificate(false);
				request.setValidateHostname(false);
			}

			// Set the host and port from the URI
			http.setHost( uri.getHost(), uri.getPort() );

			// Set the path and query parts for the request
			request.setUri( uri.getPathEtc() );

			// Set the headers
			for ( const std::string& header : args::get(headers) ) {
				std::string::size_type pos = header.find_first_of( ':' );
				if ( std::string::npos != pos ) {
					std::string key( header.substr( 0, pos ) );
					std::string val( String::trim( header.substr( pos + 1 ) ) );
					request.setField(key, val);
				}
			}

			// Set the request method
			if ( requestMethod ) {
				request.setMethod( Http::Request::methodFromString( requestMethod.Get() ) );
			}

			// Set the post data / body
			if ( postData ) {
				request.setBody( postData.Get() );
			}

			// If progress requested print a progress on screen
			if ( progress ) {
				request.setProgressCallback( []( const Http&, const Http::Request&, size_t totalBytes, size_t currentBytes ) {
					std::cout << "\rDownloaded " << FileSystem::sizeToString( currentBytes ).c_str() << " of " << FileSystem::sizeToString( totalBytes ).c_str() << "          ";
					std::cout << std::flush;
					return true;
				});
			}

			// Set follow redirect
			request.setFollowRedirect(location.Get());

			// Set the maximun number of redirects
			if ( maxRedirs ) {
				request.setMaxRedirects(maxRedirs.Get());
			}

			// Set the proxy for the request
			char * http_proxy = getenv( "http_proxy" );
			if (  !proxy && NULL != http_proxy ) {
				http.setProxy( URI( http_proxy ) );
			} else if ( proxy ) {
				http.setProxy( URI( proxy.Get() ) );
			}

			// Request a compressed response
			if ( compressed ) {
				request.setCompressedResponse( true );
			}

			// Resume existing download
			if ( resume ) {
				request.setContinue( true );
			}

			if ( !output ) {
				// Send the request
				Http::Response response = http.sendRequest(request);

				// Check the status code and display the result
				Http::Response::Status status = response.getStatus();

				if ( includeHead )
					printResponseHeaders(response);

				if ( status == Http::Response::Ok ) {
					std::cout << response.getBody();
				} else {
					std::cout << "Error " << status << std::endl << response.getStatusDescription() << std::endl;
					std::cout << response.getBody();
				}
			} else {
				std::string path( output.Get() );

				// If output path is a directory guess a file name
				if ( FileSystem::isDirectory( path ) ) {
					FileSystem::dirPathAddSlashAtEnd( path );

					std::string lastPathSegment = uri.getLastPathSegment();

					// If there's a path end segment
					if ( !lastPathSegment.empty() ) {

						// Save with the path end segment name
						if ( !FileSystem::fileExists( path + lastPathSegment ) ) {
							path += lastPathSegment;
						} else {
							path += FileSystem::fileGetNumberedFileNameFromPath( path, lastPathSegment );
						}
					} else {
						// Create a file name if no name found
						path += FileSystem::fileGetNumberedFileNameFromPath( path, "eepp-network-file", "-" );
					}
				}

				// Download the request response into a file
				Http::Response response = http.downloadRequest(request, path, Seconds(5));

				if ( includeHead )
					printResponseHeaders(response);
			}
		}
	}

	if ( verbose )
		MemoryManager::showResults();

	return EXIT_SUCCESS;
}
