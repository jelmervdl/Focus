#ifndef __NOT_SUPPORTED_ERROR__
#define __NOT_SUPPORTED_ERROR__

#include <stdexcept>
#include <string>

class NotSupportedError : public std::runtime_error {
 public:
   NotSupportedError(std::string const &message = "") : std::runtime_error("function not supported: " + message) { }
};

class SimpleException : public std::runtime_error {
 public:
   SimpleException(std::string const &message = "") : std::runtime_error("function not supported: " + message) { }
};

#endif
