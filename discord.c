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


// STRUCTS
struct request {  // Request
	char *method;  // GET, POST, etc.
	char *url;  // URL to send the request to
	struct curl_slist *headers;  // Headers
	char *content;  // Request content
};

struct response {  // Response to a request
	long code;  // Response status code
	char content[2048];  // Response content
};


// FUNCTIONS
int init() {  // Initialize bot

	curl_global_init(CURL_GLOBAL_DEFAULT);
	
	create_headers(  // Make default headers
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

int create_headers(  // Make header struct from raw strings
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

int *set_req(  // Set request struct
		struct request *req,
		char *method,  // GET, POST, etc.
		char *url,  // Url to send request to
		struct curl_slist *headers,  // Headers
		char *content  // Request content
	) {

	req->method = method;
	req->url = url;
	req->headers = headers;
	req->content = content;

	return 0;

}

int send_req(  // Send a request
	struct request *req,  // Request data
	struct response *res  // Struct to store response in
	) {

	CURL *curl = curl_easy_init();  // CURL handle
	CURLcode curl_result;  // CURL result

	curl_easy_setopt(  // Specify request type
		curl,
		CURLOPT_CUSTOMREQUEST,
		req->method
		);

	curl_easy_setopt(  // Set CURL url
		curl,
		CURLOPT_URL, 
		req->url
		);

	if (req->headers != NULL) {  // If headers are provided
		curl_easy_setopt(  // Set CURL headers
			curl,
			CURLOPT_HTTPHEADER,
			req->headers
			);
	}

	if (req->content != NULL) {  // If content is provided
		curl_easy_setopt(  // Set content
			curl,
			CURLOPT_POSTFIELDS,
			req->content
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
		res->content
		);

	curl_result = curl_easy_perform(curl);  // Do request

	if (curl_result == CURLE_OK) {  // If request succeed

		curl_easy_getinfo(  // Response code
			curl,
			CURLINFO_RESPONSE_CODE,
			&res->code
			);

	} else {  // If request failed

		fprintf(  // Print error to stderr
			stderr,
			"%s request: request to %s failed (%s)\n",
			req->method,
			req->url,
			curl_easy_strerror(curl_result)
			);

		return -1;

	}

	curl_easy_cleanup(curl);  // Clean up

	return 0;

}


// MAIN
int main() {

	init();

	struct request *imp = malloc(sizeof(struct request));
	struct response *result = malloc(sizeof(struct response));

	set_req(imp, "GET", "https://reqres.in/api/users", NULL, NULL);
	send_req(imp, result);
	printf("%d %s\n\n", result->code, result->content);

	set_req(imp, "POST", "https://reqres.in/api/users", NULL, "{\"name\": \"asdsad\",\"job\": \"leader\"}");
	send_req(imp, result);
	printf("%d %s\n\n", result->code, result->content);

	set_req(imp, "PUT", "https://reqres.in/api/users/2", NULL, "{\"name\": \"asdsad\",\"job\": \"leader\"}");
	send_req(imp, result);
	printf("%d %s\n\n", result->code, result->content);

	free(imp);
	free(result);

	stop();

	return 0;

}