#include <bcm2835_timer.h>
#include <bcm2835.h>
#include <chipset.h>
#include <priority_queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ScheduledTimer {
    uint64_t scheduled_count;
    TimerHandle handle;
    TimerCallback callback;
    uint32_t periodic_delay;
} ScheduledTimer;

typedef union LittleEndianUint64 {
    uint64_t dword;
    struct {
        uint32_t low_word;
        uint32_t high_word;
    };
} LittleEndianUint64;

// Internal implementation functions
static uint32_t get_frequency();
static void mask_and_enable_timer();
static uint64_t get_phy_count();
static void set_phy_timer_cmp_val(uint32_t);
static ScheduledTimer * get_prq_node_data(prq_node *);
static TimerHandle schedule_timer(TimerCallback, uint32_t, bool);

static prq_handle * scheduled_timers;

/*
 * Gives the frequency of the counter in Hz
 */
static inline uint32_t get_frequency() {
    return SYSTEM_TIMER_FREQ;
}


static inline void mask_and_enable_timer() {
    // Set compare value to 0
    bcm2835_timer_registers_base->Compare1 = 0;
    // Disable CMP1 interrupt
    bcm2835_int_registers_base->DisableIRQ1 |= SysTimerCMP1;
}

static inline uint64_t get_phy_count() {
    return bcm2835_timer_registers_base->CounterLower;
}



static inline void set_phy_timer_cmp_val(uint32_t val) {
  bcm2835_timer_registers_base->Compare1=val;
}

static inline ScheduledTimer * get_prq_node_data(prq_node * node) {
    return (ScheduledTimer *)node->data;
}

void bcm2835_timer_init() {
    const uint32_t freq = SYSTEM_TIMER_FREQ;
    INFO("System counter frequency: %u kHz\n", freq / 1000);


    bcm2835_int_registers_base->DisableIRQ1 |= SysTimerCMP1;


    // Init priority queue
    scheduled_timers = prq_create();


    bcm2835_int_registers_base->EnableIRQ1 |= SysTimerCMP1;
}

void bcm2835_timer_handle_interrupt() {
    volatile const uint64_t current_count = get_phy_count();

    // Begin of critical section, disable timer interrupts
    bcm2835_int_registers_base->DisableIRQ1 |= SysTimerCMP1;

    // Process all timers that are not in the future, if any
    prq_node * next_timer_node = prq_peek(scheduled_timers);
    while (next_timer_node != NULL &&
           get_prq_node_data(next_timer_node)->scheduled_count <= current_count) {
        prq_dequeue(scheduled_timers);
        ScheduledTimer * const next_timer = get_prq_node_data(next_timer_node);

        next_timer->callback();

        // If the timer is not periodic, it is removed
        if (next_timer->periodic_delay == 0) {
            kfree(next_timer);
            prq_free_node(next_timer_node);
        }
        // If the timer is periodic, it is updated and added to the queue again
        else {
            // TODO: Avoid duplicating code in `schedule_timer`
            const uint64_t scheduled_count = next_timer->scheduled_count;
            next_timer->scheduled_count = scheduled_count + next_timer->periodic_delay;
            assert(scheduled_count <= INT32_MAX);
            next_timer_node->priority = scheduled_count;

            prq_enqueue(scheduled_timers, next_timer_node);
        }

        next_timer_node = prq_peek(scheduled_timers);
    }

    // If there are no more scheduled timers in queue, mask interrupts until one is added again
    if (next_timer_node == NULL) {
        mask_and_enable_timer();
    }
    // If there still are, set compare val to next one
    else {
        set_phy_timer_cmp_val(get_prq_node_data(next_timer_node)->scheduled_count);
    }

    // End of critical section, re-enable timer interrupts
    bcm2835_int_registers_base->EnableIRQ1 |= SysTimerCMP1;

}

static TimerHandle schedule_timer(TimerCallback callback, uint32_t delay_ms, bool periodic) {
    assert(callback != NULL);
    assert(delay_ms > 0);

    const uint64_t count_offset = (get_frequency() / 1000) * delay_ms;
    volatile const uint64_t scheduled_count = get_phy_count() + count_offset;

    ScheduledTimer * const new_timer = kmalloc(sizeof(ScheduledTimer));
    prq_node * const new_timer_node = prq_create_node();

    new_timer->scheduled_count = scheduled_count;
    new_timer->callback = callback;
    assert(sizeof(TimerHandle) >= sizeof(prq_node *));
    new_timer->handle = (TimerHandle)new_timer_node;
    // 0 means the timer is not periodic
    new_timer->periodic_delay = periodic ? count_offset : 0;
    // TODO: Find better way of doing this
    assert(scheduled_count <= INT32_MAX);
    new_timer_node->priority = scheduled_count;
    new_timer_node->data = new_timer;

    // Begin of critical section, disable timer interrupts
    bcm2835_int_registers_base->DisableIRQ1 |= SysTimerCMP1;


    prq_enqueue(scheduled_timers, new_timer_node);

    set_phy_timer_cmp_val(get_prq_node_data(prq_peek(scheduled_timers))->scheduled_count);


    // End of critical section, re-enable timer interrupts
    bcm2835_int_registers_base->EnableIRQ1 |= SysTimerCMP1;

    return new_timer->handle;
}

TimerHandle bcm2835_schedule_timer_once(TimerCallback callback, uint32_t delay_ms) {
    return schedule_timer(callback, delay_ms, false);
}

TimerHandle bcm2835_schedule_timer_periodic(TimerCallback callback, uint32_t delay_ms) {
    return schedule_timer(callback, delay_ms, true);
}

void bcm2835_deschedule_timer(TimerHandle handle) {
    assert(sizeof(prq_node *) >= sizeof(TimerHandle));
    prq_node * const timer_node = (prq_node *)handle;

    prq_remove(scheduled_timers, timer_node);
    kfree(get_prq_node_data(timer_node));
    prq_free_node(timer_node);

    prq_node * const next_timer_node = prq_peek(scheduled_timers);
    // If there are no more scheduled timers in queue, mask interrupts until one is added again
    if (next_timer_node == NULL) {
        mask_and_enable_timer();
    }
    // If there still are, set compare val to next one
    else {
        set_phy_timer_cmp_val(get_prq_node_data(next_timer_node)->scheduled_count);
    }
}
