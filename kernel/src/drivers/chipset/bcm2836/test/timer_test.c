#include <timer.h>
#include <test.h>

static int callback_count;

static void test_callback() {
    callback_count++;
}

TEST_CREATE(test_timer_cb_called, {
    callback_count = 0;
    bcm2836_schedule_timer_once(test_callback, 100);
    DEBUG("If the test gets stuck here, the timer callback was never called");
    while (callback_count != 1);
    PASS();
})

TEST_CREATE(test_timer_cancel, {
    callback_count = 0;
    TimerHandle const handle = bcm2836_schedule_timer_once(test_callback, 100);
    bcm2836_deschedule_timer(handle);
    ASSERT_EQ(callback_count, 0);
})

TEST_CREATE(test_timer_periodic_cb_called, {
    callback_count = 0;
    TimerHandle const handle = bcm2836_schedule_timer_periodic(test_callback, 100);
    DEBUG("If the test gets stuck here, the timer callback was not called the required number of times");
    while (callback_count != 10);
    bcm2836_deschedule_timer(handle);
    PASS();
})
