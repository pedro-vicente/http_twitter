#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <openssl/ssl.h>
#include "nlohmann/json.hpp"
#include "ssl_read.hh"

#include "twitter.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_twitter
// httpGet.setHeader("Authorization", String.format("Bearer %s", bearerToken));
// httpGet.setHeader("Content-Type", "application/json");
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_twitter(const std::string& str_query, const std::string& BEARER_TOKEN)
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

  std::vector<std::string> headers;
  std::string response;

  int result = ssl_read(host, port_num, http, response, headers);
  if (result != 0)
  {
    std::cerr << "ssl_read failed" << std::endl;
    return -1;
  }

  for (size_t idx = 0; idx < headers.size(); idx++)
  {
    std::cout << headers.at(idx) << std::endl;
  }

  std::string remaining = extract_header_value(headers, "x-rate-limit-remaining");
  std::string reset_time = extract_header_value(headers, "x-rate-limit-reset");
  if (!remaining.empty())
  {
    std::cout << "Rate limit remaining: " << remaining << std::endl;
  }
  if (!reset_time.empty())
  {
    time_t reset = std::stol(reset_time);
    std::cout << "Rate limit resets at: " << std::ctime(&reset);
  }

  if (!response.size())
  {
    return -1;
  }

  if (is_chunked(headers))
  {
    response = decode_chunked(response);
  }

  std::cout << response.c_str() << std::endl;
  std::ofstream ofs("output.json");
  ofs << response;
  ofs.close();


  twitter_doc_t doc;
  try
  {
    nlohmann::json js = nlohmann::json::parse(response);
    from_json(js, doc);
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }

  for (size_t idx = 0; idx < doc.data.size(); idx++)
  {
    std::cout << "id: " << doc.data.at(idx).id << std::endl;
    std::cout << "text: " << doc.data.at(idx).text << std::endl;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//decode_chunked
//decode HTTP chunked transfer encoding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string decode_chunked(const std::string& response)
{
  std::string result;
  std::istringstream stream(response);
  std::string line;

  while (std::getline(stream, line))
  {
    if (!line.empty() && line.back() == '\r')
    {
      line.pop_back();
    }

    size_t chunk_size = 0;
    try
    {
      chunk_size = std::stoul(line, nullptr, 16);
    }
    catch (...)
    {
      continue;
    }

    // chunk size 0 means end
    if (chunk_size == 0)
    {
      break;
    }

    std::string chunk_data;
    chunk_data.resize(chunk_size);
    stream.read(&chunk_data[0], chunk_size);
    result += chunk_data;
    std::getline(stream, line);
  }

  return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//is_chunked
//check if response uses chunked transfer encoding
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool is_chunked(const std::vector<std::string>& headers)
{
  for (size_t idx = 0; idx < headers.size(); idx++)
  {
    std::string header = headers[idx];
    std::transform(header.begin(), header.end(), header.begin(), ::tolower);
    if (header.find("transfer-encoding") != std::string::npos &&
      header.find("chunked") != std::string::npos)
    {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//extract_header_value
//extract HTTP header value by key
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string extract_header_value(const std::vector<std::string>& headers, const std::string& key)
{
  std::string key_lower = key;
  std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);

  for (size_t idx = 0; idx < headers.size(); idx++)
  {
    std::string header_lower = headers[idx];
    std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);

    size_t pos = header_lower.find(key_lower + ":");
    if (pos != std::string::npos)
    {
      std::string value = headers[idx].substr(pos + key.length() + 1);
      // trim whitespace
      value.erase(0, value.find_first_not_of(" \t\r\n"));
      value.erase(value.find_last_not_of(" \t\r\n") + 1);
      return value;
    }
  }
  return "";
}

void from_json(const nlohmann::json& j, data_search_t& d)
{
  if (j.contains("id"))
  {
    j.at("id").get_to(d.id);
  }
  if (j.contains("text"))
  {
    j.at("text").get_to(d.text);
  }
}

void from_json(const nlohmann::json& j, twitter_doc_t& d)
{
  if (j.contains("data"))
  {
    j.at("data").get_to(d.data);
  }
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
