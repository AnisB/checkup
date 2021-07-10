namespace checkup
{
    namespace curl
    {
		// Opaque structure to avoid exposing curl data
		struct CurlInstance;
		struct CurlMessage;

        // Create a curl instance, the returned instance will be used for all the requests
        CurlInstance* create_instance();

        // Destroy a previously created curl object
        void destroy_instance(CurlInstance* curlInstance);

        // Create a curl message
        CurlMessage* create_message(bento::IAllocator& allocator);

        // Clear a curl message
        void clear_message(CurlMessage* message);

        // Fetch the raw data of the message
        const char* message_raw_data(CurlMessage* message);

        // Destroy a previously created curl message
        void destroy_message(CurlMessage* message);

        // Function that is used to send requests to an url
        const char* request(CurlInstance* curl_instance, const char* url, const char** headers, unsigned num_headers, const char* post_data, const char* proxy,
                               CurlMessage* output_string);

        // Function that uploads a file from disk
        const char* upload_file(CurlInstance* curl_instance, const char* url, const char** headers, unsigned num_headers, const char* post_data, const char* file_path,
                                   CurlMessage* output_string, double* bytes_per_sec, double* upload_time);

        // Function that downloads a file to disk
        const char* download_file(CurlInstance* curl_instance, const char* url, const char** headers, unsigned num_headers, const char* destination_file);
    }
}