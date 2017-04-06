#ifndef TRAINBOOM_EXCEPTIONS_HPP
#define TRAINBOOM_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace util {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const std::string& _variant, const std::string& _detail):
		variant(_variant), detail(_detail) {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception {
public:
	index_out_of_bound() : exception(
		"index_out_of_bound",
		"Your index is out of bound!!!") {}
};

class runtime_error : public exception {
public:
	runtime_error() : exception(
		"runtime_error",
		"Runtime Error!!!") {}
};

class invalid_iterator : public exception {
public:
	invalid_iterator() : exception(
		"invalid_iterator",
		"Your iterator is invalid!!!") {}
};

class container_is_empty : public exception {
public:
	container_is_empty() : exception(
		"container_is_empty",
		"Your container is empty!!!") {}
};
}

#endif