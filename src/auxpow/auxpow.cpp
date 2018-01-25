// Copyright (c) 2011 Vince Durham
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.


#include "init.h"
#include "primitives/block.h"
#include "base58.h"

/** Global dirty block merged mining entries. */
std::map<uint256, std::shared_ptr<CAuxPow> > mapDirtyAuxPow;

uint256 CAuxPow::CheckMerkleBranch(const uint256& hash, const std::vector<uint256>& vMerkleBranch, int nIndex) const
{
  uint256 thash = hash;
  if (nIndex == -1)
    return {};
  for (std::vector<uint256>::const_iterator it(vMerkleBranch.begin()); it != vMerkleBranch.end(); ++it)
  {
    if (nIndex & 1)
      thash = Hash(BEGIN(*it), END(*it), BEGIN(thash), END(thash));
    else
      thash = Hash(BEGIN(thash), END(thash), BEGIN(*it), END(*it));
    nIndex >>= 1;
  }
  return thash;
}


void CBlockHeader::SetAuxPow(CAuxPow* pow)
{
    if (pow != nullptr)
        nVersion |= AuxPow::BLOCK_VERSION_AUXPOW;
    else
        nVersion &= ~AuxPow::BLOCK_VERSION_AUXPOW;
    auxpow.reset(pow);
}
