/*===========================================
http_utils.c - COMP30023 Project 2
    -> implement some helper functions for 
       modules for http strings.
     * Knows nothing about sockets, just 
       operates on strings.
Names: Max Chivers, Alif Farul Azim
Project: Web Proxy
===========================================*/
#include "util/http_util.h"

#define GET_LINE_CONTINUE 1
#define STRING_MATCH 1
#define STRING_NOT_MATCH 0

/**
 * FOR CHECKING IF WE SHOULD ALLOW CACHING.
 */
#define NUM_CACHE_CONTROL_CONDITIONS 6
const char* CACHE_CONTROL_DISALLOWED[] = {
    "no-store",
    "private",
    "no-cache",
    "must-revalidate",
    "proxy-revalidate",
    "max-age=0"
};

/* --- INTERNAL FUNCTION PROTOTYPES --- */

int get_next_line(const char **p, const char **line_start, size_t *line_len);
int strncasecmp_custom(const char *s1, const char *s2, size_t n);
const char *strcasestr_custom(const char *input, const char *target);
int has_substring(const char *substring, const char *string);

/* --- DEFINING STRUCTS --- */
struct http_response {
    char *headers;         // HTTP repsonse headers (malloc'd buffer)
    size_t headers_len;    // Length of the headers buffer
    char *body;            // HTTP repsonse body (malloc'd buffer)
    size_t body_len;       // Length of the body buffer
};

/* --- FUNCTION IMPLEMENTATION --- */

/**
 * Parses the next CRLF-terminated line from a given input string.
 *
 * Given a pointer to a string `*p`, this function:
 * - Sets `*line_start` to the beginning of the next line.
 * - Sets `*line_len` to the length of the line (excluding CRLF).
 * - Advances `*p` to point just after the CRLF sequence.
 *
 * If no CRLF is found, assumes the rest of the string is a single final line.
 * If input is NULL or malformed, returns HTTP_UTIL_FAIL.
 *
 * Returns:
 * - GET_LINE_CONTINUE (1) if a line is successfully parsed (even partial line at end).
 * - HTTP_UTIL_SUCCESS (0) if input has no more data to read.
 * - HTTP_UTIL_FAIL (-1) if invalid parameters are provided.
 */
 int get_next_line(const char **p, const char **line_start, size_t *line_len) {
    // Validate input pointer and check for end of string
    if (!p || !*p || **p == '\0') return HTTP_UTIL_SUCCESS;  // No more lines to read
    
    if (line_start == NULL || line_len == NULL) return HTTP_UTIL_FAIL;

    const char *start = *p;

    // Find CRLF sequence indicating end of line
    const char *line_end = strstr(start, "\r\n");

    if (!line_end) {
        // No CRLF found, this must be the last line (could be incomplete or no trailing CRLF)
        *line_start = start;
        *line_len = strlen(start);  // Length is from start till end of string.
        *p += *line_len;            // Move pointer to end of string (no next line)
        return GET_LINE_CONTINUE;                   
    }

    // CRLF found, line is from start up to but not including CRLF.
    *line_start = start;
    *line_len = line_end - start;

    // Advance pointer past the CRLF to start of next line
        // Note: this wont segfault because of initial logic check at start.
    *p = line_end + 2;

    return GET_LINE_CONTINUE;  // Successfully read a line
}


/**
 * Case-insensitive comparison of the first `n` characters of two strings.
 */
int strncasecmp_custom(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        char c1 = tolower((unsigned char)s1[i]);
        char c2 = tolower((unsigned char)s2[i]);
        if (c1 != c2 || c1 == '\0') {
            return c1 - c2;
        }
    }
    return HTTP_UTIL_SUCCESS;
}


/**
 * Extracts the value of a specified header from a block of HTTP headers or raw block
 * 
 * Assumes headers are CRLF-terminated ("\r\n") and null-terminated at the end.
 * Assumes header names are case-insensitive.
 * Assumes `header_name` is a null-terminated string (e.g., "Host", "Content-Length").
 * 
 * Returns:
 *   A malloc'd string containing the header's value (caller must free), or
 *   NULL if the header was not found or is invalid.
 */
char *extract_header_value(const char *headers, const char *header_name) {
    if (headers == NULL || header_name == NULL) return NULL;

    const char *p = headers;
    const char *line_start;
    size_t line_len;

    size_t name_len = strlen(header_name);

    // Line-by-line check for Host header
    while (get_next_line(&p, &line_start, &line_len)) {
        if (line_len == 0) break; // End of headers

        // Find ':' separating header name and value within the current line.
        const char *colon = memchr(line_start, ':', line_len);
        if (!colon) continue;  // No colon, so note a header line.

        // Find length of the header
        size_t current_name_len = colon - line_start;

        if (current_name_len == name_len &&
            strncasecmp_custom(line_start, header_name, name_len) == HTTP_UTIL_SUCCESS) {
            // Correct header, retrieve value.
            const char *value_start = colon + 1;
            size_t value_len = line_len - (current_name_len + 1);

            // Trim leading whitespace
            while (value_len > 0 && (*value_start == ' ' || *value_start == '\t')) {
                value_start++;
                value_len--;
            }

            // Trim trailing whitespace
            while (value_len > 0 &&
                   (value_start[value_len - 1] == ' ' || value_start[value_len - 1] == '\t')) {
                value_len--;
            }

            if (value_len == 0) return NULL;

            // malloc header value
            char *value = malloc(value_len + 1);
            if (value == NULL) {
                fprintf(stderr, "Malloc error: memory allocation failed\n");
                return NULL;
            }
            memcpy(value, value_start, value_len);

            value[value_len] = '\0';

            return value;
        }
    }

    return NULL;  // Header not found.
}


/**
 * Finds the end of HTTP headers in a buffer by iterating line-by-line.
 * 
 * 
 * Returns:
 *   - The offset index immediately after the blank line ("\r\n") that terminates the headers,
 *     or -1 if the end of headers is not found within the given buffer length.
 * 
 * Uses get_next_line() to safely parse each CRLF-terminated line.
 */
int find_header_end(const char *buffer, size_t len) {
    const char *p = buffer;
    const char *line_start;
    size_t line_len;

    size_t parsed = 0;

    while (get_next_line(&p, &line_start, &line_len)) {
        parsed = p - buffer;

        if (parsed > len) {
            // Went past buffer length without finding blank line
            return HTTP_UTIL_FAIL;
        }

        if (line_len == 0) {
            // Found the blank line ending headers
            return (int)parsed;
        }
    }

    // End of headers not found within len bytes
    return HTTP_UTIL_FAIL;
}


/**
 * --- typically use for a http response ---
 * Extracts the value of the Content-Length header from the HTTP response headers.
 * 
 * Assumes `headers` is a null-terminated string containing all response headers.
 * 
 * Returns:
 *   Parsed Content-Length value as ssize_t on success,
 *   -1 if the header is not found or malformed.
 */
ssize_t retrieve_content_length(const char *headers) {
    if (headers == NULL) return HTTP_UTIL_FAIL;

    // Get the content length value
    char *content_length_str = extract_header_value(headers, CONTENT_LENGTH_HEADER_STR);
    if (content_length_str == NULL) {
        return HTTP_UTIL_FAIL;  // Header not found
    }
    
    // Manually null-terminate before \r or \n if present
    char *p = content_length_str;
    while (*p && *p != '\r' && *p != '\n') p++;
    *p = '\0';

    // Convert to integer
    char *endptr;
    ssize_t content_length = strtoll(content_length_str, &endptr, 10);  // 10 for decimal base

    // Check for errors during conversion
    if (*endptr != '\0' || content_length < 0) {
        return HTTP_UTIL_FAIL;  // malformed or negative
    }
    
    free(content_length_str); // Free earlier extract header

    return content_length;
}


/**
 * Returns the last non-empty header line before the blank line.
 * 
 * Useful for logging the "tail" of the request.
 * It returns the last non-empty line before the blank, which by structure of
 * HTTP headers, will be the last header line.
 *  
 * Returns:
 *   A const string of the last header line that is valid as long as raw_request
 *   is (without extra CRLF),
 *   or NULL if not found or headers are malformed.
 */
char *retrieve_header_tail(const char *raw_request) {
    if (raw_request == NULL) return NULL;

    const char *p = raw_request;
    const char *line_start = NULL, *prev_start = NULL;
    size_t line_len = 0, prev_len = 0;

    // Skip request line
    if (!get_next_line(&p, &line_start, &line_len)) return NULL;

    // Line-by-line skip of the headers.
    while (get_next_line(&p, &line_start, &line_len)) {
        // Blank line = end of headers
        if (line_len == 0) break;

        // Move current to previous
        prev_start = line_start;
        prev_len = line_len;
    }

    // If we never saw a header line, or prev was request line, return NULL
    if (!prev_start || prev_len == 0) return NULL;

    // malloc last line to return 
    char *result = malloc(prev_len + 1);
    if (result == NULL) return NULL;
    memcpy(result, prev_start, prev_len);
    result[prev_len] = '\0';

    return result;
}

/**
 * --- typically use for a http request ---
 * Extracts and returns the URI of the HTTP request (in the request line).
 * 
 * For a proxy, the request URI usually looks like:
 *   http://example.com/path
 * 
 * Returns:
 *   A malloc'd copy of the request URI (caller must free),
 *   or NULL if malformed.
 */
char *retrieve_request_uri(const char *raw_request) {
    if (raw_request == NULL) return NULL;

    const char *p = raw_request;
    const char *line_start;
    size_t line_len;

    // Get the request line (first line of the request)
    if (!get_next_line(&p, &line_start, &line_len)) {
        return NULL;  // Malformed or empty request
    }

    // Scan for the method ("GET")
    const char *method_end = memchr(line_start, ' ', line_len);
    if (!method_end) return NULL;

    // Skip whitespace after method
    const char *uri_start = method_end + 1;
    // check whether uri_start would go past the end of the request line 
    if (uri_start >= line_start + line_len) return NULL;

    // Find end of URI (next space before HTTP version)
    const char *uri_end = memchr(uri_start, ' ', line_start + line_len - uri_start);
    if (!uri_end || uri_end == uri_start) return NULL;

    size_t uri_len = uri_end - uri_start;

    // malloc and copy the URI
    char *uri = malloc(uri_len + 1);
    if (uri == NULL) return NULL;
    memcpy(uri, uri_start, uri_len);
    uri[uri_len] = '\0';

    return uri;
}

/**
 * Checks if a substring exists within a string (case-insensitive).
 * Performs exact substring matching - "max-age=0" will NOT match "max-age=300".
 * Returns STRING_MATCH on found substring, and STRING_NOT_MATCH otherwise
 * 
 */
int has_substring(const char *substring, const char *string) {
    size_t sub_len = strlen(substring);
    size_t str_len = strlen(string);

    // If substring is longer than string, can't match
    if (sub_len > str_len) {
        return STRING_NOT_MATCH;
    }
    
    // Check each possible starting position in the string
    for (size_t i = 0; i <= str_len - sub_len; i++) {
        // Compare substring length characters starting at position i
        if (strncasecmp_custom(string + i, substring, sub_len) == HTTP_UTIL_SUCCESS) {
            return STRING_MATCH; // Exact substring match found
        }
    }
    return STRING_NOT_MATCH; // No match found
}

/*
 * Arguments
 * req_headers -> The request's headers from which to extract the value of the cache-control header
 * 
 * Returns an IS_CACHABLE_FALSE 0 if the conditions for caching are not met
 * Returns an CACHABLE_TRUE 1 if the conditions for caching are met
 *      This is also assumed if the Cache-control is not found.
 * 
 * Conditions checked for caching 
 * 1) Check for the cache-control arguments specified in task 3(private, no-store, no-cache, max-age=0,must-revalidate, proxy-revalidate)
*/
int is_cacheable(const char *resp_headers){
    if (resp_headers == NULL) return IS_CACHEABLE_FALSE;
    // Extract the values associated with the cache-control header
    // Check if there is a cache control header first

    char *cache_control_vals = extract_header_value(resp_headers, "cache-control");

    // Check if the cache control header was successfully extracted
    if (!cache_control_vals) {
        // fprintf(stderr, "Parsing HTTP error: Cache-Control header missing or malformed\n");
        // Not found so assume we can cache it
        return IS_CACHEABLE_TRUE; 
    }

    // Check if the cache-control header value has any of the conditions for not caching.
    for(int i = 0; i < NUM_CACHE_CONTROL_CONDITIONS;i++){
        if (has_substring(CACHE_CONTROL_DISALLOWED[i], cache_control_vals) == STRING_MATCH) {
            free(cache_control_vals);
            return IS_CACHEABLE_FALSE;
        }
    }

    free(cache_control_vals);
    
    return IS_CACHEABLE_TRUE;
}

/**
 * Extracts the Cache-Control header from a header string and parses its max-age value.
 *
 * headers The full headers string.
 * The max-age value in seconds, or NO_MAX_AGE (UINT32_MAX) if not found or invalid, meaning 
 * it should be treated as to having an unlimited max age.
 */
uint32_t get_max_age_from_headers(const char *headers) {
    if (headers == NULL) return NO_MAX_AGE; // Null input check

    // Extract the value of the "Cache-Control" header (e.g., "public, max-age=12345")
    char *cache_control_value = extract_header_value(headers, "Cache-Control");
    if (cache_control_value == NULL) return NO_MAX_AGE; // Header not found

    // Find the substring "max-age=" in the header value
        // (case-insensitive check.)
    const char *max_age_str = strcasestr_custom(cache_control_value, "max-age=");
    if (!max_age_str) {
        free(cache_control_value);
        return NO_MAX_AGE; // max-age directive not present
    }

    // Move pointer to the start of the number (after "max-age=")
    max_age_str += strlen("max-age=");

    // Skip any whitespace after '='
    while (*max_age_str == ' ' || *max_age_str == '\t') max_age_str++;

    // Parse the number as an unsigned long 
    char *endptr;
    unsigned long max_age = strtoul(max_age_str, &endptr, 10);
    
    // Check if the conversion failed (no digits found)
    // or if the value exceeds what can be stored in uint32_t
    if (endptr == max_age_str || max_age > NO_MAX_AGE) {
        free(cache_control_value); // free only after checking
        return NO_MAX_AGE;
    }
    
    free(cache_control_value); // free after successful parsing

    // Safe to cast since value is in range
    return (uint32_t)max_age;
}

/**
 * Case-insensitive search for `target` in `input`.
 * Returns a pointer to the first match, or NULL if not found.
 */
const char *strcasestr_custom(const char *input, const char *target) {
    if (input == NULL || target == NULL) return NULL; // NULL check

    
    // If target is empty, match at the beginning of input
    if (*target == '\0') return input;

    // Calculate the length of the target string
    size_t target_len = 0;
    while (target[target_len]) target_len++;

    // Pointer to iterate through the input string
    const char *p = input;
    while (*p) {
        // Compare current segment with target (case-insensitive)
        if (strncasecmp_custom(p, target, target_len) == HTTP_UTIL_SUCCESS) {
            // Match found, return pointer to match location
            return p;
        }
        // Move to the next character in input
        p++;
    }

    // No match found, return NULL
    return NULL;
}
