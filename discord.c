// DEPENDENCIES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <curl/curl.h>


// GLOBALS
#ifndef DISC_BASE_URL
	#define DISC_BASE_URL "https://discord.com/api"  // Base URL for requests
#endif
#ifndef N_DISC_HEADERS
	#define N_DISC_HEADERS 3
#endif


// INTERNAL STRUCTS
struct Request {  // Request
	char *method;  // GET, POST, etc.
	char *url;  // URL to send the request to
	struct curl_slist *headers;  // Headers
	char *content;  // Request content
};

struct Response {  // Response to a request
	long code;  // Response status code
	char content[2048];  // Response content
};

// DISCORD STRUCTS
struct Application {  // Discord Application object
	char *id;  // ID of the app
	char *name;  // Name of the app
	char *icon;  // Icon has of the app
	char *description;  // Description of the app
	char **rpc_origins;  // [TODO] List of RPC origin URLS, if RPC is enabled
	int bot_public;  // When 0, only the app owner can add the app to guilds
	int bot_require_code_grant;  // When 1, the app's bot will only join upon completion of the full OAuth2 code grant flow
	char *bot;  // [TODO make into user object] Partial user object for the bot user associated with the app
	char *terms_of_service_url;  // URL of the app's Terms of Service
	char *privacy_policy_url;  // URL of the app's Privacy Policy
	char *owner;  // [TODO] Partial user object for the owner of the app
};



// INTERNAL FUNCTIONS
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
		struct Request *req,
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
	struct Request *req,  // Request data
	struct Response *res  // Struct to store response in
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

// DISCORD INTERNAL FUNCTIONS
int create_disc_headers(  // Create headers for discord requests
		struct curl_slist *headers,  // Headers struct to append to
		char *api_key  // Discord bot API key
	) {

	char *uagent = "User-Agent: DiscordBot (https://google.com, 0)";
	char *auth_start = "Authorization: Bot ";  // TODO add api key
	char auth[92];
	strcpy(auth, auth_start);
	strcat(auth, api_key);

	char *headers_arr[N_DISC_HEADERS] = {  // Raw string headers
		uagent,
		auth,
		"content-type: application/json"
	};

	create_headers(
		headers,
		headers_arr,
		N_DISC_HEADERS
		);

	return 0;

}

// DISCORD FUNCTIONS
int init() {  // Initialize bot

	curl_global_init(CURL_GLOBAL_DEFAULT);

	return 0;

}

int start() {
	return 0;
}

int stop() {

	//curl_slist_free_all(DISC_HEADERS);  // Free disc headers list
	curl_global_cleanup();  // Global CURL cleanup

	return 0;
}

char *get_current_application(  // [TODO finish] Get info on current application
	char *api_key  // API key of discord bot
	) {

	struct Request *req = malloc(sizeof(struct Request));  // Create request struct
	struct Response *res = malloc(sizeof(struct Response));  // Create response struct

	set_req(req, "GET", DISC_BASE_URL "/applications/@me", NULL, NULL);  // Set request
	send_req(req, res);  // Send request

	//if (res->code == ) {

	char json[2048] = "";
	strcpy(json, res->content);

	free(req);
	free(res);

	//return json;  // TODO change to Application object

}