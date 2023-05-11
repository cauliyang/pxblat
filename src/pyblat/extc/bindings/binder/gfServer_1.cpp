#include <gfServer.hpp>
#include <iterator>
#include <memory>
#include <string>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_gfServer_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// cppbinding::pystatusServer(std::string &, std::string &, struct cppbinding::gfServerOption &) file:gfServer.hpp line:229
	M("cppbinding").def("pystatusServer", (std::string (*)(std::string &, std::string &, struct cppbinding::gfServerOption &)) &cppbinding::pystatusServer, "C++: cppbinding::pystatusServer(std::string &, std::string &, struct cppbinding::gfServerOption &) --> std::string", pybind11::arg("hostName"), pybind11::arg("portName"), pybind11::arg("options"));

	// cppbinding::pygetFileList(std::string &, std::string &) file:gfServer.hpp line:230
	M("cppbinding").def("pygetFileList", (std::string (*)(std::string &, std::string &)) &cppbinding::pygetFileList, "C++: cppbinding::pygetFileList(std::string &, std::string &) --> std::string", pybind11::arg("hostName"), pybind11::arg("portName"));

	// cppbinding::pyqueryServer(std::string &, std::string &, std::string &, std::string &, bool, bool) file:gfServer.hpp line:231
	M("cppbinding").def("pyqueryServer", (std::string (*)(std::string &, std::string &, std::string &, std::string &, bool, bool)) &cppbinding::pyqueryServer, "C++: cppbinding::pyqueryServer(std::string &, std::string &, std::string &, std::string &, bool, bool) --> std::string", pybind11::arg("type"), pybind11::arg("hostName"), pybind11::arg("portName"), pybind11::arg("faName"), pybind11::arg("complex"), pybind11::arg("isProt"));

	// cppbinding::test_stdout() file:gfServer.hpp line:234
	M("cppbinding").def("test_stdout", (void (*)()) &cppbinding::test_stdout, "C++: cppbinding::test_stdout() --> void");

	// cppbinding::test_add(int &) file:gfServer.hpp line:235
	M("cppbinding").def("test_add", (void (*)(int &)) &cppbinding::test_add, "C++: cppbinding::test_add(int &) --> void", pybind11::arg("a"));

	// cppbinding::test_stat(struct cppbinding::UsageStats &) file:gfServer.hpp line:236
	M("cppbinding").def("test_stat", (void (*)(struct cppbinding::UsageStats &)) &cppbinding::test_stat, "C++: cppbinding::test_stat(struct cppbinding::UsageStats &) --> void", pybind11::arg("stats"));

}
