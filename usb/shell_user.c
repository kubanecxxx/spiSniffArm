/**
 * @file shell_user.c
 * @author kubanec
 * @date 9.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
//#include "test.h"

#include "shell.h"
#include "chprintf.h"
#include "stdlib.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//#define TEST_WA_SIZE    THD_WA_SIZE(1024)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
	size_t n, size;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
	static const char *states[] =
	{ THD_STATE_NAMES};
	Thread *tp;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "   name    stack prio refs     state time  \r\n");
	tp = chRegFirstThread();
	do
	{
		chprintf(chp, "%15s  %.8lx  %4lu  %4lu  %9s  %lu \r\n", tp->p_name,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t) (tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	}while (tp != NULL );
}

static SPIDriver * spi = &SPID1;
static SPIConfig config =
{ NULL, GPIOA, 8, 0 };

static uint8_t hex2int(const char * hex)
{
	uint8_t i;
	uint8_t temp = 0;

	uint8_t a;
	for (a = 2; a != 0; a--)
	{
		i = *hex++;
		if (i >= '0' && i <= '9')
		{
			i -= 48;
		}
		else
		{
			//udělat maly pismeno
			i |= 0x20;
			i -= 87;
		}
		temp |= i << ((a - 1) * 4);
	}

	return temp;
}

static void cmd_data(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc != 2)
	{
		chprintf(chp, "data smula");
		return;
	}

	uint32_t i;
	uint8_t tx[100];
	uint8_t rx[100];
	uint32_t count = atoi(argv[0]);
	char * ar = argv[1];

	for (i = 0; i < count; i++)
	{
		tx[i] = hex2int(ar);
		ar += 2;
	}

	spiSelect(spi);
	spiExchange(spi, count, tx, rx);
	spiUnselect(spi);

	chSequentialStreamWrite(chp,"data",4);

	for (i = 0; i < count; i++)
		chSequentialStreamPut(chp,rx[i]);

	chSequentialStreamPut(chp,'\n');
	chSequentialStreamPut(chp,'\r');
}

static void cmd_readConf(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	chprintf(chp, "conf %d\n\r", config.cr1);
}

static void cmd_Setup(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) chp;
	(void) argc;
	(void) argv;

	if (argc != 1)
	{
		chprintf(chp, "set smula\n\r");
	}
	uint32_t cr = atoi(argv[0]);
	config.cr1 = cr;

	if (spi->state == SPI_READY)
		spiStop(spi);

	spiStart(spi, &config);
	palSetPadMode(config.ssport,config.sspad,PAL_MODE_OUTPUT_PUSHPULL);
	palSetGroupMode(GPIOA,0b111,5,PAL_MODE_ALTERNATE(5));
}

const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ "data", cmd_data },
{ "read_conf", cmd_readConf },
{ "setup", cmd_Setup },
{ NULL, NULL } };
