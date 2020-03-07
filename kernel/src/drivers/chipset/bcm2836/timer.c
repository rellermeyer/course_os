#include <timer.h>
#include <bcm2836.h>
#include <chipset.h>
#include <stdint.h>
#include <stdio.h>
#include <priority_queue.h>
#include <stdlib.h>

typedef struct ScheduledTimer
{
    uint64_t scheduled_count;
    TimerHandle handle;
    TimerCallback callback;
}
ScheduledTimer;

typedef union LittleEndianUint64
{
    uint64_t dword;
    struct {
        uint32_t low_word;
        uint32_t high_word;
    };
}
LittleEndianUint64;

/*
 * Gives the frequency of the counter in kHz
 */
static inline uint32_t get_frequency()
{
    uint32_t val;
    // Read CNTFRQ
    asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val));
    return val;
}

static prq_handle* scheduled_timers;

static void unmask_and_enable_timer()
{
    // Disable output mask, enable timer
    static const uint32_t cntp_ctl = 0b01;
    // Write CNTP_CTL
    asm volatile ("mcr p15, 0, %0, c14, c2, 1" :: "r"(cntp_ctl));
}

static void mask_and_enable_timer()
{
    // Enable output mask, enable timer
    static const uint32_t cntp_ctl = 0b11;
    // Write CNTP_CTL
    asm volatile ("mcr p15, 0, %0, c14, c2, 1" :: "r"(cntp_ctl));
}

static uint64_t get_phy_count()
{
    LittleEndianUint64 val;
    // Read CNTPCT
    asm volatile ("mrrc p15, 0, %0, %1, c14" : "=r"(val.low_word), "=r"(val.high_word));
    return val.dword;
}

/*
static int32_t get_phy_timer_val()
{
    int32_t val;
    // Read CNTP_TVAL
    asm volatile ("mrc p15, 0, %0, c14, c2, 0" : "=r"(val));
    return val;
}

static void set_phy_timer_val(int32_t val)
{
    // Write CNTP_TVAL
    asm volatile ("mcr p15, 0, %0, c14, c2, 0" :: "r"(val));
}

static uint64_t get_phy_timer_cmp_val()
{
    LittleEndianUint64 val;
    // Read CNTP_CVAL
    asm volatile ("mrrc p15, 2, %0, %1, c14" : "=r"(val.low_word), "=r"(val.high_word));
    return val.dword;
}
*/

static void set_phy_timer_cmp_val(uint64_t val)
{
    const LittleEndianUint64 le_val = (LittleEndianUint64)val;
    // Write CNTP_CVAL
    asm volatile ("mcrr p15, 2, %0, %1, c14" :: "r"(le_val.low_word), "r"(le_val.high_word));
}

static inline ScheduledTimer* get_prq_node_data(prq_node* node)
{
    return (ScheduledTimer*) node->data;
}


static void foo() {
    kprintf("Timer callback called\n");
}
void bcm2836_timer_init()
{
    const uint32_t freq = get_frequency();
    kprintf("System counter frequency: %u kHz\n", freq / 1000);

    bcm2836_registers_base->Core0TimersInterruptControl = PHYSICAL_SECURE_TIMER;

    // Init priority queue
    scheduled_timers = prq_create();

    mask_and_enable_timer();


    // TEMP
    bcm2836_schedule_timer_once(foo, 2000);
//    set_phy_timer_cmp_val(get_phy_count() + freq * 2000);
//    unmask_and_enable_timer();
}

// TODO: Handle possibility of timer scheduling functions or interrupt handler being interrupted

void timer_handle_interrupt()
{
    volatile const uint64_t current_count = get_phy_count();

    // Process all timers that are not in the future, if any
    prq_node* next_timer_node = prq_peek(scheduled_timers);
    while (get_prq_node_data(next_timer_node)->scheduled_count <= current_count) {
        prq_dequeue(scheduled_timers);
        ScheduledTimer* next_timer = get_prq_node_data(next_timer_node);

        next_timer->callback();
        kfree(next_timer);
        prq_free_node(next_timer_node);

        next_timer_node = prq_peek(scheduled_timers);
    }
    set_phy_timer_cmp_val(get_prq_node_data(next_timer_node)->scheduled_count);


    // TEMP
//    kprintf("Timer interrupt received\n");
//    mask_and_enable_timer();
}

// TODO: Handle possibility of two timers getting scheduled for the same time (counter value), PRQ needs support

/*
 * This implementation may execute timers out of order if their delay is very small
 */
TimerHandle bcm2836_schedule_timer_once(TimerCallback callback, uint32_t delay_ms)
{
    volatile const uint64_t scheduled_count = get_phy_count() + (get_frequency() / 1000) * delay_ms;

    ScheduledTimer* new_timer = (ScheduledTimer*) kmalloc(sizeof(ScheduledTimer));
    prq_node* new_timer_node = prq_create_node();

    new_timer->scheduled_count = scheduled_count;
    new_timer->callback = callback;
    new_timer->handle = (TimerHandle) new_timer_node;
    // TODO: Find better way of doing this
    new_timer_node->priority = UINT64_MAX - scheduled_count;
    new_timer_node->data = new_timer;

    prq_enqueue(scheduled_timers, new_timer_node);

    set_phy_timer_cmp_val(get_prq_node_data(prq_peek(scheduled_timers))->scheduled_count);

    unmask_and_enable_timer();

    return new_timer->handle;
}

TimerHandle bcm2836_schedule_timer_periodic(TimerCallback callback, uint32_t delay_ms)
{
    // TEMP
    return 0;
}

void bcm2836_deschedule_timer(TimerHandle handle) {}
