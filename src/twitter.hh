#ifndef TWITTER_HH
#define TWITTER_HH

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// data_search_t
// JSON search return is an array named "data" with each element of 2 JSON objects with keys "id" and "text"
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct data_search_t
{
  std::string id;
  std::string text;
};

struct twitter_doc_t
{
  std::vector<data_search_t> data;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// function prototypes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_twitter(const std::string& str_query, const std::string& BEARER_TOKEN);
std::string decode_chunked(const std::string& response);
bool is_chunked(const std::vector<std::string>& headers);
std::string extract_header_value(const std::vector<std::string>& headers, const std::string& key);
std::string query_v2(const std::string& s);
std::string url_encode(const std::string& s);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// JSON serialization (required by nlohmann::json)
/////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(const nlohmann::json& j, data_search_t& d);
void from_json(const nlohmann::json& j, twitter_doc_t& d);

#endif