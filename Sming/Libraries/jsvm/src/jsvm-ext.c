#include "jsvm-ext.h"

#include "m_printf.h"

#define jerryx_port_handler_print_char(A) m_putc(A)

jerry_value_t alertFunction (const jerry_value_t function_obj,
							 const jerry_value_t this_val,
							 const jerry_value_t args_p[],
							 const jerry_length_t args_cnt)
{
	  jerry_value_t str_val = jerry_value_to_string (args_p[0]);

	  jerry_size_t req_sz = jerry_get_string_size (str_val);
	  jerry_char_t str_buf_p[req_sz + 1];

	  jerry_string_to_char_buffer (str_val, str_buf_p, req_sz);
	  str_buf_p[req_sz] = '\0';

	  m_printf("%s",(char *)str_buf_p);
	  jerry_release_value(str_val);

	  return jerry_create_boolean (true);
}

jerry_value_t printFunction (const jerry_value_t function_obj,
							 const jerry_value_t this_val,
							 const jerry_value_t args_p[],
							 const jerry_length_t args_cnt)
{
	  (void) function_obj; /* unused */
	  (void) this_val; /* unused */

	  static const char *null_str = "\\u0000";
	  jerry_length_t arg_index = 0;
	  jerry_size_t chr_index = 0;
	  jerry_size_t null_index = 0;

	  jerry_value_t ret_val = jerry_create_undefined ();

	  for (arg_index = 0;
	       jerry_value_is_undefined (ret_val) && arg_index < args_cnt;
	       arg_index++)
	  {
	    jerry_value_t str_val = jerry_value_to_string (args_p[arg_index]);

	    if (!jerry_value_is_error (str_val))
	    {
	      if (arg_index != 0)
	      {
	        jerryx_port_handler_print_char (' ');
	      }

	      jerry_size_t substr_size;
	      jerry_length_t substr_pos = 0;
	      jerry_char_t substr_buf[256];

	      while ((substr_size = jerry_substring_to_char_buffer (str_val,
	                                                            substr_pos,
	                                                            substr_pos + 256,
	                                                            substr_buf,
	                                                            256)) != 0)
	      {
	        for (chr_index = 0; chr_index < substr_size; chr_index++)
	        {
	          char chr = (char) substr_buf[chr_index];
	          if (chr == '\0')
	          {
	            for (null_index = 0; null_str[null_index] != 0; null_index++)
	            {
	              jerryx_port_handler_print_char (null_str[null_index]);
	            }
	          }
	          else
	          {
	            jerryx_port_handler_print_char (chr);
	          }
	        }

	        substr_pos += substr_size;
	      }

	      jerry_release_value (str_val);
	    }
	    else
	    {
	      ret_val = str_val;
	    }
	  }

	  jerryx_port_handler_print_char ('\n');

	  return ret_val;
}
