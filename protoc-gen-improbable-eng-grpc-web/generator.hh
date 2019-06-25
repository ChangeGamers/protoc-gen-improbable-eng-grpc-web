#pragma once

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/printer.h>

#include "generator-options.hh"

class Generator
  : public google::protobuf::compiler::CodeGenerator
{
public:

  Generator();
  ~Generator() override;

  void Generator::GenerateFileImports
    ( const GeneratorOptions&                  options
    , google::protobuf::io::Printer&           printer
    , const google::protobuf::FileDescriptor*  file
    ) const;

  void Generator::GenerateServiceDefinitions
    ( const GeneratorOptions&                  options
    , google::protobuf::io::Printer&           printer
    , const google::protobuf::FileDescriptor*  file
    ) const;

  void Generator::GenerateServiceClients
    ( const GeneratorOptions&                  options
    , google::protobuf::io::Printer&           printer
    , const google::protobuf::FileDescriptor*  file
    ) const;

  bool Generate
    ( const google::protobuf::FileDescriptor*        file
    , const std::string&                             parameter
    , google::protobuf::compiler::GeneratorContext*  context
    , std::string*                                   error
    ) const override;
};
