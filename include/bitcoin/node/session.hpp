/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-node.
 *
 * libbitcoin-node is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_NODE_SESSION_HPP
#define LIBBITCOIN_NODE_SESSION_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <set>
#include <system_error>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/poller.hpp>
#include <bitcoin/node/responder.hpp>

namespace libbitcoin {
namespace node {

class BCN_API session
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;

    session(threadpool& pool, network::handshake& handshake,
        network::protocol& protocol, chain::blockchain& blockchain,
        poller& poller, chain::transaction_pool& transaction_pool,
        responder& responder, size_t minimum_start_height=0);

    void start(completion_handler handle_complete);
    void stop(completion_handler handle_complete);

private:
    void subscribe(const std::error_code& ec,
        completion_handler handle_complete);
    void new_channel(const std::error_code& ec,
        network::channel_ptr node);
    void broadcast_new_blocks(const std::error_code& ec, uint32_t fork_point,
        const chain::blockchain::block_list& new_blocks,
        const chain::blockchain::block_list& replaced_blocks);

    bool receive_inv(const std::error_code& ec,
        const inventory_type& packet, network::channel_ptr node);
    bool receive_get_blocks(const std::error_code& ec,
        const get_blocks_type& packet, network::channel_ptr node);

    void new_tx_inventory(const hash_digest& tx_hash,
        network::channel_ptr node);
    void request_tx_data(const std::error_code& ec, bool tx_exists,
        const hash_digest& tx_hash, network::channel_ptr node);

    void new_block_inventory(const hash_digest& block_hash,
        network::channel_ptr node);
    void request_block_data(const hash_digest& block_hash,
        network::channel_ptr node);
    void fetch_block_handler(const std::error_code& ec,
        const block_type& block, const hash_digest block_hash,
        network::channel_ptr node);

    sequencer strand_;
    network::handshake& handshake_;
    network::protocol& protocol_;
    chain::blockchain& blockchain_;
    chain::transaction_pool& tx_pool_;
    node::poller& poller_;
    node::responder& responder_;
    std::atomic<uint64_t> last_height_;
    size_t minimum_start_height_;

    // HACK: this is for access to broadcast_new_blocks to facilitate server
    // inheritance of full_node. The organization should be refactored.
    friend class full_node;
};

} // namespace node
} // namespace libbitcoin

#endif

