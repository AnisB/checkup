// External includes
#include <curl/curl.h>

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_rest/curl_api.h"

namespace checkup
{
	namespace curl
	{
		// Callback to receive a response
		size_t recieve_response(void* content, size_t size, size_t nmemb, CurlMessage* user_data)
		{
			bento::DynamicString* str = (bento::DynamicString*)user_data;
			size_t real_size = size * nmemb;
			str->append((char*)content, (uint32_t)real_size);
			return real_size;
		}

		CurlInstance* create_instance()
		{
			curl_global_init(CURL_GLOBAL_ALL);
			CURL* curl_instance = curl_easy_init();
			return (CurlInstance*)curl_instance;
		}

		void destroy_instance(CurlInstance* curl_instance)
		{
			CURL* curl = (CURL*)curl_instance;
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		CurlMessage* create_message(bento::IAllocator& allocator)
		{	
			return (CurlMessage*)bento::make_new<bento::DynamicString>(allocator, allocator);
		}

		const char* message_raw_data(CurlMessage* message)
		{
			bento::DynamicString* str = (bento::DynamicString*)message;
			return str->c_str();
		}

		void destroy_message(CurlMessage* message)
		{
			bento::DynamicString* str = (bento::DynamicString*)message;
			bento::make_delete<bento::DynamicString>(str->_allocator, str);
		}

		const char* request(CurlInstance* curl_instance, const char* url, const char** header, unsigned num_headers, const char* post_data, const char* proxy,
		                    CurlMessage* output_message)
		{
			// Convert the CURL ptr
			CURL* curl_ptr = (CURL*)curl_instance;

			// Lets set the authentication URL
			curl_easy_setopt(curl_ptr, CURLOPT_URL, url);

			// We do not want the header in the response
			curl_easy_setopt(curl_ptr, CURLOPT_HEADER, false ? 1L : 0L);

			// Build our header if necessary
			struct curl_slist* chunk = nullptr;
			if (num_headers != 0) {
				for (unsigned arg_idx = 0; arg_idx < num_headers; ++arg_idx) {
					chunk = curl_slist_append(chunk, header[arg_idx]);
				}
				curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, chunk);
			}

			// If we have to, we add post data
			if (post_data != nullptr) {
				curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, post_data);
			}

			// Set the response read function (to read the authentication token)
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, recieve_response);
			// Set the structure that will hold the authentication data
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, output_message);

			// If a proxy was specified, use it
			if (proxy != nullptr) {
				curl_easy_setopt(curl_ptr, CURLOPT_CUSTOMREQUEST, proxy);
			}

			// Send the request
			CURLcode res = curl_easy_perform(curl_ptr);
			if (res != CURLE_OK) {
				return curl_easy_strerror(res);
			}

			// If we built a header
			if (num_headers != 0)
				curl_slist_free_all(chunk);

			// We need to reset before leaving
			curl_easy_reset(curl_ptr);

			// All went fine
			return nullptr;
		}

		const char* upload_file(CurlInstance* curl_instance, const char* url, const char** headers, unsigned num_headers, const char* post_data, const char* file_path,
		                        CurlMessage* output_message, double* bytes_per_sec, double* upload_time)
		{
			// Convert the CURL ptr
			CURL* curl_ptr = (CURL*)curl_instance;

			// Open the file to upload (or at least try and fail in case it was not possible possible
			FILE* fd;
			fopen_s(&fd, file_path, "rb");
			if (!fd) {
				return "File to upload could not be opened";
			}

			// Lets set the authentication URL
			curl_easy_setopt(curl_ptr, CURLOPT_URL, url);

			// We do not want the header in the response
			curl_easy_setopt(curl_ptr, CURLOPT_HEADER, false ? 1L : 0L);

			// Build our header if necessary
			struct curl_slist* chunk = nullptr;
			if (num_headers != 0) {
				for (unsigned arg_idx = 0; arg_idx < num_headers; ++arg_idx) {
					chunk = curl_slist_append(chunk, headers[arg_idx]);
				}
				curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, chunk);
			}

			// If we have to, we add post data
			if (post_data != nullptr) {
				curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, post_data);
			}
			// Set the response read function (to read the authentication token)
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, recieve_response);
			// Set the structure that will hold the authentication data
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, output_message);

			// If a proxy was specified, use it
			curl_easy_setopt(curl_ptr, CURLOPT_CUSTOMREQUEST, "PUT");

			// Tell curl that we want to upload something
			curl_easy_setopt(curl_ptr, CURLOPT_UPLOAD, 1L);

			// set where to read from (on Windows you need to use READFUNCTION too)
			curl_easy_setopt(curl_ptr, CURLOPT_READDATA, fd);

			// Send the request
			CURLcode res = curl_easy_perform(curl_ptr);
			if (res != CURLE_OK) {
				return curl_easy_strerror(res);
			} else {
				curl_easy_getinfo(curl_ptr, CURLINFO_SPEED_UPLOAD, &bytes_per_sec);
				curl_easy_getinfo(curl_ptr, CURLINFO_TOTAL_TIME, &upload_time);
			}

			// If we built a header
			if (num_headers != 0)
				curl_slist_free_all(chunk);

			// We need to reset before leaving
			curl_easy_reset(curl_ptr);

			// All went fine
			return nullptr;
		}

		size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
		{
			size_t written = fwrite(ptr, size, nmemb, stream);
			return written;
		}

		const char* download_file(CurlInstance* curl_instance, const char* url, const char** headers, unsigned num_headers, const char* destination_file)
		{
			// Convert the CURL ptr
			CURL* curl_ptr = (CURL*)curl_instance;

			// Open the output file
			FILE* file_ptr = fopen(destination_file, "wb");
			if (!file_ptr) {
				return "File to download could not be opened";
			}
			// Set the url
			curl_easy_setopt(curl_ptr, CURLOPT_URL, url);

			// We do not want the header in the response
			curl_easy_setopt(curl_ptr, CURLOPT_HEADER, false ? 1L : 0L);

			// Build our header if necessary
			struct curl_slist* chunk = nullptr;
			if (num_headers != 0) {
				for (unsigned arg_idx = 0; arg_idx < num_headers; ++arg_idx) {
					chunk = curl_slist_append(chunk, headers[arg_idx]);
				}
				curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, chunk);
			}

			// Set the write data
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, file_ptr);

			// Perform the http request
			CURLcode res = curl_easy_perform(curl_ptr);

			if (CURLE_OK != res) {
				/* we failed */
				fprintf(stderr, "curl told us %d\n", res);
			}

			// If we built a header
			if (num_headers != 0)
				curl_slist_free_all(chunk);

			curl_easy_reset(curl_ptr);

			// Close the file
			fclose(file_ptr);

			// Return
			return CURLE_OK == res ? nullptr : curl_easy_strerror(res);
		}
	}
}