#if 0
gcc -Wall -o levelfts -std=c++11 -lstdc++ -lleveldb levelfts.cc; exit
#endif

#include <cassert>
#include <iostream>
#include <fstream>
#include <sysexits.h>
#include <leveldb/db.h>

#define PROGRAM_NAME "levelfts"

void abort_with_usage() {
  std::cerr << "Usage: " PROGRAM_NAME " index <textpath> <indexpath>" << std::endl;
  std::cerr << "       " PROGRAM_NAME " search <keyword> <indexpath>" << std::endl;
  exit(EX_USAGE);
}

int create_ii(const char *textpath, const char *iipath) {
  leveldb::DB *ii = nullptr;
  leveldb::Options opening_options;
  opening_options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(opening_options, iipath, &ii);
  assert(status.ok());

  std::ifstream textfile(textpath);
  char token[3] = {(char)-1, 0};
  char position = 0;
  leveldb::WriteOptions writing_options;
  while (true) {
    token[1] = textfile.get();
    if (!textfile.good()) {
      token[1] = (char)-1;
    }
    printf("%s: %d\n", token, (int) position);
    leveldb::Status putting_status = ii->Put(writing_options, token, &position);
    if (!textfile.good()) {
      break;
    }
    token[0] = token[1];
    ++position;
  }
  delete ii;
  return EX_OK;
}

int search_with_ii(const std::string &keyword, const char *iipath) {
  leveldb::DB *ii = nullptr;
  leveldb::Options options;
  options.create_if_missing = false;
  leveldb::Status status = leveldb::DB::Open(options, iipath, &ii);
  assert(status.ok());

  char token[3] = {0};
  char prev_position = -1;
  leveldb::ReadOptions getting_options;
  bool matched = true;
  for (auto keyword_iter = keyword.cbegin(); keyword_iter != keyword.cend(); ++keyword_iter) {
    token[1] = *keyword_iter;
    if (token[0] != 0) {
      std::string getting_buffer;
      leveldb::Status getting_status = ii->Get(getting_options, token, &getting_buffer);
      char position = getting_buffer.front();
      printf("%s: %d\n", token, (int) position);
      if ((prev_position >= 0) && (position != prev_position + 1)) {
        matched = false;
        break;
      }
      prev_position = position;
    }
    token[0] = token[1];
  }
  std::cout << (matched ? "matched" : "not matched") << std::endl;
  delete ii;
  return EX_OK;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    abort_with_usage();
  } 
  if (strcmp("index", argv[1]) == 0) {
    return create_ii(argv[2], argv[3]);
  } else if (strcmp("search", argv[1]) == 0) {
    return search_with_ii(argv[2], argv[3]);
  } else {
    std::cerr << "Illegal command name" << std::endl;
    abort_with_usage();
  }
}
