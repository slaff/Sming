#include <stdarg.h>
#ifdef ARCH_HOST
#define __USE_MISC 1
#endif
#include <sys/time.h>

#include "jerry-core/include/jerryscript-port.h"
#include "m_printf.h"

#ifndef strnlen
static size_t
strnlen(const char *s, size_t maxlen)
{
  const char *p;

  p = memchr(s, '\0', maxlen);
  if (p == NULL)
    return maxlen;

  return p - s;
}
#endif

/**
 * Provide console message implementation for the engine.
 */
void
jerry_port_console (const char *format, /**< format string */
                    ...) /**< parameters */
{
  va_list args;
  va_start (args, format);
  m_vprintf(format, args);
  va_end (args);
} /* jerry_port_console */

/**
 * Provide log message implementation for the engine.
 */
void
jerry_port_log (jerry_log_level_t level, /**< log level */
                const char *format, /**< format string */
                ...)  /**< parameters */
{
  (void) level; /* ignore log level */

  va_list args;
  va_start (args, format);
  /* TODO, uncomment when vprint link is ok */
  /* vprintf (stderr, format, args); */
  va_end (args);
} /* jerry_port_log */


/** exit - cause normal process termination  */
void exit (int status)
{
  while (true)
  {
  }
} /* exit */

/**
 * fwrite
 *
 * @return number of bytes written
 */
size_t
fwrite (const void *ptr, /**< data to write */
        size_t size, /**< size of elements to write */
        size_t nmemb, /**< number of elements */
        FILE *stream) /**< stream pointer */
{
  return size * nmemb;
} /* fwrite */

/**
 * Default implementation of jerry_port_get_current_time.
 */
double jerry_port_get_current_time ()
{
  struct timeval tv;

  if (gettimeofday (&tv, NULL) != 0)
  {
    return 0;
  }

  return ((double) tv.tv_sec) * 1000.0 + ((double) tv.tv_usec) / 1000.0;
} /* jerry_port_get_current_time */

/**
 * Default implementation of jerry_port_get_local_time_zone_adjustment. Uses the 'tm_gmtoff' field
 * of 'struct tm' (a GNU extension) filled by 'localtime_r' if available on the
 * system, does nothing otherwise.
 *
 * @return offset between UTC and local time at the given unix timestamp, if
 *         available. Otherwise, returns 0, assuming UTC time.
 */
double jerry_port_get_local_time_zone_adjustment (double unix_ms,  /**< ms since unix epoch */
                                                  bool is_utc)  /**< is the time above in UTC? */
{
	return 0.0;
}

static bool abort_on_fail = false;

/**
 * Sets whether 'abort' should be called instead of 'exit' upon exiting with
 * non-zero exit code in the default implementation of jerry_port_fatal.
 */
void jerry_port_default_set_abort_on_fail (bool flag) /**< new value of 'abort on fail' flag */
{
  abort_on_fail = flag;
} /* jerry_port_default_set_abort_on_fail */

/**
 * Check whether 'abort' should be called instead of 'exit' upon exiting with
 * non-zero exit code in the default implementation of jerry_port_fatal.
 *
 * @return true - if 'abort on fail' flag is set,
 *         false - otherwise.
 */
bool jerry_port_default_is_abort_on_fail ()
{
  return abort_on_fail;
} /* jerry_port_default_is_abort_on_fail */

/**
 * Default implementation of jerry_port_fatal.
 */
void jerry_port_fatal (jerry_fatal_code_t code)
{
  if (code != 0
      && code != ERR_OUT_OF_MEMORY
      && jerry_port_default_is_abort_on_fail ())
  {
    abort ();
  }
  else
  {
    exit (code);
  }
} /* jerry_port_fatal */


#define MPRINTF_BUF_SIZE 256

#define OVERFLOW_GUARD 24

/**
 * @fn int m_printf(const char *fmt, ...);
 *
 * @param fmt - printf compatible format string
 *
 * @retval int - number of characters written to console
 */
int printf(const char *fmt, ...)
{
	char buf[MPRINTF_BUF_SIZE], *p;
	va_list args;
	int n = 0;

	va_start(args, fmt);
	m_vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	p = buf;
	while (*p)
	{
		m_putc(*p);
		n++;
		p++;
	}

	return n;
}


/**
 * Opens file with the given path and reads its source.
 * @return the source of the file
 */
uint8_t *
jerry_port_read_source (const char *file_name_p, /**< file name */
                        size_t *out_size_p) /**< [out] read bytes */
{
  return NULL;
} /* jerry_port_read_source */

/**
 * Release the previously opened file's content.
 */
void
jerry_port_release_source (uint8_t *buffer_p) /**< buffer to free */
{
  free (buffer_p);
} /* jerry_port_release_source */

/**
 * Normalize a file path
 *
 * @return length of the path written to the output buffer
 */
size_t
jerry_port_normalize_path (const char *in_path_p,   /**< input file path */
                           char *out_buf_p,         /**< output buffer */
                           size_t out_buf_size,     /**< size of output buffer */
                           char *base_file_p)       /**< base file path */
{
  size_t ret = 0;

  (void) base_file_p;

  /* Do nothing, just copy the input. */
  const size_t len = strnlen (in_path_p, out_buf_size);
  if (len < out_buf_size)
  {
    strncpy (out_buf_p, in_path_p, out_buf_size);
    ret = len;
  }

  return ret;
} /* jerry_port_normalize_path */


/**
 * Get the module object of a native module.
 *
 * @return Undefined, if 'name' is not a native module
 *         jerry_value_t containing the module object, otherwise
 */
jerry_value_t
jerry_port_get_native_module (jerry_value_t name) /**< module specifier */
{
  (void) name;
  return jerry_create_undefined ();
} /* jerry_port_get_native_module */


