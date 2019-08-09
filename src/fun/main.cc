#include "Fun.h"

void usage() {
  std::cout << "bitcoin-fun [-h][-d datafile] method params(numeric, string, list, json)" << std::endl;
}

int main(int argc, char **argv) {
  char *dataref = NULL;
  string method = "";
  string params = "";
  int c;

  if (argc <= 1) {
    usage();
    return 1;
  }

  while ((c = getopt(argc, argv, "d:h")) != -1) {
    switch (c) {
      case 'd':
        dataref = optarg;
        break;
      case 'h': default:
        usage();
        exit(0);
    }
  }

  auto index = optind;
  if (index < argc) {
    method = string(argv[index]);
    index++;
  }
  if (index < argc) {
    params = string(argv[index]);
    index++;
  }
  if (index < argc) {
    fprintf(stderr, "Error: Unknown additional arguements:%s \n", argv[index]);
    return 1;
  }

  auto handler = new FunHandler();
  handler->handle(method, params, dataref);
  delete handler;
}

