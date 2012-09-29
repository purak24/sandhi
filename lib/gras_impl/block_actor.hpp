//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP
#define INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP


#include <gnuradio/gras.hpp>
#include <gnuradio/block.hpp>
#include <Apology/Worker.hpp>
#include <gras_impl/debug.hpp>
#include <gras_impl/token.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/output_buffer_queues.hpp>
#include <gras_impl/input_buffer_queues.hpp>
#include <gras_impl/interruptible_thread.hpp>
#include <vector>
#include <set>

namespace gnuradio
{

static GRAS_FORCE_INLINE unsigned long myulround(const double x)
{
    return (unsigned long)(x + 0.5);
}

static GRAS_FORCE_INLINE unsigned long long myullround(const double x)
{
    return (unsigned long long)(x + 0.5);
}

struct BlockActor : Apology::Worker
{
    BlockActor(void);
    ~BlockActor(void);

    Block *block_ptr;

    //per port properties
    std::vector<size_t> input_items_sizes;
    std::vector<size_t> output_items_sizes;
    std::vector<size_t> input_history_items;
    std::vector<size_t> output_multiple_items;
    std::vector<size_t> input_multiple_items;
    std::vector<bool> input_inline_enables;

    //keeps track of production
    std::vector<uint64_t> items_consumed;
    std::vector<uint64_t> items_produced;

    //work buffers for the classic interface
    size_t work_noutput_items;
    std::vector<const void *> work_input_items;
    std::vector<void *> work_output_items;
    std::vector<int> work_ninput_items;
    std::vector<int> fcast_ninput_items;

    //work buffers for the new work interface
    Block::InputItems input_items;
    Block::OutputItems output_items;
    ptrdiff_t work_io_ptr_mask;

    //track work's calls to produce and consume
    std::vector<size_t> produce_items;
    std::vector<size_t> consume_items;
    std::vector<bool> consume_called;

    //track the subscriber counts
    std::vector<Token> input_tokens;
    std::vector<Token> output_tokens;
    std::set<Token> token_pool;

    std::vector<SBufferToken> output_buffer_tokens;

    //buffer queues and ready conditions
    InputBufferQueues input_queues;
    OutputBufferQueues<SBuffer> output_queues;

    //tag tracking
    std::vector<bool> input_tags_changed;
    std::vector<std::vector<Tag> > input_tags;
    Block::tag_propagation_policy_t tag_prop_policy;

    //interruptible thread stuff
    boost::shared_ptr<InterruptibleThread> interruptible_thread;

    //handlers
    /*
    void handle_input_msg(const tsbe::TaskInterface &, const size_t, const tsbe::Wax &);
    void handle_output_msg(const tsbe::TaskInterface &, const size_t, const tsbe::Wax &);
    void topology_update(const tsbe::TaskInterface &);
    void handle_block_msg(const tsbe::TaskInterface &, const tsbe::Wax &);
    void handle_allocation(const tsbe::TaskInterface &);
    void handle_task(const tsbe::TaskInterface &);
    void mark_done(const tsbe::TaskInterface &);
    void conclusion(const tsbe::TaskInterface &task_iface, const bool);
    void buffer_returner(const size_t index, SBuffer &buffer);
    void input_update(const tsbe::TaskInterface &task_iface);
    void sort_tags(const size_t index);
    void trim_tags(const tsbe::TaskInterface &, const size_t index);
    * */

    //work helpers
    int work_ret;
    inline void task_work(void)
    {
        this->work_ret = block_ptr->Work(this->input_items, this->output_items);
    }

    //is the fg running?
    enum
    {
        BLOCK_STATE_INIT,
        BLOCK_STATE_LIVE,
        BLOCK_STATE_DONE,
    } block_state;
    size_t hint; //some kind of allocation hint
    Affinity buffer_affinity;

    std::vector<std::vector<BufferHintMessage> > output_allocation_hints;

    //rate settings
    bool enable_fixed_rate;
    double relative_rate;
    bool forecast_fail;
    bool forecast_enable;
    bool topology_init;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP*/