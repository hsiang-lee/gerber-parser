#pragma once

class Parser {
 public:
  virtual ~Parser(){};
  virtual bool Run() = 0;
  virtual bool EndOfFile() = 0;
};
