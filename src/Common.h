#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <map>
#include <vector>
#include <set>
#include <memory>

using std::string;
using std::map;
using std::vector;
using std::set;
using std::pair;
using std::make_pair;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;


template <typename T>
inline bool str2int(const char *raw, T &value) {
  std::istringstream ss(raw);
  ss >> value;
  return !(ss.fail() || ss.bad());
}
template <typename T>
inline bool str2int(const string &raw, T &value) {
  std::istringstream ss(raw);
  ss >> value;
  return !(ss.fail() || ss.bad());
}

inline vector<string> str2vstr (const string &raw, const string delimiter = ",") {
  string str = raw;
  vector<string> data;
  size_t pos;
  while (true) {
    pos = str.find(delimiter);
    if (pos == std::string::npos) {
      data.push_back(str);
      break;
    } else {
      data.push_back(str.substr(0, pos));
      str.erase(0, pos + delimiter.length());
    }
  }
  return data;
}

template<typename T>
inline string stringify(const T &value) {
  return static_cast<std::ostringstream*>( &(std::ostringstream() << value) )->str();
}

inline string stringify(const string &value) {
  return value;
}

template<typename T>
inline bool str2T(const string &raw, T &value) {
  std::istringstream ss(raw);
  ss >> value;
  return !(ss.fail() || ss.bad());
}

inline bool str2T(const string &raw, string &value) {
  value = raw;
  return true;
}

template <typename T>
inline bool str2vint (const string &raw, vector<T> &data, const string delimiter = ",") {
  string str = raw;
  size_t pos;
  while (true) {
    pos = str.find(delimiter);
    if (pos == std::string::npos) {
      T v;
      if (str2int(str, v)) {
        data.push_back(v);
      } else {
        return false;
      }
      break;
    } else {
      T v;
      if (str2int(str.substr(0, pos), v)) {
        data.push_back(v);
      } else {
        return false;
      }

      str.erase(0, pos + delimiter.length());
    }
  }
  return true;
}


#endif
