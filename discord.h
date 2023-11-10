int init();

int start();

int stop();

int make_headers(
	struct curl_slist *headers,
	char *headers_raw[],
	int n_headers
	);

char *post(
	char result[],
	char *url,
	struct curl_slist *headers,
	char *content
	);