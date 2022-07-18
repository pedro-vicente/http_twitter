#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <openssl/ssl.h>
#include "nlohmann/json.hpp"
#include "ssl_read.hh"

const std::string BEARER_TOKEN("my_secret_token");

/////////////////////////////////////////////////////////////////////////////////////////////////////
//prototypes
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string query_v2(const std::string& s);
std::string url_encode(const std::string& s);
int get_twitter(const std::string& str_query);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  get_twitter("from:watch_econ -is:retweet");
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//data_search_t
//JSON search return is an array named "data" with each element of 2 JSON objects with keys "id" and "text"
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

void from_json(const nlohmann::json& j, data_search_t& d)
{
  j.at("id").get_to(d.id);
  j.at("text").get_to(d.text);
}

void from_json(const nlohmann::json& j, twitter_doc_t& d)
{
  j.at("data").get_to(d.data);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_twitter
// httpGet.setHeader("Authorization", String.format("Bearer %s", bearerToken));
// httpGet.setHeader("Content-Type", "application/json");
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_twitter(const std::string& str_query)
{
  const std::string port_num = "443";
  const std::string host = "api.twitter.com";
  std::string http;
  http += "GET ";
  http += query_v2(str_query);
  http += " HTTP/1.1\r\n";
  http += "Host: " + host;
  http += "\r\n";
  http += "Authorization: Bearer " + BEARER_TOKEN;
  http += "\r\n";
  http += "Content-Type: application/json";
  http += "\r\n";
  http += "Connection: close\r\n\r\n";
  std::cout << http.c_str() << std::endl;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //get response
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> header;

  std::string json = ssl_read(host, port_num, http, header);
  for (size_t idx = 0; idx < header.size(); idx++)
  {
    std::cout << header.at(idx) << std::endl;
  }
  if (!json.size())
  {
    return -1;
  }

  std::cout << json.c_str() << std::endl;
  std::ofstream ofs("output.json");
  ofs << json;
  ofs.close();


  twitter_doc_t doc;
  nlohmann::json js = nlohmann::json::parse(json);
  from_json(js, doc);

  for (size_t idx = 0; idx < doc.data.size(); idx++)
  {
    std::cout << "id: " << doc.data.at(idx).id << std::endl;
    std::cout << "text: " << doc.data.at(idx).text << std::endl;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//url_encode
// space = %20
// : = %3A
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string url_encode(const std::string& s_)
{
  std::string s(s_);

  for (size_t idx = 0; idx < s.size(); idx++)
  {
    char c = s.at(idx);
    if (c == ' ')
    {
      s.replace(idx, 1, "%20");
    }
    else if (c == ':')
    {
      s.replace(idx, 1, "%3A");
    }
  }

  std::cout << s << std::endl;
  return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//query_v2
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string query_v2(const std::string& s)
{
  std::string q = "https://api.twitter.com/2/tweets/search/recent?query=";
  q += url_encode(s);
  return q;
}

