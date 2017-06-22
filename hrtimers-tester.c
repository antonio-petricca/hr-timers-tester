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

#define OUTER_LOOP_START  10      /* 10 uS */
#define OUTER_LOOP_END    1000000 /* 1 s  */
#define LOOP_ITEMS        46

struct perf_sample
{
  int     expected;
  ktime_t sampled;
};

static struct  completion sem;

static struct  hrtimer    timer;
static ktime_t            timer_incr;
static ktime_t            timer_perf;
static int                timer_perf_index;
static struct perf_sample timer_perf_samplings[LOOP_ITEMS];

static int                inner_loop_index; 
static int                inner_loop_incr; 
static int                inner_loop_end; 

enum hrtimer_restart hrtimers_test_callback(struct hrtimer *timer)
{
  if (inner_loop_index >= inner_loop_end)
  {
    complete(&sem);
    return HRTIMER_NORESTART;
  }

  timer_perf_samplings[timer_perf_index].expected = inner_loop_index;
  
  timer_perf_samplings[timer_perf_index].sampled = 
    ktime_sub(ktime_get(), timer_perf);

  /*pr_info(
    "  inner loop = %9d => %9llu\n", 
    inner_loop_index,
    ktime_to_ns(timer_perf_samplings[timer_perf_index].sampled)
  );*/

  timer_perf_index++;
  timer_perf = ktime_get();

  inner_loop_index += inner_loop_incr;
  timer_incr        = ktime_set(0, (inner_loop_index * 1000));

  hrtimer_forward_now(timer, timer_incr);

  return HRTIMER_RESTART;
}

static void hrtimers_test_collect(void)
{
  int     outer_loop_index;
  int     outer_loop_end;
  ktime_t perf_time;
  
  pr_info("Collecting...\n");
  
  init_completion(&sem);
  hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  timer.function   = &hrtimers_test_callback;

  outer_loop_index = OUTER_LOOP_START;
  outer_loop_end   = (OUTER_LOOP_END / 10);
  
  timer_perf_index = 0;
  perf_time        = ktime_get();

  while (outer_loop_index <= outer_loop_end)
  {
    //pr_info("outer loop = %9d\n", outer_loop_index);

    inner_loop_index = outer_loop_index;
    inner_loop_incr  = outer_loop_index;
    timer_incr       = ktime_set(0, (inner_loop_incr * 1000));

    inner_loop_end   = 
        (outer_loop_index < outer_loop_end)
      ? (inner_loop_index * 10)
      : (inner_loop_index * (10 + 1))
    ;

    timer_perf = ktime_get();

    hrtimer_start(&timer, timer_incr, HRTIMER_MODE_REL);
    wait_for_completion_killable(&sem);
    
    outer_loop_index *= 10;
  }

  hrtimer_cancel(&timer);

  pr_info(
    "Completed in ~ %9llu ns.\n", 
    ktime_to_ns(ktime_sub(ktime_get(), perf_time))
  );
}

static void hrtimers_test_print_samplings(void)
{
  int index = 0;

  pr_info("Sampled values:\n");

  for (index = 0; index < LOOP_ITEMS; index++)
  {
    unsigned long long time = ktime_to_ns(timer_perf_samplings[index].sampled);

    pr_info(
      " [%2d] %7d = %7llu.%03llu uS",
      (index + 1),
      timer_perf_samplings[index].expected,
      (time / 1000),
      (time % 1000)
    );
  }
}

static int __init hrtimers_test_init(void)
{
  hrtimers_test_collect();
  hrtimers_test_print_samplings();

  return 0;
}

static void __exit hrtimers_test_exit(void)
{
  pr_info("module unloaded.\n");
}

module_init(hrtimers_test_init);
module_exit(hrtimers_test_exit); 