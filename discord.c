// DEPENDENCIES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include "discord.h"


// GLOBALS
#ifndef DISC_API_KEY  // If API key not defined by user
	#define DISC_API_KEY "todo_api_key"  // Bot API key
#endif
#define DISC_BASE_URL "https://discord.com/api"  // Base URL for requests
#define DISC_UAGENT_HEADER "User-Agent: DiscordBot (https://google.com, 0)"  // Discord User Agent header
#define DISC_AUTH_HEADER "Authorization: Bot " DISC_API_KEY  // Auth header

#define N_DISC_HEADERS 3  // Number of discord headers
char *DISC_HEADERS_RAW[N_DISC_HEADERS] = {  // Raw string headers
	DISC_UAGENT_HEADER,
	DISC_AUTH_HEADER,
	"content-type: application/json"
};
struct curl_slist *DISC_HEADERS = NULL;  // Header list


// FUNCTIONS
int init() {  // Initialize bot

	curl_global_init(CURL_GLOBAL_DEFAULT);
	
	make_headers(  // Make default headers
		DISC_HEADERS,
		DISC_HEADERS_RAW,
		N_DISC_HEADERS
		);

	return 0;

}

int start() {
	return 0;
}

int stop() {

	curl_slist_free_all(DISC_HEADERS);  // Free disc headers list
	curl_global_cleanup();  // Global CURL cleanup

	return 0;
}

int make_headers(  // Make header struct from raw strings
	struct curl_slist *headers,  // Headers list to add to
	char *headers_raw[], // Array of string headers
	int n_headers  // Number of headers in headers_raw
	) {

	for (int index = 0; index < n_headers; index++) {  // Iterate over raw headers
		headers = curl_slist_append(  // Add current raw header to headers list
			headers, 
			headers_raw[index]
			);
	}

	return 0;

}

size_t req_callback(  // Callback function for requests
	char *data,  // Data delivered by request
	size_t size,
	size_t nmemb,
	char *var  // String variable to write to
	) {

	if (sizeof(data) <= sizeof(var)) {  // If output buffer is large enough
		strcpy(  // Copy result to output var
			var,
			data
			);
	} else {
		fprintf(  // Print error to stderr
			stderr,
			"Request callback: output buffer too small"
			);
	}

	return size * nmemb;  // Return same size so that CURL knows callback was successful

}

char *request(  // Send a request
	char *result,  // Variable to store result in
	char *method,  // GET, POST, PUT, DELETE, etc.
	char *url,  // URL to send it to
	struct curl_slist *headers,  // Headers
	char *content  // Request content
	) {

	CURL *curl = curl_easy_init();  // CURL handle
	CURLcode curl_result;  // CURL result

	curl_easy_setopt(  // Specify request type
		curl,
		CURLOPT_CUSTOMREQUEST,
		method
		);

	curl_easy_setopt(  // Set CURL url
		curl,
		CURLOPT_URL, 
		(url) ? (url) : (DISC_BASE_URL)  // If URL provided, set it; otherwise, use base URL
		);

	curl_easy_setopt(  // Set CURL headers
		curl,
		CURLOPT_HTTPHEADER,
		(headers) ? (headers) : (DISC_HEADERS)  // If headers provided, set them; otherwise, use default headers
		);

	if (content) {  // If content is provided
		curl_easy_setopt(  // Set content
			curl,
			CURLOPT_POSTFIELDS,
			content
			);
	}

	curl_easy_setopt(  // Set callback function
		curl,
		CURLOPT_WRITEFUNCTION,
		req_callback
		);

	curl_easy_setopt(  // Set callback function parameter
		curl,
		CURLOPT_WRITEDATA,
		result
		);

	curl_result = curl_easy_perform(curl);  // Do request

	if (curl_result != CURLE_OK) {  // If request failed

		fprintf(  // Print error to stderr
			stderr,
			"%s request: request to %s failed (%s)\n",
			method,
			url,
			curl_easy_strerror(curl_result)
			);

	}

	curl_easy_cleanup(curl);

	return result;

}


// MAIN
int main() {

	init();
	char result[2048];

	request(result, "GET", "https://reqres.in/api/users?page=2", NULL, NULL);
	printf("%s\n\n", result);

	request(result, "POST", "https://reqres.in/api/users", NULL, "{\"name\": \"asdsad\",\"job\": \"leader\"}");
	printf("%s\n\n", result);

	request(result, "PUT", "https://reqres.in/api/users/2", NULL, "{\"name\": \"asdsad\",\"job\": \"leader\"}");
	printf("%s\n\n", result);

	stop();

	return 0;

}