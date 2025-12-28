#include <iostream>
#include <fstream>
#include <sstream>
#include "twitter.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//extract_value
//extract JSON string value given a key
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string extract_value(const std::string& content, const std::string& key)
{
  size_t pos_key = content.find("\"" + key + "\"");
  if (pos_key == std::string::npos) return "";

  size_t pos_colon = content.find(":", pos_key);
  if (pos_colon == std::string::npos) return "";

  size_t first = content.find("\"", pos_colon);
  if (first == std::string::npos) return "";

  size_t second = content.find("\"", first + 1);
  if (second == std::string::npos) return "";

  return content.substr(first + 1, second - first - 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  std::ifstream file("config.json");
  if (!file.is_open())
  {
    return -1;
  }

  std::stringstream ss;
  ss << file.rdbuf();
  std::string buf = ss.str();
  file.close();

  std::string BEARER_TOKEN = extract_value(buf, "TWITTER_BEARER_TOKEN");
  if (BEARER_TOKEN.empty())
  {
    return -1;
  }

  get_twitter("from:watch_econ -is:retweet", BEARER_TOKEN);
  return 0;
}
