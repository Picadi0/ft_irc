#pragma once

#include "style.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#if defined(__linux__)
#include <cstdlib>
#include <cstring>
#endif

using std::cout;
using std::endl;
using std::list;
using std::map;
using std::pair;
using std::runtime_error;
using std::string;
