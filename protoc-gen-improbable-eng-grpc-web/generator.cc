#include <string>
#include <map>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/stubs/strutil.h>

#include "generator-options.hh"
#include "generator.hh"

using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::EnumDescriptor;
using google::protobuf::FileDescriptor;
using google::protobuf::MethodDescriptor;
using google::protobuf::ServiceDescriptor;
using google::protobuf::compiler::CodeGenerator;
using google::protobuf::compiler::GeneratorContext;
using google::protobuf::compiler::ParseGeneratorParameter;
using google::protobuf::io::Printer;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::ReplaceCharacters;
using google::protobuf::StripSuffixString;
using google::protobuf::StripPrefixString;
using google::protobuf::StringPiece;

namespace {

  std::string firstCharToLower(std::string str) {
    if(str.length() > 0) {
      str[0] = std::tolower(str[0]);
    }

    return str;
  }

  std::string removePathExtname(const std::string& path) {
    auto dotIndex = path.find_last_of('.');

    if(dotIndex != std::string::npos) {
      return path.substr(0, dotIndex);
    }

    return path;
  }

  //////////////////////////////////////////////////////////////////////////////
  // START of stolen methods from the official protoc js_generator
  // @SEE: https://github.com/protocolbuffers/protobuf/blob/54288a01cebfd0bfa62ca581dd07ffd6f9c77f2c/src/google/protobuf/compiler/js/js_generator.cc

  bool StrEndsWith(StringPiece sp, StringPiece x) {
    return sp.size() >= x.size() && sp.substr(sp.size() - x.size()) == x;
  }

  std::string StripProto
    ( const std::string& filename
    )
  {
    const char* suffix =
      StrEndsWith(filename, ".protodevel") ? ".protodevel" : ".proto";
    return StripSuffixString(filename, suffix);
  }

  std::string ModuleAlias(const std::string& filename) {
    std::string basename = StripProto(filename);
    ReplaceCharacters(&basename, "-", '$');
    ReplaceCharacters(&basename, "/", '_');
    ReplaceCharacters(&basename, ".", '_');
    return basename + "_pb";
  }

  std::string GetRootPath
    ( const std::string& from_filename
    , const std::string& to_filename
    )
  {
    // if (to_filename.find("google/protobuf") == 0) {
      // Well-known types (.proto files in the google/protobuf directory) are
      // assumed to come from the 'google-protobuf' npm package.  We may want to
      // generalize this exception later by letting others put generated code in
      // their own npm packages.
      // return "google-protobuf/";
    // }

    size_t slashes = std::count(
      from_filename.begin(),
      from_filename.end(), '/'
    );

    if (slashes == 0) {
      return "./";
    }
    std::string result = "";
    for (size_t i = 0; i < slashes; i++) {
      result += "../";
    }
    return result;
  }

  template<class DescriptorT>
  std::string GetNestedMessageName(const DescriptorT* descriptor) {
    if (descriptor == NULL) {
      return "";
    }
    std::string result = StripPrefixString(
        descriptor->full_name(), descriptor->file()->package());
    // Add a leading dot if one is not already present.
    if (!result.empty() && result[0] != '.') {
      result = "." + result;
    }
    return result;
  }

  std::string MaybeCrossFileRef
    ( const GeneratorOptions& options
    , const FileDescriptor* from_file
    , const Descriptor* to_message
    )
  {
    return ModuleAlias(to_message->file()->name()) +
          GetNestedMessageName(to_message->containing_type()) + "." +
          to_message->name();
  }

  // std::string MaybeCrossFileRef
  //   ( const GeneratorOptions& options
  //   , const FileDescriptor* from_file
  //   , const EnumDescriptor* to_enum
  //   )
  // {
  //   return ModuleAlias(
  //     to_enum->file()->name()) +
  //       GetNestedMessageName(to_enum->containing_type()) + "." +
  //         to_enum->name();
  // }

  // END of stolen methods from the official protoc js_generator
  //////////////////////////////////////////////////////////////////////////////

  std::string GetTsFilename
    ( const GeneratorOptions& options
    , const std::string& filename
    )
  {
    return StripProto(filename) + "_pb.ts";
  }
}

Generator::Generator() {

}

Generator::~Generator() {

}

void Generator::GenerateFileImports
  ( const GeneratorOptions&  options
  , Printer&                 printer
  , const FileDescriptor*    file
  ) const
{
  printer.Print("import {grpc} from '@improbable-eng/grpc-web';\n\n");

  std::vector<const FileDescriptor*> importFiles;
  importFiles.push_back(file);

  for(auto i=0; file->dependency_count() > i; ++i) {
    importFiles.push_back(file->dependency(i));
  }

  for(auto importFile : importFiles) {
    const std::string& name = importFile->name();

    std::string importPath =
      GetRootPath(file->name(), name) + GetTsFilename(options, name);
    importPath = importPath.substr(0, importPath.size() - 3);

    printer.Print(
      "import * as $alias$ from '$file$';\n",
      "alias", ModuleAlias(name), "file", importPath);
  }
}

void Generator::GenerateServiceDefinitions
  ( const GeneratorOptions&  options
  , Printer&                 printer
  , const FileDescriptor*    file
  ) const
{
  for(auto i=0; file->service_count() > i; ++i) {
    auto service = file->service(i);

    std::map<std::string, std::string> vars;
    vars["ServiceName"] = service->name();
    vars["FullServiceName"] = service->full_name();

    printer.Print("\n");
    printer.Print(vars, "export class $ServiceName$ {\n");
    printer.Indent();

    printer.Print(vars,
      "static readonly serviceName = \"$FullServiceName$\";\n\n");

    for(auto n=0; service->method_count() > n; ++n) {
      auto method = service->method(n);

      vars["methodName"] = firstCharToLower(method->name());
      vars["MethodName"] = method->name();
      vars["methodRequestStream"] = method->client_streaming() ?
        "true" : "false";
      vars["methodResponseStream"] = method->server_streaming() ?
        "true" : "false";
      vars["methodRequestType"] = MaybeCrossFileRef(
        options, file,
        method->input_type()
      );
      vars["methodResponseType"] = MaybeCrossFileRef(
        options, file,
        method->output_type()
      );

      printer.Print(vars,
        "static readonly $MethodName$ = {\n"
        "  methodName: \"$MethodName$\",\n"
        "  service: $ServiceName$,\n"
        "  requestStream: $methodRequestStream$,\n"
        "  responseStream: $methodResponseStream$,\n"
        "  requestType: $methodRequestType$,\n"
        "  responseType: $methodResponseType$,\n"
        "};\n\n"
      );
    }

    printer.Outdent();
    printer.Print("}\n");
  }
}

void Generator::GenerateServiceClients
  ( const GeneratorOptions&  options
  , Printer&                 printer
  , const FileDescriptor*    file
  ) const
{
  for(auto i=0; file->service_count() > i; ++i) {
    auto service = file->service(i);
    std::map<std::string, std::string> vars;
    vars["ServiceName"] = service->name();

    printer.Print("\n");
    printer.Print(vars, "export class $ServiceName$Client {\n");
    printer.Indent();

    for(auto n=0; service->method_count() > n; ++n) {
      auto method = service->method(n);
      vars["methodName"] = firstCharToLower(method->name());
      printer.Print(vars, "$methodName$() {\n");
      printer.Indent();

      // @TODO: Implement method bodies
      printer.Print(vars, "throw new Error('minga_protoc_gen_improbable_eng_grpc_web client methods are unimplemented');\n");

      printer.Outdent();
      printer.Print(vars, "}\n");
    }

    printer.Outdent();
    printer.Print(vars, "}\n");
  }
}

bool Generator::Generate
  ( const FileDescriptor*  file
  , const std::string&     parameter
  , GeneratorContext*      context
  , std::string*           error
  ) const
{

  GeneratorOptions options(parameter);

  std::string filename = removePathExtname(file->name()) + "_pb_service.ts";
  Printer printer(context->Open(filename), '$');

  printer.Print("// GENERATED CODE -- DO NOT EDIT!\n\n");

  GenerateFileImports(options, printer, file);
  GenerateServiceDefinitions(options, printer, file);
  GenerateServiceClients(options, printer, file);

  return true;
}
