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

#include "TurboDecoder.h"
#include "TurboDecoderImpl.h"

using namespace fec;

std::unique_ptr<TurboDecoder> TurboDecoder::create(const Turbo::detail::Structure& structure)
{
  return std::unique_ptr<TurboDecoder>(new TurboDecoderImpl(structure));
}

TurboDecoder::TurboDecoder(const Turbo::detail::Structure& structure) : structure_(structure)
{
  for (size_t i = 0; i < this->structure().constituentCount(); ++i) {
    code_.push_back(MapDecoder::create(this->structure().constituent(i)));
  }
  extrinsic_.resize(this->structure().innerStateSize());
  extrinsicBuffer_.resize(this->structure().innerStateSize());;
  parityIn_.resize(this->structure().innerParitySize());
  parityOut_.resize(this->structure().innerParitySize());
}

void TurboDecoder::decodeBlocks(std::vector<LlrType>::const_iterator parity, std::vector<BitField<size_t>>::iterator msg, size_t n)
{
  for (size_t i = 0; i < n; ++i) {
    decodeBlock(parity, msg);
    parity += structure().paritySize();
    msg += structure().msgSize();
  }
}

void TurboDecoder::soDecodeBlocks(Codec::detail::InputIterator input, Codec::detail::OutputIterator output, size_t n)
{
  for (size_t i = 0; i < n; ++i) {
    soDecodeBlock(input, output);
    ++input;
    ++output;
  }
}