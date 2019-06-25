#pragma once

#include <string>

class GeneratorOptions {
private:
  std::string error_;

public:
  GeneratorOptions
    ( const std::string& parameter
    );

  bool hasError
    ( std::string* error
    ) const;

};
