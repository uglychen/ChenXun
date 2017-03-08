#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/time.h>
#include <errno.h>

/* Typical include path would be <librdkafka/rdkafka.h>, but this program
* is builtin from within the librdkafka source tree and thus differs. */
#include "rdkafka.h"  /* for Kafka driver */

static int run = 1;
static rd_kafka_t *rk;

static void stop(int sig) 
{
    run = 0;
    fclose(stdin); /* abort fgets() */
}

static void hexdump(FILE *fp, const char *name, const void *ptr, size_t len)
{
    const char *p = (const char *)ptr;
    unsigned int of = 0;

    if (name)
        fprintf(fp, "%s hexdump (%zd bytes):\n", name, len);

    for (of = 0; of < len; of += 16)
    {
        char hexen[16 * 3 + 1];
        char charen[16 + 1];
        int hof = 0;

        int cof = 0;
        int i;

        for (i = of; i < (int)of + 16 && i < (int)len; i++)
        {
            hof += sprintf(hexen + hof, "%02x ", p[i] & 0xff);
            cof += sprintf(charen + cof, "%c",
                isprint((int)p[i]) ? p[i] : '.');
        }
        fprintf(fp, "%08x: %-48s %-16s\n",
            of, hexen, charen);
    }
}

/**
* Kafka logger callback (optional)
*/
static void logger(const rd_kafka_t *rk, int level,const char *fac, const char *buf) 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "%u.%03u RDKAFKA-%i-%s: %s: %s\n",
        (int)tv.tv_sec, (int)(tv.tv_usec / 1000),
        level, fac, rk ? rd_kafka_name(rk) : NULL, buf);
}

/**
* Message delivery report callback.
* Called once for each message.
* See rdkafka.h for more information.
*/
static void msg_delivered(rd_kafka_t *rk, void *payload, size_t len, rd_kafka_resp_err_t error_code, void *opaque, void *msg_opaque)
{
    if (error_code)
        fprintf(stderr, "%% Message delivery failed: %s\n",rd_kafka_err2str((rd_kafka_resp_err_t)error_code));
}

static void sig_usr1(int sig) 
{
    rd_kafka_dump(stdout, rk);
}

void send_data_to_kafka(char *brokers, char *topic, int partition)
{
    rd_kafka_conf_t *conf;
    rd_kafka_topic_t *rkt;
    rd_kafka_topic_conf_t *topic_conf;
    char errstr[512];
    char tmp[16];

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    /* Set logger */
    rd_kafka_conf_set_log_cb(conf, logger);

    /* Quick termination */
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    /* Topic configuration */
    topic_conf = rd_kafka_topic_conf_new();

    signal(SIGINT, stop);
    signal(SIGUSR1, sig_usr1);

    /*
    * Producer
    */
    char buf[2048];

    /* Set up a message delivery report callback.
    * It will be called once for each message, either on successful
    * delivery to broker, or upon failure to deliver to broker. */
    rd_kafka_conf_set_dr_cb(conf, msg_delivered);

    /* Create Kafka handle */
    if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr))))
    {
        fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
        exit(1);
    }

    rd_kafka_set_log_level(rk, LOG_DEBUG);

    /* Add brokers */
    if (rd_kafka_brokers_add(rk, brokers) == 0)
    {
        fprintf(stderr, "%% No valid brokers specified\n");
        exit(1);
    }

    /* Create topic */
    rkt = rd_kafka_topic_new(rk, topic, topic_conf);

    while (run && fgets(buf, sizeof(buf), stdin))
    {
        size_t len = strlen(buf);
        if (buf[len - 1] == '\n')
        {
            buf[--len] = '\0';
        }

        /* Send/Produce message. */
        if (rd_kafka_produce(rkt, partition,
            RD_KAFKA_MSG_F_COPY,
            /* Payload and length */
            buf, len,
            /* Optional key and its length */
            NULL, 0,
            /* Message opaque, provided in
            * delivery report callback as
            * msg_opaque. */
            NULL) == -1)
        {
            fprintf(stderr,
                "%% Failed to produce to topic %s "
                "partition %i: %s\n",
                rd_kafka_topic_name(rkt), partition,
                rd_kafka_err2str(
                rd_kafka_errno2err(errno)));
            /* Poll to handle delivery reports */
            rd_kafka_poll(rk, 0);
            continue;
        }

        /* Poll to handle delivery reports */
        rd_kafka_poll(rk, 0);
    }

    /* Poll to handle delivery reports */
    rd_kafka_poll(rk, 0);

    /* Wait for messages to be delivered */
    while (run && rd_kafka_outq_len(rk) > 0)
        rd_kafka_poll(rk, 100);

    /* Destroy topic */
    rd_kafka_topic_destroy(rkt);

    /* Destroy the handle */
    rd_kafka_destroy(rk);

    /* Let background threads clean up and terminate cleanly. */
    run = 5;
    while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1)
        printf("Waiting for librdkafka to decommission\n");
    if (run <= 0)
        rd_kafka_dump(stdout, rk);

    return;
}

int main(int argc, char **argv) 
{
    char *brokers = "192.168.1.108:9092";
    char *topic = "chenxun";
    send_data_to_kafka(brokers, topic, 2);

    return 0;
}
