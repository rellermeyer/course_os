#include <bcm2836_timer.h>
#include <test.h>

static int callback_count_1;
static int callback_count_2;

static void test_callback_1() {
    callback_count_1++;
}

static void test_callback_2() {
    callback_count_2++;
}

TEST_CREATE(test_timer_cb_called, {
    callback_count_1 = 0;

    chipset.schedule_timer_once(test_callback_1, 50);
    DEBUG("If the test gets stuck here, the timer callback was never called");
    while (callback_count_1 != 1);
})

TEST_CREATE(test_timer_cancel, {
    callback_count_1 = 0;

    TimerHandle const handle = chipset.schedule_timer_once(test_callback_1, 50);
    chipset.deschedule_timer(handle);
    ASSERT_EQ(callback_count_1, 0);
})

TEST_CREATE(test_timer_periodic_cb_called, {
    callback_count_1 = 0;

    TimerHandle const handle = chipset.schedule_timer_periodic(test_callback_1, 50);
    DEBUG("If the test gets stuck here, the timer callback was not called the requirxed number of times");
    while (callback_count_1 != 10);
    chipset.deschedule_timer(handle);
})

TEST_CREATE(test_timer_order, {
    callback_count_1 = 0;
    callback_count_2 = 0;

    chipset.schedule_timer_once(test_callback_1, 50);
    chipset.schedule_timer_once(test_callback_2, 100);
    DEBUG("If the test gets stuck here, one of the timer callbacks was never called");
    while (callback_count_1 != 1);
    ASSERT_EQ(callback_count_2, 0);
    while (callback_count_2 != 1);
})
