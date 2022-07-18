#ifndef READ_HH_
#define READ_HH_

#include <string>
#include <vector>

std::string ssl_read(const std::string& host, const std::string& port_num, const std::string& buf, std::vector<std::string>& header);
std::string get_response(const std::string& host, const std::string& port_num, const std::string& api);

#endif
