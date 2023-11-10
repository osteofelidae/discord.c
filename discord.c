// DEPENDENCIES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void req_callback(  // Callback function for requests
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
}

char *get(  // Send a GET request
	char *result,  // Variable to store result in
	char *url,  // URL to send it to
	struct curl_slist *headers  // Headers
	) {

	CURL *http = curl_easy_init();  // CURL handle
	CURLcode curl_result;  // CURL result
	char err[512];  // Error message  [[TODO make size efficient]]

	curl_easy_setopt(  // Set CURL url
		http,
		CURLOPT_URL, 
		(url) ? (url) : (DISC_BASE_URL)  // If URL provided, set it; otherwise, use base URL
		);

	curl_easy_setopt(  // Set CURL headers
		http,
		CURLOPT_HTTPHEADER,
		(headers) ? (headers) : (DISC_HEADERS)  // If headers provided, set them; otherwise, use default headers
		);

	curl_easy_setopt(  // Set callback function
		http,
		CURLOPT_WRITEFUNCTION,
		req_callback
		);

	curl_easy_setopt(
		http,
		CURLOPT_WRITEDATA,
		result
		);

	curl_result = curl_easy_perform(http);  // Do request

	if (curl_result != CURLE_OK) {  // If request failed

		sprintf(err,  // Create error message
			"GET request: request to %s failed\n",
			url
			);

		fprintf(  // Print error to stderr
			stderr,
			err
			);

	}

	curl_easy_cleanup(http);

	return result;

}

char *post(  // Send a POST request
	char *result,  // Variable to store result in
	char *url,  // URL to send it to
	struct curl_slist *headers,  // Headers
	char *content  // POST content
	) {

	CURL *http = curl_easy_init();  // CURL handle
	CURLcode curl_result;  // CURL result
	char err[512];  // Error message  [[TODO make size efficient]]

	curl_easy_setopt(  // Set CURL url
		http,
		CURLOPT_URL, 
		(url) ? (url) : (DISC_BASE_URL)  // If URL provided, set it; otherwise, use base URL
		);

	curl_easy_setopt(  // Set CURL headers
		http,
		CURLOPT_HTTPHEADER,
		(headers) ? (headers) : (DISC_HEADERS)  // If headers provided, set them; otherwise, use default headers
		);

	curl_easy_setopt(  // Set POST content
		http,
		CURLOPT_POSTFIELDS,
		content
		);

	curl_easy_setopt(  // Set callback function
		http,
		CURLOPT_WRITEFUNCTION,
		req_callback
		);

	curl_easy_setopt(
		http,
		CURLOPT_WRITEDATA,
		result
		);

	curl_result = curl_easy_perform(http);  // Do request

	if (curl_result != CURLE_OK) {  // If request failed

		sprintf(err,  // Create error message
			"POST request: request to %s failed\n",
			url
			);

		fprintf(  // Print error to stderr
			stderr,
			err
			);

	}

	curl_easy_cleanup(http);

	return result;

}


// MAIN
int main() {

	init();
	char result[2048];

	get(result, "https://reqres.in/api/users?page=2", NULL);

	printf(result);

	post(result, "https://reqres.in/api/users", NULL, "{\"name\": \"morpheus\",\"job\": \"leader\"}");

	printf(result);

	stop();

	return 0;

}