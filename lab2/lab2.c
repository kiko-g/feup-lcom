#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include <stdbool.h>
#include <stdint.h>
extern uint32_t global_interrupt_counter;

int main(int argc, char *argv[])
{
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");
  lcf_log_output("/home/lcom/labs/lab2/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field)
{
   uint8_t status;
   if(timer_get_conf(timer, &status)) {
      printf("Unable to retrieve the timer's configuration\n");
      return 1;
   }

   if(timer_display_conf(timer, status, field)) {
      printf("Unable to display the timer's configuration");
      return 1;
   }

   return 0;
}


int(timer_test_time_base)(uint8_t timer, uint32_t freq) 
{
    return timer_set_frequency(timer, freq);
}


int(timer_test_int)(uint8_t time)
{
    uint8_t irq_set;
    int ipc_status, r;
    message msg;

    if(timer_subscribe_int(&irq_set)) {
      printf("Failed to subscribe\n");
      return 1;
    }

    while(time)
    {
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) /* received notification */
      {
        switch (_ENDPOINT_P(msg.m_source))
        {
          case HARDWARE:  /* hardware interrupt notification */
            if (msg.m_notify.interrupts & irq_set) 
            {
              timer_int_handler();
	            if(global_interrupt_counter % sys_hz()  == 0)
              {
                 time--;
		             timer_print_elapsed_time();
	            }
            }
           break;

	        default:
           break; /* no other notifications expected: do nothing */
        }
    }

    else { }   /* received a standard message, not a notification */
               /* no standard messages expected: do nothing */
  }

  if(timer_unsubscribe_int()) {
    printf("Failed to unsubscribe\n");
    return 1;
  }

  return 0;
}
