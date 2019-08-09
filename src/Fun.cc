#include "Fun.h"

void FunHandler::handle(const string& method, const string& params, const char* dataref) {
  bool methodValid = true;
  bool paramsValid = true;
  if (method == "help") {
    std::cout << "help message here" << std::endl;
  } else
    methodValid = false;

  if (!methodValid) {
    fprintf(stderr, "Error: Unsupported method %s\n", method.c_str());
    fprintf(stderr, "Valid methods:\n");
    for (auto &name : methods) {
      fprintf(stderr, "%s\n", name.c_str());
    }
  }
  if (methodValid && !paramsValid) {
    fprintf(stderr, "Error: Invalid params(%s) for method %s\n", params.c_str(), method.c_str());
  }
}
