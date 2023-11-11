int init();

int start();

int stop();

int create_headers(
	struct curl_slist *headers,
	char *headers_raw[],
	int n_headers
	);
