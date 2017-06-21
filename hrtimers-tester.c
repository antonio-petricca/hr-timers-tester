/**
 * @file    hrtimers-tester.c
 * @author  Antonio Petricca (antonio.petricca@gmail.com)
 * @date    20 Jun 2017
 * @version 0.0.1
 * @brief   An implementation of a HR timers tester.
*/

#include <linux/hrtimer.h> // High Resolution Timers
#include <linux/init.h>    // Macros used to mark up functions __init __exit
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel
#include <linux/ktime.h>   // ktime_get, ...
#include <linux/module.h>  // Core header for loading LKMs into the kernel

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio Petricca (antonio.petricca@gmail.com)");
MODULE_DESCRIPTION("HR timers tester.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

#define START_TIME      100
#define END_TIME        1000 * 1000 * 1000
#define STEP_LOOPS      5
#define LOOP_MULTIPLIER 2

static struct  completion sem;
static struct  hrtimer    timer;

static ktime_t            loc_perf_time;
static ktime_t            tot_perf_time;

static ktime_t            end_time;
volatile ktime_t          delay_time;
volatile int              step_loops;

#define GET_PERF_TIME(perf_time) ktime_to_ns(ktime_sub(ktime_get(), perf_time))
#define SET_PERF_TIME(perf_time) perf_time = ktime_get()

enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
  pr_info(
    "[%d] %15llu = %15llu ns\n", 
    (STEP_LOOPS - step_loops + 1),
    ktime_to_ns(delay_time), 
    GET_PERF_TIME(loc_perf_time)
  );

  SET_PERF_TIME(loc_perf_time);
  
  step_loops--;

  if (step_loops <= 0)
  {
    if (delay_time >= end_time)
    {
      complete(&sem);
      return HRTIMER_NORESTART;
    }

    delay_time *= LOOP_MULTIPLIER;
    step_loops  = STEP_LOOPS;
  }

  hrtimer_forward_now(timer, delay_time);
  return HRTIMER_RESTART;
}

static int __init timertest_init(void)
{
  pr_info("Started...\n");
  
  init_completion(&sem);

  hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  timer.function = &timer_callback;

  delay_time = ktime_set(0, START_TIME);
  end_time   = ktime_set(0, END_TIME);
  step_loops = STEP_LOOPS;

  SET_PERF_TIME(tot_perf_time);
  SET_PERF_TIME(loc_perf_time);

  hrtimer_start(&timer, delay_time, HRTIMER_MODE_REL);
  wait_for_completion_killable(&sem);

  pr_info("Completed in ~ %15llu ns.\n", GET_PERF_TIME(tot_perf_time));

  hrtimer_cancel(&timer);

  return 0;
}

static void __exit timertest_exit(void)
{
  pr_info("module unloaded.\n");
}

module_init(timertest_init);
module_exit(timertest_exit); 