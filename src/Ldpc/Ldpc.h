/*******************************************************************************
 Copyright (c) 2015, Etienne Pierre-Doray, INRS
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 Declaration of Ldpc class
 ******************************************************************************/

#ifndef LDPC_H
#define LDPC_H

#include <thread>
#include <random>
#include <chrono>
#include <algorithm>

#include <boost/serialization/export.hpp>

#include "../Code.h"
#include "../Structure/BitMatrix.h"

namespace fec {

/**
 *  This class represents an ldpc encode / decoder.
 *  It offers methods encode and to decode data given an Structure.
 *
 *  The structure of the parity bits generated by an Ldpc object is as follow.
 *
 *    | syst | parity |
 *
 *  where syst are the systematic msg bits and parity are the added parity required
 *  to create a consistent bloc.
 *
 *  The structure of the extrinsic information is the case
 *
 *    | check1 | check2 | ... |
 *
 *  where checkX are the messages at the last iteration from the check node X 
 *  that would be transmitted to the connected bit nodes at the next iteration.
 */
class Ldpc : public Code
{
  friend class boost::serialization::access;
public:
  static SparseBitMatrix gallagerConstruction(size_t n, size_t wc, size_t wr);
  
  class Structure;
  class EncoderOptions
  {
    friend class Structure;
  public:
    EncoderOptions(const SparseBitMatrix& H) {H_ = H;}
    
  private:
    SparseBitMatrix H_;
  };
  
  class DecoderOptions {
    friend class Structure;
  public:
    DecoderOptions() = default;
    
    DecoderOptions& decoderType(Code::DecoderType type) {decoderType_ = type; return *this;}
    DecoderOptions& iterations(size_t n) {iterationCount_ = n; return *this;}
    
  private:
    Code::DecoderType decoderType_ = Approximate;
    size_t iterationCount_;
  };
  /**
   *  This class represents a ldpc code structure.
   *  It provides a usefull interface to store and acces the structure information.
   */
  class Structure : public Code::Structure {
    friend class ::boost::serialization::access;
  public:
    Structure() = default;
    Structure(const EncoderOptions&, const DecoderOptions&);
    virtual ~Structure() = default;
    
    virtual const char * get_key() const;
    
    virtual Code::Structure::Type type() const {return Code::Structure::Ldpc;}
    
    inline const SparseBitMatrix& checks() const {return H_;}
    
    inline size_t iterationCount() const {return iterationCount_;}
    
    void syndrome(std::vector<uint8_t>::const_iterator parity, std::vector<uint8_t>::iterator syndrome) const;
    virtual bool check(std::vector<BitField<uint8_t>>::const_iterator parity) const;
    virtual void encode(std::vector<BitField<bool>>::const_iterator msg, std::vector<BitField<uint8_t>>::iterator parity) const;
    
  private:
    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version) {
      using namespace boost::serialization;
      ar & ::BOOST_SERIALIZATION_BASE_OBJECT_NVP(Code::Structure);
      ar & ::BOOST_SERIALIZATION_NVP(H_);
      ar & ::BOOST_SERIALIZATION_NVP(DC_);
      ar & ::BOOST_SERIALIZATION_NVP(T_);
      ar & ::BOOST_SERIALIZATION_NVP(A_);
      ar & ::BOOST_SERIALIZATION_NVP(B_);
    }
    
    void computeGeneratorMatrix(SparseBitMatrix&& H);
    
    SparseBitMatrix H_;
    SparseBitMatrix DC_;
    SparseBitMatrix T_;
    SparseBitMatrix A_;
    SparseBitMatrix B_;
    
    size_t iterationCount_;
  };
  
  
  Ldpc(const Structure& structure, int workGroupdSize = 4);
  virtual ~Ldpc() = default;
  
  virtual const char * get_key() const;
  
  inline const Structure& structure() const {return structure_;}
  
protected:
  Ldpc() = default;
  
  virtual void decodeBlocks(std::vector<LlrType>::const_iterator parity, std::vector<BitField<bool>>::iterator msg, size_t n) const;
  virtual void soDecodeBlocks(InputIterator input, OutputIterator output, size_t n) const;
  
private:
  template <typename Archive>
  void serialize(Archive & ar, const unsigned int version) {
    using namespace boost::serialization;
    ar & ::BOOST_SERIALIZATION_NVP(structure_);
    ar.template register_type<Structure>();
    ar & ::BOOST_SERIALIZATION_BASE_OBJECT_NVP(Code);
  }
  
  Structure structure_;
};
  
}

BOOST_CLASS_EXPORT_KEY(fec::Ldpc);
BOOST_CLASS_TYPE_INFO(fec::Ldpc,extended_type_info_no_rtti<fec::Ldpc>);
BOOST_CLASS_EXPORT_KEY(fec::Ldpc::Structure);
BOOST_CLASS_TYPE_INFO(fec::Ldpc::Structure,extended_type_info_no_rtti<fec::Ldpc::Structure>);

#endif
