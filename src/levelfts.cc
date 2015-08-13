#if 0
gcc -Wall -o levelfts -std=c++11 -lstdc++ -lleveldb levelfts.cc; exit
#endif

#include <cassert>
#include <iostream>
#include <leveldb/db.h>

int main(int argc, char **argv) {
  leveldb::DB *db = nullptr;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "ftindex", &db);
  assert(status.ok());
  delete db;
  return 0;
}
