/*
 * Copyright 2015 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef COMPILATIONUNIT_H
#define COMPILATIONUNIT_H

#include <string>
#include <istream>

class Compilationunit
{
public:
    Compilationunit(std::string file, std::istream *is = nullptr) : fileName(std::move(file)), istream(is) {}
    
    Compilationunit(const Compilationunit &o) = delete;
    Compilationunit(Compilationunit &&o) {
        fileName = std::move(o.fileName);
        istream = o.istream;
        ownsStream = o.ownsStream;
        
        o.istream = nullptr;
        o.ownsStream = false;
    };
    
    ~Compilationunit() {
        if (ownsStream)
            delete istream;
    }
    
    const std::string& getFilename() const { return fileName; }
    
    std::istream* getStream() const
    {
      if (!istream)
      {
          ownsStream = true;
          istream = new std::ifstream(fileName);
      }
      return istream;
    }
private:
    std::string fileName;
    mutable std::istream * istream;
    mutable bool ownsStream = false;
};

#endif // COMPILATIONUNIT_H
