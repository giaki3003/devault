// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2019 The DeVault developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMS_H
#define BITCOIN_CHAINPARAMS_H

#include "chainparamsbase.h"
#include "consensus/params.h"
#include "primitives/block.h"
#include "protocol.h"

#include <memory>
#include <vector>

typedef std::map<int, uint256> MapCheckpoints;

struct CCheckpointData {
    MapCheckpoints mapCheckpoints;
};

struct ChainTxData {
    int64_t nTime;
    int64_t nTxCount;
    double dTxRate;
};

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Bitcoin system. There are three: the main network on which people trade goods
 * and services, the public test network which gets reset from time to time and
 * a regression test mode which is intended for private networks only. It has
 * minimal difficulty to ensure that blocks can be found instantly.
 */
class CChainParams {
public:
 
    const Consensus::Params &GetConsensus() const { return consensus; }
    const CMessageHeader::MessageMagic &DiskMagic() const { return diskMagic; }
    const CMessageHeader::MessageMagic &NetMagic() const { return netMagic; }
    int GetDefaultPort() const { return nDefaultPort; }

    const CBlock &GenesisBlock() const { return genesis; }
    /** Default value for -checkmempool and -checkblockindex argument */
    bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    /** Policy: Filter transactions that do not match well-defined patterns */
    bool RequireStandard() const { return fRequireStandard; }
    uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    /**
     * Make miner stop after a block is found. In RPC, don't return until
     * nGenProcLimit blocks are generated.
     */
    bool MineBlocksOnDemand() const { return fMineBlocksOnDemand; }
    /** Return the BIP70 network string (main, test or regtest) */
    std::string NetworkIDString() const { return strNetworkID; }
    /** Return the list of hostnames to look up for DNS seeds */
    const std::vector<std::string> &DNSSeeds() const { return vSeeds; }
    int ExtCoinType() const { return nExtCoinType; }
    const std::string &CashAddrPrefix() const { return cashaddrPrefix; }
    const std::string &CashAddrSecretPrefix() const { return cashaddrSecretPrefix; }
    const CCheckpointData &Checkpoints() const { return checkpointData; }
    const ChainTxData &TxData() const { return chainTxData; }

protected:
    CChainParams() = default;

    Consensus::Params consensus;
    CMessageHeader::MessageMagic diskMagic;
    CMessageHeader::MessageMagic netMagic;
    int nDefaultPort;
    uint64_t nPruneAfterHeight;
    std::vector<std::string> vSeeds;
    int nExtCoinType;
    std::string cashaddrPrefix;
    std::string cashaddrSecretPrefix;
    std::string strNetworkID;
    CBlock genesis;
    bool fDefaultConsistencyChecks;
    bool fRequireStandard;
    bool fMineBlocksOnDemand;
    CCheckpointData checkpointData;
    ChainTxData chainTxData;
};

/**
 * Creates and returns a std::unique_ptr<CChainParams> of the chosen chain.
 * @returns a CChainParams* of the chosen chain.
 * @throws a std::runtime_error if the chain is not supported.
 */
std::unique_ptr<CChainParams> CreateChainParams(const std::string &chain);

/**
 * Return the currently selected parameters. This won't change after app
 * startup, except for unit tests.
 */
const CChainParams &Params();

/**
 * Sets the params returned by Params() to those for the given BIP70 chain name.
 * @throws std::runtime_error when the chain is not supported.
 */
void SelectParams(const std::string &chain);

#endif // BITCOIN_CHAINPARAMS_H
