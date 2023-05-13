#ifndef PYGF_SERVER_HPP
#define PYGF_SERVER_HPP

#include "gfServer.hpp"
using namespace cppbinding;

namespace ecppbinding {

int pystartServer(std::string &hostName, std::string &portName, int fileCount, std::vector<std::string> &seqFiles,
                  gfServerOption &options, UsageStats &stats, Signal &signal);

}

#endif  // !#ifndef PYGF_SERVER_HPP
