/*******************************************************************************
 This file is part of FeCl.
 
 Copyright (c) 2015, Etienne Pierre-Doray, INRS
 Copyright (c) 2015, Leszek Szczecinski, INRS
 All rights reserved.
 
 FeCl is free software: you can redistribute it and/or modify
 it under the terms of the Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 FeCl is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the Lesser General Public License
 along with FeCl.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef FEC_CONVOLUTIONAL_H
#define FEC_CONVOLUTIONAL_H

#include "../Codec.h"
#include "../Structure/Trellis.h"
#include "../Structure/Permutation.h"

namespace fec {
  
  /**
   *  This class represents a convolutional encode / decoder.
   *  It offers methods encode and to decode data given a Structure.
   *
   *  The structure of the parity bits generated by a ConvolutionalCodec object is as follows
   *
   *    | parity1 | parity2 | ... | tail1 | tail2 ... |
   *
   *  where parityX is the output symbol sequence of the branch used at stage X
   *  in the trellis, tailX is are the output symbol sequence of the tail branch
   *  at stage X.
   *
   *  The structure of the extrinsic information
   *
   *    | msg | systTail |
   *
   *  where msg are the extrinsic msg L-values generated by the map decoder
   *  and systTail are the tail bit added to the message for trellis termination.
   */
  class Convolutional : public Codec
  {
    friend class boost::serialization::access;
  public:
    
    /**
     *  Trellis termination types.
     *  This specifies the type of termination at the end of each bloc.
     */
    enum Termination {
      Tail, /**< The state is brought to zero by implicitly adding new msg bit */
      Truncate /**< The state is forced to zero by truncating the trellis */
    };
    
    struct EncoderOptions {
    public:
      EncoderOptions(Trellis trellis, size_t length) {trellis_ = trellis; length_ = length;}
      EncoderOptions& termination(Termination type) {termination_ = type; return *this;}
      
      Trellis trellis_;
      size_t length_;
      Termination termination_ = Truncate;
    };
    struct DecoderOptions {
    public:
      DecoderOptions() = default;
      
      DecoderOptions& algorithm(Codec::DecoderAlgorithm algorithm) {algorithm_ = algorithm; return *this;}
      
      Codec::DecoderAlgorithm algorithm_ = Approximate;
    };
    struct PunctureOptions {
    public:
      PunctureOptions(std::vector<bool> mask = {}) {mask_ = mask;};
      
      PunctureOptions& tailMask(std::vector<bool> mask) {tailMask_ = mask; return *this;}
      
      std::vector<bool> mask_;
      std::vector<bool> tailMask_;
    };
    /**
     *  This class represents a convolutional code structure.
     *  It provides a usefull interface to store and acces the structure information.
     */
    class Structure : public Codec::Structure {
      friend class ::boost::serialization::access;
    public:
      Structure() = default;
      Structure(const EncoderOptions& encoder, const DecoderOptions& decoder);
      Structure(const EncoderOptions& encoder);
      virtual ~Structure() = default;
      
      virtual const char * get_key() const;
      virtual Codec::Structure::Type type() const {return Codec::Structure::Convolutional;}
      
      void setDecoderOptions(const DecoderOptions& decoder);
      void setEncoderOptions(const EncoderOptions& encoder);
      DecoderOptions getDecoderOptions() const;
      
      inline size_t length() const {return length_;}
      inline size_t tailSize() const {return tailSize_;}
      inline size_t systTailSize() const {return tailSize_ * trellis().inputSize();}
      inline Termination termination() const {return termination_;}
      inline const Trellis& trellis() const {return trellis_;}
      
      virtual bool check(std::vector<BitField<size_t>>::const_iterator parity) const;
      virtual void encode(std::vector<BitField<size_t>>::const_iterator msg, std::vector<BitField<size_t>>::iterator parity) const;
      void encode(std::vector<BitField<size_t>>::const_iterator msg, std::vector<BitField<size_t>>::iterator parity, std::vector<BitField<size_t>>::iterator tail) const;
      
      Permutation createPermutation(const PunctureOptions& options) const;
      
    private:
      template <typename Archive>
      void serialize(Archive & ar, const unsigned int version) {
        using namespace boost::serialization;
        ar & ::BOOST_SERIALIZATION_BASE_OBJECT_NVP(Codec::Structure);
        ar & ::BOOST_SERIALIZATION_NVP(trellis_);
        ar & ::BOOST_SERIALIZATION_NVP(termination_);
        ar & ::BOOST_SERIALIZATION_NVP(tailSize_);
        ar & ::BOOST_SERIALIZATION_NVP(length_);
      }
      
      Trellis trellis_;
      size_t length_;
      Termination termination_;
      size_t tailSize_;
    };
    
    Convolutional() = default;
    Convolutional(const Structure& structure, int workGroupSize = 8);
    Convolutional(const EncoderOptions& encoder, const DecoderOptions& decoder, int workGroupSize = 8);
    Convolutional(const EncoderOptions& encoder, int workGroupSize = 8);
    Convolutional(const Convolutional& other) : Codec(&structure_) {*this = other;}
    virtual ~Convolutional() = default;
    
    virtual const char * get_key() const;
    
    inline const Structure& structure() const {return structure_;}
    void setDecoderOptions(const DecoderOptions& decoder) {structure_.setDecoderOptions(decoder);}
    void setEncoderOptions(const EncoderOptions& encoder) {structure_.setEncoderOptions(encoder);}
    DecoderOptions getDecoderOptions() const {return structure_.getDecoderOptions();}
    
    Permutation createPermutation(const PunctureOptions& options) {return structure_.createPermutation(options);}
    
  protected:
    virtual void decodeBlocks(std::vector<LlrType>::const_iterator parity, std::vector<BitField<size_t>>::iterator msg, size_t n) const;
    virtual void soDecodeBlocks(InputIterator input, OutputIterator output, size_t n) const;
    
  private:
    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version) {
      using namespace boost::serialization;
      ar & ::BOOST_SERIALIZATION_NVP(structure_);
      ar.template register_type<Structure>();
      ar & ::BOOST_SERIALIZATION_BASE_OBJECT_NVP(Codec);
    }
    
    Structure structure_;
  };
  
}

BOOST_CLASS_TYPE_INFO(fec::Convolutional,extended_type_info_no_rtti<fec::Convolutional>);
BOOST_CLASS_EXPORT_KEY(fec::Convolutional);
BOOST_CLASS_TYPE_INFO(fec::Convolutional::Structure,extended_type_info_no_rtti<fec::Convolutional::Structure>);
BOOST_CLASS_EXPORT_KEY(fec::Convolutional::Structure);

#endif
