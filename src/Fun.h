#ifndef FUN_H
#define FUN_H

#include "Common.h"

class FunHandler {
public:
  const vector<string> methods = {
    "help"
  };
  void handle(const string& method, const string& params, const char* dataref);
};

#endif
