#include <google/protobuf/compiler/plugin.h>
#include "generator.hh"

using google::protobuf::compiler::PluginMain;

int main(int argc, char* argv[]) {
  Generator generator;
  PluginMain(argc, argv, &generator);
  return 0;
}
