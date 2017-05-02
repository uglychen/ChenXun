���ĵ�Ϊzlogʹ���ֲᣬ�����https://hardysimpson.github.io/zlog/UsersGuide-CN.html

����ʹ�ò��裺

1  ����Ͱ�װzlog

����zlog-latest-stable.tar.gz  ���뱾�ļ�ͬ��Ŀ¼������ѹ���� zlog.tar.gz

$ tar -zxvf zlog-latest-stable.tar.gz
$ cd zlog-latest-stable/

$ make 

$ sudo make install

or

$ sudo make PREFIX=/usr/local/ install

PREFIXָ���˰�װ��·������ת��֮��Ϊ������ĳ������ҵ�zlog��̬��

$ sudo vi /etc/ld.so.conf
/usr/local/lib

$ sudo ldconfig

����ĳ�������֮ǰ����֤libzlog.so��ϵͳ�Ķ�̬���ӿ�����������ҵ���Ŀ¼�¡����������������linux�����ϵͳ�Լ���취��

����һ���make���⣬������
$ make 32bit # 32bit version on 64bit machine, libc6-dev-i386 is needed
$ make noopt # without gcc optimization

$ make doc   # lyx and hevea is needed

$ make test  # test code, which is also good example for zlog

makefile����GNU make�ĸ�ʽд�ģ����������ƽ̨����ҪԤװgnu make��gcc�����ߣ��ֹ��޸�һ���Լ�ƽ̨��makefileҲ�С�
2  Ӧ�ó�����ú�����zlog

Ӧ�ó���ʹ��zlog�ܼ򵥣�ֻҪ��C�ļ������һ�С�

#include "zlog.h"
����zlog��Ҫpthread�⣬�����ǣ�

$ cc -c -o app.o app.c -I/usr/local/include
  # -I[where zlog.h is put]

$ cc -o app app.o -L/usr/local/lib -lzlog -lpthread

  # -L[where libzlog.so is put]

3  Hello World ����

��Щ������$(top_builddir)/test/test_hello.c, test_hello.conf

дһ��C�ļ���
$ vi test_hello.c
#include <stdio.h> 

#include "zlog.h"

 

int main(int argc, char** argv)

{

int rc;
zlog_category_t *c;

rc = zlog_init("test_hello.conf");

if (rc) {

printf("init failed\n");
return -1;

}

c = zlog_get_category("my_cat");

if (!c) {

printf("get cat fail\n");

zlog_fini();
return -2;

}

zlog_info(c, "hello, zlog");

zlog_fini();

return 0;

} 

дһ�������ļ������ں�test_hello.cͬ����Ŀ¼��:
$ vi test_hello.conf
[formats]

simple = "%m%n"

[rules]

my_cat.DEBUG    >stdout; simple

���磺
///////////////////////////////////////
[global]
strict init = true
reload conf period = 10M

buffer min = 1024
buffer max = 2MB

rotate lock file = /tmp/zlog.lock
rotate lock file = self
default format = "%d %-5V [%c:%f:%L]: %m%n"

file perms = 600
fsync period = 1K
[formats]
simple  =   "%d %-5V [%c:%f:%L]: %m%n"
[rules]
#my_cat.DEBUG            >stdout
*.DEBUG        "./log/trade.log", 10M ~        "./log/trade-%d(%Y%m%d).#6s.log"


#my_cat.DEBUG    >stdout; simple
#my_cat.INFO      >stdout
///////////////////////////////////////

���롢Ȼ������!
$ cc -c -o test_hello.o test_hello.c -I/usr/local/include
$ cc -o test_hello test_hello.o -L/usr/local/lib -lzlog

$ ./test_hello

hello, zlog

4  ���򵥵�Hello World

���������$(top_builddir)/test/test_default.c, test_default.conf. Դ�����ǣ�

#include <stdio.h>
#include "zlog.h"

int main(int argc, char** argv)

{

int rc;
rc = dzlog_init("test_default.conf", "my_cat");

if (rc) {

printf("init failed\n");
return -1;

}

dzlog_info("hello, zlog");

zlog_fini();

return 0;

} 

�����ļ���test_default.conf����test_hello.confһģһ�������ִ�г�������Ҳһ��������������������dzlog API���ں�һ��Ĭ�ϵ�zlog_category_t����������ֲ����ݡ�




//***************************************
//***************************************
���������Ϊ�ֲ����ݣ�

Chapter 1  zlog��ʲô��

zlog��һ���߿ɿ��ԡ������ܡ��̰߳�ȫ�������������Ĵ�C��־�����⡣

��ʵ�ϣ���C����������û���ر�õ���־�����⣨����JAVA����ĵ�log4j������C++��log4cxx����C����Ա��ϲ�����Լ������ӡ�printf���Ǹ�ͦ�õ����ӣ���û�취ͨ�����øı���־�ĸ�ʽ��������ļ���syslog�Ǹ�ϵͳ��������ӣ������ٶ��������ҹ��ܱȽϵ�����

������д��zlog��

zlog��Ч�ʡ����ܡ���ȫ���ϴ�󳬹���log4c����������cд�ɵģ����бȽϺõ�ͨ���ԡ�

zlog����Щ���ԣ�

syslog����ģ�ͣ���log4jģ�͸���ֱ���˵�
��־��ʽ���ƣ�������log4j��pattern layout
���������������̬�ļ�����̬�ļ���stdout��stderr��syslog���û��Զ����������
����ʱ�ֶ����Զ�ˢ�������ļ���ͬʱ��֤��ȫ��
�����ܣ����ҵıʼǱ��ϴﵽ25������־ÿ��, �����syslog(3)���rsyslogd��1000���ٶ�
�û��Զ���ȼ�
���̺߳Ͷ���̻����±�֤��ȫת��
��ȷ��΢��
�򵥵��ð�װdzlog��һ������Ĭ��ֻ��һ�����ࣩ
MDC���̼߳�-ֵ�Եı�������չ�û��Զ�����ֶ�
����ϣ�����������ʱ���zlog�Լ�����־������״̬
�����������⣬ֻҪ�Ǹ�POSIXϵͳ�ͳ�(��Ȼ��Ҫһ��C99���ݵ�vsnprintf)
������ӣ�

��ҳ��http://hardysimpson.github.com/zlog/

���أ�https://github.com/HardySimpson/zlog/releases

���䣺HardySimpson1984@gmail.com

1.1  ������˵��

zlog�ǻ���POSIX�ġ�Ŀǰ�������еĻ���ֻ��AIX��linux����������ϵͳ�£�FreeBSD, NetBSD, OpenBSD, OpenSolaris, Mac OS X...������Ҳ���У������⻶ӭ̽�֡�
zlogʹ����һ��C99���ݵ�vsnprintf��Ҳ����˵��������С���㣬vsnprintf���᷵��Ŀ���ַ���Ӧ�еĳ��ȣ���������\0��)����������ϵͳ��vsnprintf������ô�����ģ�zlog�Ͳ�֪����ô���󻺴档�����Ŀ�껺�治����ʱ��vsnprintf����-1��zlog�ͻ���Ϊ���д��ʧ�ܡ����˵���Ŀǰ�����c��׼�����C99��׼��glibc 2.1,libc on AIX, libc on freebsd...���Ǻõģ�����glibc2.0���ǡ�����������£��û���Ҫ�Լ���װһ��C99���ݵ�vsnprintf����crack��������⡣���Ƽ�ctrio, ����C99-snprintf��ֻҪ��buf.c���У�ף���ˣ�
�������ṩ�����°汾����������ƽ̨�ϰ�װ���룬�ǳ���л��
auto tools�汾: https://github.com/bmanojlovic/zlog

cmake�汾: https://github.com/lisongmin/zlog

windows�汾: https://github.com/lopsd07/WinZlog

1.2  zlog 1.2 ����˵��

zlog 1.2 ��������Щ����
�Թܵ���֧�֣��Ӵ�zlog�������cronolog��������־���˳��������
ȫ�����־ת��֧�֣����5.6
���������ԵĴ���Ķ�
zlog 1.2 �ڿⷽ���Ǻ�zlog 1.0/1.1�����Ƽ��ݵģ��������ڣ�
���еĺ��ΪСд��ZLOG_INFO->zlog_info�����㿪�����ֹ����롣����һ���޴�ĸı䣬���zlog1.1/1.0���û�Ҫ��zlog 1.2�Ļ�����Ҫдһ���ű�����Դ�����еĴ�д�����滻ΪСд��Ȼ�����±�����ĳ������ṩ��һ���ű���
sed -i -e ��s/\b\w*ZLOG\w*\b/\L&\E/g�� aa.c
ȡ����auto tools��ʹ�ã�Ҳ����˵�����������κ�ƽ̨������Ҫgcc��gnu make���ܱ��밲װzlog�������Ĳ���ϵͳ(Aix, OpenSolaris..)���ܰ�װgcc��gnu make����ȻҲ���������޸�makefile����ɱ��룬����ƽ̨���о����Geek������������ɣ�
Chapter 2  zlog����ʲô��

zlog��Ŀ���ǳ�Ϊһ�����������־�����⣬����ֱ��֧�������������д�����ݿ⣬����ֱ��֧����־���ݵĹ��˺ͽ�����

ԭ������ԣ���־���Ǳ�Ӧ�ó�����õģ����л�����־���ϵ�ʱ�䶼��Ӧ�ó�������ʱ���һ���֣�������˵����Щ�������ܷ�ʱ�䣬������Ӧ�ó�����ٶȡ���Щ�¶�Ӧ���ڱ�Ľ��̻��߱�Ļ���������

�������Ҫ��Щ���ԣ��ҽ���ʹ��rsyslog��zLogFabric��Logstash����Щ��־�Ѽ������ˡ��洢�������Ȼ���ǵ����Ľ��̣�����Ӧ�ó����һ���֡�

Ŀǰzlog�Ѿ�֧��7.4�������Լ�ʵ��һ��������������ɵİ���־������������̻�������������������־�ķ���ƥ�䡢��־��ʽ���͵Ĺ�������zlog��

Ŀǰ�ҵ��뷨��ʵ��һ��zlog-redis�ͻ��ˣ����Զ���������ܣ�����־�洢����������Զ�̵�redis�������ڣ�Ȼ������������(Ҳʹ��zlog��)������־д���ļ����棬��֪�����Ϊ����뷨��Σ���ӭ������ϵ̽�֡�

Chapter 3  Hello World

3.1  ����Ͱ�װzlog

����zlog-latest-stable.tar.gz

$ tar -zxvf zlog-latest-stable.tar.gz
$ cd zlog-latest-stable/

$ make 

$ sudo make install

or

$ sudo make PREFIX=/usr/local/ install

PREFIXָ���˰�װ��·������ת��֮��Ϊ������ĳ������ҵ�zlog��̬��

$ sudo vi /etc/ld.so.conf
/usr/local/lib

$ sudo ldconfig

����ĳ�������֮ǰ����֤libzlog.so��ϵͳ�Ķ�̬���ӿ�����������ҵ���Ŀ¼�¡����������������linux�����ϵͳ�Լ���취��

����һ���make���⣬������
$ make 32bit # 32bit version on 64bit machine, libc6-dev-i386 is needed
$ make noopt # without gcc optimization

$ make doc   # lyx and hevea is needed

$ make test  # test code, which is also good example for zlog

makefile����GNU make�ĸ�ʽд�ģ����������ƽ̨����ҪԤװgnu make��gcc�����ߣ��ֹ��޸�һ���Լ�ƽ̨��makefileҲ�С�
3.2  Ӧ�ó�����ú�����zlog

Ӧ�ó���ʹ��zlog�ܼ򵥣�ֻҪ��C�ļ������һ�С�

#include "zlog.h"
����zlog��Ҫpthread�⣬�����ǣ�

$ cc -c -o app.o app.c -I/usr/local/include
  # -I[where zlog.h is put]

$ cc -o app app.o -L/usr/local/lib -lzlog -lpthread

  # -L[where libzlog.so is put]

3.3  Hello World ����

��Щ������$(top_builddir)/test/test_hello.c, test_hello.conf

дһ��C�ļ���
$ vi test_hello.c
#include <stdio.h> 

#include "zlog.h"

 

int main(int argc, char** argv)

{

int rc;
zlog_category_t *c;

rc = zlog_init("test_hello.conf");

if (rc) {

printf("init failed\n");
return -1;

}

c = zlog_get_category("my_cat");

if (!c) {

printf("get cat fail\n");

zlog_fini();
return -2;

}

zlog_info(c, "hello, zlog");

zlog_fini();

return 0;

} 

дһ�������ļ������ں�test_hello.cͬ����Ŀ¼��:
$ vi test_hello.conf
[formats]

simple = "%m%n"

[rules]

my_cat.DEBUG    >stdout; simple

���롢Ȼ������!
$ cc -c -o test_hello.o test_hello.c -I/usr/local/include
$ cc -o test_hello test_hello.o -L/usr/local/lib -lzlog

$ ./test_hello

hello, zlog

3.4  ���򵥵�Hello World

���������$(top_builddir)/test/test_default.c, test_default.conf. Դ�����ǣ�

#include <stdio.h>
#include "zlog.h"

int main(int argc, char** argv)

{

int rc;
rc = dzlog_init("test_default.conf", "my_cat");

if (rc) {

printf("init failed\n");
return -1;

}

dzlog_info("hello, zlog");

zlog_fini();

return 0;

} 

�����ļ���test_default.conf����test_hello.confһģһ�������ִ�г�������Ҳһ��������������������dzlog API���ں�һ��Ĭ�ϵ�zlog_category_t�����6.5��

Chapter 4  Syslog ģ��

4.1  ����(Category)������(Rule)�͸�ʽ(Format)

zlog��3����Ҫ�ĸ������(Category)������(Rule)�͸�ʽ(Format)��

����(Category)�������ֲ�ͬ�����롣�����еķ��������������һ���ַ�������һ�������������ͨ����ȡ��ͬ�ķ�������category�������������ͬ�������־�����ڲ�ͬ��Ŀ�ġ�

��ʽ(Format)���������������־�ĸ�ʽ�������Ƿ��д���ʱ������Ƿ�����ļ�λ����Ϣ�ȣ��������������ĸ�ʽsimple���Ǽ򵥵��û��������Ϣ+���з���

����(Rule)���ǰѷ��ࡢ��������ļ�����ʽ�������������һ�������е���־�Ƿ����������������ʲô��ʽ�����

���ԣ�������ִ�����������ʱ��

zlog_category_t *c;
c = zlog_get_category("my_cat");

zlog_info(c, "hello, zlog");

zlog���ҵ�c��������"my_cat"����Ӧ�������ļ��еĹ�����

[rules]
my_cat.DEBUG    >stdout; simple

Ȼ�����飬Ŀǰ������־�ļ����Ƿ���Ϲ����еļ����������Ƿ��������ΪINFO>=DEBUG������������־�ᱻ��������Ҹ����������򣬻ᱻ�����stdout����׼����� ������ĸ�ʽ��simple���������ļ��ж�����

[formats]
simple = "%m%n" 

�������Ļ�ϴ�ӡ

hello, zlog
������������̡��û�Ҫ������д�Լ�����Ϣ����־�������������ʲô��ʽ��������ǿ�������ļ�����ɵġ�

4.2  syslogģ�ͺ�log4jģ�͵�����

�ã���ôĿǰ���ģ�ͺ�syslog��ʲô��ϵ�أ�����Ϊֹ�����ģ�ͻ��ǱȽ���log4j��log4j��ģ��������logger, appender��layout���������ڣ���log4j���棬�����е�logger�������е�logger��һһ��Ӧ�ģ�����һ��logger��Ψһ�ļ���һ��һ��ϵ��log4j, log4cxx, log4cpp, log4cplus, log4net��Ψһѡ��

������ģ���ǲ����ģ����Ƿ����˹�������filters�����ֲ�������ֻ�ܰ�����Ū�ø��ӻ��ҡ����������ǰ�Ŀ��ת��syslog��ģ�ͣ�����һ����Ƶĺܼ�����ȷ��ģ�͡�

������һ�ڵ����ӣ������zlog�������ļ�������ô2�й���

[rules]
my_cat.DEBUG     >stdout; simple

my_cat.INFO      >stdout;

Ȼ��һ�д����������������

hello, zlog
2012-05-29 10:41:36 INFO [11288:test_hello.c:41] hello, zlog

����һ�������еķ����Ӧ�����ļ��е���������log4j���û����ܻ�˵��"��ܺã�����ֻҪ��log4j���������appenderҲ������һ����"���Լ�������һ�����ӣ�

[rules]
my_cat.WARN     "/var/log/aa.log"

my_cat.DEBUG    "/var/log/bb.log"

�����ǣ�

zlog_info(c, "info, zlog");
zlog_debug(c, "debug, zlog");

�����aa.log��ֻ��һ����־

2012-05-29 10:41:36 INFO [11288:test_hello.c:41] info, zlog
����bb.log����������

2012-05-29 10:41:36 INFO [11288:test_hello.c:41] info, zlog
2012-05-29 10:41:36 DEBUG [11288:test_hello.c:42] debug, zlog

����������ܿ���������log4j�޷����׵�������һ�㡣��zlog���棬һ��������Զ�Ӧ�������ÿ���������Լ��ļ�������͸�ʽ��������û��ܰ���������ˡ�����������Լ���������־��

4.3  ��չsyslogģ��

���Ե��������ܿ�����zlog��ģ�͸���syslog��ģ�͡����ҵ��ǣ���syslog���棬��ʩ��facility���Ǹ�int�ͣ����ұ����ϵͳ������Ǽ�������ѡ��zlog�ߵ�Զһ�㣬��һ���ַ�������ʶ���ࡣ

syslog��һ��ͨ���"*"��ƥ�����е���ʩ��facility����zlog����Ҳһ����"*"ƥ�����з��ࡣ���ṩ��һ���ܷ���İ취���ض������ϵͳ�и�������Ĵ���ֻҪ��ôд��

[rules]
*.error    "/var/log/error.log"

zlogǿ������е����������¼�����ƥ�䡣�����ķ����������ģ�

c = zlog_get_category("my_cat");
Ȼ�������ļ���������

[rules]
my_cat.*      "/var/log/my_cat.log"

my_.NOTICE    "/var/log/my.log"

����������ƥ��c����"my_cat"��ͨ���"_" ��ʾ�ϼ����ࡣ "my_"��"my_cat"��"my_dog"���ϼ����ࡣ����һ��ͨ�����"!"�����5.5.2

Chapter 5  �����ļ�

�󲿷ֵ�zlog����Ϊȡ���������ļ�������־������ȥ����ʲô��ʽ����ôת���������ļ���zlog�ĺڻ����Ҿ���������ڻ���Ƶļ����ˡ����Ǹ������ļ����ӣ�

# comments
[global]

strict init = true

buffer min = 1024

buffer max = 2MB

rotate lock file = /tmp/zlog.lock

default format = "%d.%us %-6V (%c:%F:%L) - %m%n"

file perms = 600

 

[levels]

TRACE = 10

CRIT = 130, LOG_CRIT

 

[formats]

simple = "%m%n"

normal = "%d %m%n"

 

[rules]

default.*               >stdout; simple

*.*                     "%12.2E(HOME)/log/%c.log", 1MB*12; simple

my_.INFO                >stderr;

my_cat.!ERROR           "/var/log/aa.log"

my_dog.=DEBUG           >syslog, LOG_LOCAL0; simple

my_mice.*               $user_define;

�йص�λ���������ڴ��С���ߴ�����ʱ����������1k 5GB 4M�����ĵ�λ��

# 1k => 1000 bytes 
# 1kb => 1024 bytes 

# 1m => 1000000 bytes 

# 1mb => 1024*1024 bytes

# 1g => 1000000000 bytes 

# 1gb => 1024*1024*1024 byte

��λ�Ǵ�Сд�����еģ�����1GB 1Gb 1gB�ǵ�Ч�ġ�

5.1  ȫ�ֲ���

ȫ�ֲ�����[global]��ͷ��[]����һ���ڵĿ�ʼ���ĸ�С�ڵ�˳���ܱ䣬����Ϊglobal-levels-formats-rules����һ�ڿ��Ժ��Բ�д���﷨Ϊ

(key) = (value)
strict init
���"strict init"��true��zlog_init()�����ϸ������еĸ�ʽ�͹����κδ��󶼻ᵼ��zlog_init() ʧ�ܲ��ҷ���-1����"strict init"��false��ʱ��zlog_init()����Դ���ĸ�ʽ�͹��� �������Ĭ��Ϊtrue��

reload conf period
���ѡ����zlog����һ��ʱ�������Զ����������ļ������صļ����ÿ����д��־�Ĵ��������塣��д��־��������һ��ֵ���ڲ��������zlog_reload()�������ء�ÿ��zlog_reload()����zlog_init()֮�����¼����ۼӡ���Ϊzlog_reload()��ԭ���Եģ�����ʧ�ܼ����õ�ǰ��������Ϣ�������Զ������ǰ�ȫ�ġ�Ĭ��ֵ��0���Զ������ǹرյġ�

buffer min
buffer max
zlog�ڶ���Ϊÿ���߳����뻺�档"buffer min"�ǵ����������Сֵ��zlog_init()��ʱ������������ȵ��ڴ档д��־��ʱ�����������־���ȴ��ڻ��棬������Զ����䣬ֱ����"buffer max"�� ������־�ٳ�����"buffer max"�ͻᱻ�ضϡ���� "buffer max" �� 0����ζ�Ų����ƻ��棬ÿ������Ϊԭ�ȵ�2����ֱ������������������ڴ�Ϊֹ�������С���Լ��� KB, MB �� GB��Щ��λ��Ĭ����˵"buffer min"�� 1K �� "buffer max" ��2MB��

rotate lock file
���ѡ��ָ����һ�����ļ���������֤������������־��ȫת����zlog����zlog_init()ʱ���Զ�дȨ�޴�����ļ���ȷ����ִ�г�����û���Ȩ�޴����Ͷ�д����ļ���ת����־��α�����ǣ�

write(log_file, a_log)
if (log_file > 1M)

if (pthread_mutex_lock succ && fcntl_lock(lock_file) succ)
if (log_file > 1M) rotate(log_file);
fcntl_unlock(lock_file);

pthread_mutex_unlock;

mutex_lock���ڶ��̣߳� fcntl_lock���ڶ���̡�fcntl_lock��POSIX�����������man 3 fcntl���������ȫϵͳ��Ч�ġ���ĳ���������������󣬲���ϵͳ���ͷŴ˽��̳��е������������Ϊʲô��fcntl������֤��ȫת����������Ҫ�����ļ��ж�дȨ�ޡ�

Ĭ����˵��rotate lock file = self������������£�zlog���ᴴ���κ����ļ����������ļ���Ϊ���ļ���fcntl�ǽ������������û��������ɵ��޸Ĵ洢���ǵ������ļ���һ����˵��������־�ļ����ᱻ��ͬ����ϵͳ�û��Ľ���ת���������������ļ���Ϊ���ļ��ǰ�ȫ�ġ�

�������������·����Ϊ���ļ�������/tmp/zlog.lock��zlog����zlog_init()��ʱ�򴴽�����ļ�������ж������ϵͳ�û��Ľ�����Ҫת��ͬһ����־�ļ���ȷ��������ļ����ڶ���û����ɶ�д��Ĭ��ֵ��/tmp/zlog.lock��

default format
���������ȱʡ����־��ʽ��Ĭ��ֵΪ��

"%d %V [%p:%F:%L] %m%n"
���ָ�ʽ�������������������

2012-02-14 17:03:12 INFO [3758:test_hello.c:39] hello, zlog
file perms
���ָ���˴�����־�ļ���ȱʡ����Ȩ�ޡ�����ע��������Ĳ�������־�ļ���Ȩ��Ϊ"file perms"& ~umask��Ĭ��Ϊ600��ֻ����ǰ�û���д��

fsync period
��ÿ������д��һ����������־���ļ���zlog�����fsync(3)���ò���ϵͳ���ϰ�����д��Ӳ�̡�������ÿ�����򵥶�ͳ�Ƶģ�������zlog_reload()��ᱻ��0������ָ�����ǣ�����־�ļ����Ƕ�̬���ɻ��߱�ת��������£�zlog���ܱ�֤�������ļ����㶨��zlogֻfsync()�Ǹ�ʱ��ո�write()���ļ������������ṩ��д��־�ٶȺ����ݰ�ȫ��֮���ƽ�⡣���ӣ�

$ time ./test_press_zlog 1 10 100000
real	0m1.806s

user	0m3.060s 

sys	 0m0.270s

 

$ wc -l press.log  

1000000 press.log  

 

$ time ./test_press_zlog 1 10 100000 #fsync period = 1K

real	0m41.995s 

user	0m7.920s 

sys	 0m0.990s

 

$ time ./test_press_zlog 1 10 100000 #fsync period = 10K

real	0m6.856s 

user	0m4.360s 

sys	 0m0.550s

����㼫���ں���ȫ�������ٶȵĻ�����ͬ��IO�ļ�����5.5.3��Ĭ��ֵ��0���ɲ���ϵͳ������ʲôʱ��ˢ���浽�ļ���

5.2  ��־�ȼ��Զ���

��һ����[levels]��ʼ�����ڶ����û��Լ�����־�ȼ���������û��Զ������־��¼��һ��ʹ�á���һ�ڿ��Ժ��Բ�д���﷨Ϊ��

(level string) = (level int), (syslog level, optional)
(level int)������[1,253]�����Χ�ڣ�Խ��Խ��Ҫ��(syslog level)�ǿ�ѡ�ģ��������Ĭ��ΪLOG_DEBUG��

���7.3��

5.3  ��ʽ(Formats)

��һ����[formats]��ʼ������������־�ĸ�ʽ���﷨Ϊ��

(name) = "(actual formats)"
�ܺ���⣬(name)������Ĺ���ʹ�á�(name)���������ֺ���ĸ��ɣ��»���"_"Ҳ����ĸ��(actual format)ǰ����Ҫ��˫���š� (actual formats)������ת���ַ���ɣ�����һ�ڡ�

5.4  ת����ʽ��

ת����ʽ��������Ǵ�C��printf�������泭���ġ�һ��ת����ʽ�����ı��ַ���ת��˵����ɡ�

ת����ʽ�����ڹ������־�ļ�·���������ʽ(format)�С�

����԰�������ı��ַ��ŵ�ת����ʽ�����档

ÿ��ת��˵�������԰ٷֺ�(%)��ͷ�ģ��������ѡ�Ŀ�����η��������ת���ַ���β��ת���ַ����������ʲô���ݣ����������������ʱ�����ڡ����̺ŵȵȡ�������η�����������ֶε������С��ȡ����Ҷ��롣�����Ǽ򵥵����ӡ�

���ת����ʽ���ǣ�

"%d(%m-%d %T) %-5V [%p:%F:%L] %m%n".
Դ�����е�д��־����ǣ�

zlog_info(c, "hello, zlog");
���������

02-14 17:17:42 INFO  [4935:test_hello.c:39] hello, zlog
����ע�⵽�����ı��ַ���ת��˵��֮��û����ʽ�ķָ�����zlog������ʱ��֪��������ת��˵���Ŀ�ͷ�ͽ�β���������������%-5p���ת��˵����������־����Ҫ������룬ռ5���ַ���

5.4.1  ת���ַ�

���Ա����ϵ�ת���ַ���

�ַ�
Ч��
����
%c
������
aa_bb
%d()
����־��ʱ�䡣�������Ҫ��һ��С����()�ں�˵����������ڸ�ʽ������%d(%F)����%d(%m-%d %T)���������С���ţ�Ĭ����%d(%F %T)�������ڵĸ�ʽ�� strftime(2)�ĸ�ʽһ�¡����5.4.3
%d(%F) 2011-12-01
%d(%m-%d %T) 12-01 17:17:42
%d(%T) 17:17:42.035
%d 2012-02-14 17:03:12
%d()
%E()
��ȡ����������ֵ
%E(USER) simpson
%ms
���룬3λ�����ַ���
ȡ��gettimeofday(2)
013
%us
΢�룬6λ�����ַ���
ȡ��gettimeofday(2)
002323
%F
Դ�����ļ�������Դ��__FILE__�ꡣ��ĳЩ�������� __FILE__�Ǿ���·������%f��ȥ��Ŀ¼ֻ�����ļ��������߱�������ѡ����Ե���
test_hello.c
������ĳЩ��������
/home/zlog/src/test/test_hello.c
%f
Դ�����ļ��������$F���һ����/������Ĳ��֡���Ȼ�����һ����������ʧ
test_hello.c
%H
����������Դ�� gethostname(2)
zlog-dev
%L
Դ������������Դ��__LINE__��
135
%m
�û���־���û���zlog�����������־��
hello, zlog
%M
MDC (mapped diagnostic context)��ÿ���߳�һ�ż�ֵ�Ա���������Ӧ��ֵ������������һ��С����()�ں��������� %M(clientNumber) ��clientNumbe�Ǽ��� ��� 7.1
%M(clientNumber) 12345
%n
���з���Ŀǰ����֧��windows���з�
\n
%p
����ID����Դ��getpid()
2134
%U
���ú�������������__func__(C99)����__FUNCTION__(gcc)�����������֧�ֵĻ���
main
%V
��־���𣬴�д
INFO
%v
��־����Сд
info
%t
16���Ʊ�ʾ���߳�ID����Դ��pthread_self()
"0x%x",(unsigned int) pthread_t
0xba01e700
%T
�൱��%t,�������Գ����ͱ�ʾ��
"%lu", (unsigned long) pthread_t
140633234859776
%%
һ���ٷֺ�
%
%[�����ַ�]
����Ϊ����zlog_init()����ʧ��
 
 
5.4.2  ������η�

һ����˵���ݰ�ԭ����������������˿�����η������ܹ�������С�ֶο�ȡ�����ֶο�Ⱥ����Ҷ��롣��Ȼ��Ҫ����һ�������ܴ��ۡ�

��ѡ�Ŀ�����η����ڰٷֺź�ת���ַ�֮�䡣

��һ����ѡ�Ŀ�����η���������ʶ������(-)��Ȼ���ǿ�ѡ����С�ֶο�ȣ�����һ��ʮ�������ֳ�������ʾ�����м����ַ��ᱻ�����������ݱ���û����ô���ַ����������ո����������Ҷ��룩ֱ����С�ֶο��Ϊֹ��Ĭ������������Ҳ�����Ҷ��롣��Ȼ��Ҳ����ʹ��������־��ָ��Ϊ������ұ�������롣����ַ�Ϊ�ո�(space)��������ݵĿ�ȳ�����С�ֶο�ȣ��������ݵĿ���������Զ����ض����ݡ�

������Ϊ����������ֶο�����ı䡣����ֶο���Ƿ���һ������(.)�����ʮ�������ֳ�����������ݵĿ�ȳ���������ֶο�ȣ���β��������ַ�����������ֶο�ȵĲ��֣����ᱻ��ȥ�� ����ֶο����8�����ݵĿ����10������������ַ��ᱻ������������Ϊ��C��printf��һ���ģ��Ѻ���Ĳ��ֽضϡ�

�����Ǹ��ֿ�����η��ͷ���ת���ַ����һ���õ����ӡ�

������η�
�����
��С�ֶο��
����ֶο��
��ע
%20c
��
20
��
�󲹳�ո����������С��20���ַ�����
%-20c
��
20
��
�Ҳ���ո����������С��20���ַ�����
%.30c
��
��
30
�������������30���ַ�����ȡǰ30���ַ���ȥ������ġ�
%20.30c
��
20
30
���������С��20���ַ������󲹳�ո������20-30֮�䣬����ԭ��������������30���ַ�����ȡǰ30���ַ���ȥ������ġ�
%-20.30c
��
20
30
���������С��20���ַ������Ҳ���ո������20-30֮�䣬����ԭ��������������30���ַ�����ȡǰ30���ַ���ȥ������ġ�
5.4.3  ʱ���ַ�

������ת���ַ�d֧�ֵ�ʱ���ַ���

�����ַ�������strftime(2)���ɵģ����ҵ�linux����ϵͳ��֧�ֵ��ǣ�

�ַ�
Ч��
����
%a
һ�����и������д��������locale��ʾ
Wed
%A
һ�����и����ȫ��������locale��ʾ
Wednesday
%b
��д���·���������locale��ʾ
Mar
%B
�·�ȫ��������locale��ʾ
March
%c
����ʱ������ڵ�ȫ��ʾ�� ����locale��ʾ
Thu Feb 16 14:16:35 2012
%C
���� (��/100)��2λ������(SU)
20
%d
һ�����е�ĳһ�� (01-31)
06
%D
�൱��%m/%d/%y. (����������ר�ã�������Ҫ֪���ڱ�Ĺ���%d/%m/%y ����������Ҳ����˵�ڹ��ʻ����������ʽ���������⣬Ҫ����) (SU)
02/16/12
%e
����%d��һ�����е�ĳһ�죬����ͷ�ϵ�0���滻�ɿո�(SU)
6
%F
�൱��%Y-%m-%d (ISO 8601���ڸ�ʽ)(C99)
2012-02-16
%G
The ISO 8601 week-based year (see NOTES) with century as a decimal number. The 4-digit year corre�\ sponding to the ISO week number (see %V). This has the same format and value as %Y, except that if the ISO week number belongs to the previous or next year, that year is used instead. (TZ)
�����ǲ���%V������꣬��������ǰ���첻������ĵ�һ�ܣ�������һ��
2012
%g
�൱��%G�����ǲ������� (00-99). (TZ)
12
%h
�൱��%b(SU)
Feb
%H
Сʱ��24Сʱ��ʾ(00-23)
14
%I
Сʱ��12Сʱ��ʾ(01-12)
02
%j
һ���еĸ���(001-366)
047
%k
Сʱ��24Сʱ��ʾ( 0-23)�� һλ��ǰ��Ϊ�ո� (�ɺ�%H�Ƚ�) (TZ)
15
%l
Сʱ��12Сʱ��ʾ( 0-12)�� һλ��ǰ��Ϊ�ո� (�ɺ�%�Ƚ�)(TZ)
3
%m
�·�(01-12)
02
%M
����(00-59)
11
%n
���з� (SU)
\n
%p
"AM" �� "PM"�����ݵ�ʱ��ʱ�䣬����locale��ʾ��Ӧ��ֵ������"����"��"����" �� ������"PM"���賿��"AM"
PM
%P
�൱��%p������Сд������locale��ʾ��Ӧ��ֵ (GNU)
pm
%r
ʱ��+��׺AM��PM����POSIX locale���൱��%I:%M:%S %p. (SU)
03:11:54 PM
%R
Сʱ(24Сʱ��):���� (%H:%M) (SU) ���Ҫ����ģ���%T
15:11
%s
Epoch������������Ҳ���Ǵ�1970-01-01 00:00:00 UTC. (TZ)
1329376487
%S
��(00-60). (����60��Ϊ������)
54
%t
�Ʊ��tab(SU)
 
%T
Сʱ(24Сʱ��):����:�� (%H:%M:%S) (SU)
15:14:47
%u
һ�ܵ������(1-7)����һ��1�����%w (SU)
4
%U
һ���е��������(00-53)��������һ�ܵĿ�ʼ��һ���е�һ���������ڵ����ǵ�01�ܡ����%V��%W
07
%V
ISO 8601�������(01-53)��01���ǵ�һ��������4����������ܡ����%U ��%W(SU)
07
%w
һ�ܵ������(0-6)��������0�����%u
4
%W
һ���е��������(00-53)����һ��һ�ܵĿ�ʼ��һ���е�һ����һ���ڵ����ǵ�01�ܡ����%V��%W
07
%x
��ǰlocale�µ�ƫ������
02/16/12
%X
��ǰlocale�µ�ƫ��ʱ��
15:14:47
%y
����������Ŀ�����(00-99)
12
%Y
��������Ŀ�����
2012
%z
��ǰʱ�������GMTʱ���ƫ����������RFC 822-conformant������(��˵��Ҳ��֪����ɶ) (using "%a, %d %b %Y %H:%M:%S %z"). (GNU)
+0800
%Z
ʱ����(����еĻ�)
CST
%%
һ���ٷֺ�
%
 
5.5  ����(Rules)

��һ����[rules]��ͷ�������������־����ô�����ˡ���ʽ���Լ�������ġ���ڿ��Ժ��Բ�д������������û����־����ˣ��ٺ١��﷨�ǣ�

(category).(level)    (output), (options, optional); (format name, optional)
��zlog_init()�����õ�ʱ�����й��򶼻ᱻ�����ڴ��С���zlog_get_category()�����ã�����ͱ�����������ࣨ5.5.2������ʵ��д��־��ʱ������zlog_info()�����õ�ʱ�򣬾ͻ�Ƚ����INFO�͸�������ĵȼ���������������־�᲻��ͨ�����������������zlog_reload()�����õ�ʱ�������ļ��ᱻ���¶��룬�������еĹ��򣬲������¼�������Ӧ�Ĺ���

5.5.1  ����ƥ��

zlog��6��Ĭ�ϵļ���"DEBUG", "INFO", "NOTICE", "WARN", "ERROR"��"FATAL"��������������־������������ aa.DEBUG��ζ���κδ��ڵ���DEBUG�������־�ᱻ�������Ȼ���������ı��ʽ�������ļ��еļ����Ǵ�Сд�����еġ�

���ʽ	����
*	���еȼ�
aa.debug	�����ڵȼ�>=debug
aa.=debug	�����ڵȼ�==debug
aa.!debug	�����ڵȼ�!=debug
�û������Զ���ȼ������7.3��

5.5.2  ����ƥ��

������������ֺ���ĸ��ɣ��»���"_"Ҳ����ĸ��

�ܽ�
�����ļ��������
ƥ��Ĵ������
��ƥ��Ĵ������
*ƥ������
*.*
aa, aa_bb, aa_cc, xx, yy ...
NONE
��_��β�ķ���ƥ�䱾�����¼�����
aa_.*
aa, aa_bb, aa_cc, aa_bb_cc
xx, yy
����_��β�ľ�ȷƥ�������
aa.*
aa
aa_bb, aa_cc, aa_bb_cc
!ƥ����Щû���ҵ�����ķ���
!.*
xx
aa(as it matches rules above)
5.5.3  �������

Ŀǰzlog֧��������������﷨�ǣ�

[���], [����ѡ��, ��ѡ]; [format(��ʽ)��, ��ѡ]

����	����ֶ�	
����ѡ��
��׼���	>stdout	
������
��׼�������	>stderr	
������
�����syslog	>syslog	
syslog��ʩ(facilitiy)��
LOG_USER(default), LOG_LOCAL[0-7]
����
�ܵ����	|cat	
������
�ļ�	"�ļ�·��"	
�ļ�ת�������5.6
10M * 3 �� "press.#r.log"
ͬ��IO�ļ�	-"�ļ�·��"	 
�û��Զ������	$name	
"path" ��̬���߾�̬������record���
stdout, stderr, syslog
��������������ֻ��sylog�ĸ���ѡ���������岢����д�ġ�

ֵ��ע����ǣ�zlog��д��־��ʱ��������������

write(STDOUT_FILENO, zlog_buf_str(a_thread->msg_buf), zlog_buf_len(a_thread->msg_buf))
�������ĳ����Ǹ��ػ����̣���������ʱ���STDOUT_FILENO��Ҳ����1���ļ��������ص��Ļ����ᷢ��ʲô����أ�

��־�ᱻд���µ�1���ļ���������������ļ����棡���յ����ʼ���˵zlog����־д���Լ��������ļ�����ȥ�ˣ�

���ԣ�ǧ��Ҫ���ػ����̵Ĺ����������>stdout��>stderr������������Ԥ�ϵĽ���������һ��Ҫ������նˣ���"/dev/tty"���档

�ܵ����
*.*    | /usr/bin/cronolog /www/logs/example_%Y%m%d.log ; normal
����һ����zlog��������ܵ����cronolog�����ӡ�ʵ�ֵ�ԭ��ܼ򵥣���zlog_init��ʱ�����popen("/usr/bin/cronolog /www/logs/example_%Y%m%d.log", "w")������������ļ�����������дָ����ʽ����־��ʹ��cronolog�����ɰ���ָ����־Ч�ʱ�zlog�Լ��Ķ�̬·����Ч��Ҫ�ߣ���Ϊͨ���ܵ�������ÿ�δ򿪹رն�̬·�����ļ���������

[rules] 
*.*             	"press%d(%Y%m%d).log"

$ time ./test_press_zlog 1 10 100000

real	0m4.240s 

user	0m2.500s 

sys	0m5.460s 

  

[rules] 

*.*                  | /usr/bin/cronolog press%Y%m%d.log

$ time ./test_press_zlog 1 10 100000

real	0m1.911s 

user	0m1.980s 

sys	0m1.470s

������ʹ�ùܵ�Ҳ�������Ƶģ�

POSIX.1-2001��֤��д������PIPE_BUF��С��������ԭ�ӵġ�linux��PIPE_BUFΪ4096��
������־�ĳ��ȳ���PIPE_BUF��ʱ�����ж���и��ӹ�ϵ�Ľ���дͨ��zlogдͬһ���ܵ���Ҳ������zlog_init֮��fork����ӽ��̣���ʱֻ��һ��cronolog�Ľ��̼���һ���ܵ�����������־���ݿ��ܻύ��
������̷ֱ�zlog_init���������cronolog���̣�дӵ��ͬһ���ļ�·������־�ļ�����ʹ������־���Ȳ�����PIPE_BUF��Ҳ�п��ܵ�����־������Ϊcronolog�������ļ����������ģ�����֪��������־�ı߽������
���ԣ��ܽ�һ�£�ʹ�ùܵ��������������־�ļ�������ǣ�

������д��������־���Ȳ����ơ����������ڵĶ��߳�д��־��ԭ�����Ѿ���zlog��֤�ˡ�
�и��ӹ�ϵ�Ķ���̣�������־���Ȳ��ܳ���PIPE_BUF��4096��
�޸��ӹ�ϵ�Ķ����ʹ�ùܵ�ͬʱдһ����־�����۵�����־�����Ƕ��٣����п��ܵ�����־����
zlog�����ֱ���ļ�����ܱ�֤��ʹ�Ƕ���̣�ͬʱ����zlogдһ����־�ļ�Ҳ��������������¡�

�ļ�
�ļ�·��
���������·�����߾���·������˫����"������ת����ʽ�����������ļ�·���ϡ������ļ�·���� "%E(HOME)/log/out.log"����������$HOME��/home/harry������������ļ���/home/harry/log/output.log��ת����ʽ����� 5.4��

zlog���ļ����ܼ�Ϊǿ������

����������ܵ�(FIFO)�������ڵ���ǰ��mkfifo(1)����
*.*    "/tmp/pipefile"
�����NULL��Ҳ���ǲ����
*.*     "/dev/null"
���κ������������ն�
*.*     "/dev/tty"
ÿ�߳�һ����־���ڳ������е�Ŀ¼��
*.*     "%T.log"
������н��̺����ֵ���־��ÿ�죬��$HOME/logĿ¼��ÿ1GBת��һ�Σ�����5����־�ļ���
*.*     "%E(HOME)/log/aa.%p.%d(%F).log",1GB * 5
aa_���¼����࣬ÿ������һ����־
aa_.*      "/var/log/%c.log"
�ļ�ת��
�����ļ��Ĵ�С�͸�����zlog��������ֶ���ת��������־�ļ�̫���ʱ������

"%E(HOME)/log/out.log", 1M * 3 ~ "%E(HOME)/log/out.log.#r"
���������������Ǳ����zlog��ת���������5.6

ͬ��IO�ļ�
���ļ�·��ǰ����һ��"-"�ʹ���ͬ��IOѡ��ڴ��ļ�(open)��ʱ�򣬻���O_SYNCѡ��򿪣���ʱ��ÿ��д��־��������Ȳ���ϵͳ������д��Ӳ�̺�ŷ��ء����ѡ�Ϊ��ʱ��

$ time ./test_press_zlog 100 1000
real	0m0.732s

user	0m1.030s

sys	 0m1.080s

$ time ./test_press_zlog 100 1000 # synchronous I/O open

real	0m20.646s

user	0m2.570s

sys	 0m6.950s

��ʽ��
�ǿ�ѡ�ģ������д����ȫ�����������Ĭ�ϸ�ʽ��

[global]
default format = "%d(%F %T) %V [%p:%F:%L] %m%n"

�û��Զ���������7.4
5.6  �ļ�ת��

Ϊʲô��Ҫ����־�ļ�ת�������Ѿ���ʵ�ʵ����л����в�ֹһ�εĿ���������Ϊ��־�ļ����󣬵���ϵͳӲ�̱��ű������ߵ�����־�ļ��������ʹ��grepҲҪ���Ѻܶ�ʱ����Ѱ��ƥ�����־��������־ת�������ܽ������¼��ַ�ʽ��

���̶�ʱ������з���־��
���磬ÿ������һ����־

aa.2012-08-02.log 
aa.2012-08-03.log

aa.2012-08-04.log

������־�ʺϵĳ����ǣ�����Ա���֪��ÿ�����ɵ���־����Ȼ��ϣ����n����֮���ܾ�ȷ���ҳ�ĳ���������־��������־�з��������־������ɣ���εķ�������cronosplit���������������־���ݵ�ʱ���ַ��������к��ڵ��з֣��ϲ�İ취����crontab+logrotate��mv�������ƶ������Ⲣ����ȷ��������������������־���ŵ���һ����ļ�����ȥ����

��zlog���棬����������Ҫ����־ת����������ɣ��򵥵�����־�ļ�����������ʱ�������ַ������ܽ�����⣺

*.*  "aa.%d(%F).log"
������cronolog����ɣ��ٶȻ����һ��

*.*  | cronolog aa.%F.log
������־��С�з�
�����ڿ����������ʺϵĳ����ǣ������ڶ�ʱ�������ɴ�������־�����ñ༭��vi,ue���ܿ��ٴ򿪵���־��С�����޵ģ����ߴ����־����������ͬ���ģ�������־���зֿ������º���split�ȹ�������ɣ������ڿ������Ի����Ӳ��裬�������Ҳ������־������ɡ�ֵ��һ����Ǵ浵������ģʽ��nlog�����֮ΪSequence��Rolling����Sequence�����

aa.log (new) 
aa.log.2 (less new) 

aa.log.1 

aa.log.0 (old)

����Rolling�������

aa.log (new) 
aa.log.0 (less new) 

aa.log.1 

aa.log.2 (old)

����˵���ָ��ӷ����˵�ֱ����

���ֻ�����ɸ����µ��ļ���������ģ���Ҫ��־������������ɾ���ɵĹ��������ⲿ�����Ǻ����ж���Щ��־�Ǿɵġ�

��򵥵�zlog��ת������Ϊ

*.*     "aa.log", 10MB
���������Rolling�������ÿ��aa.log����10MB��ʱ�򣬻���������������

aa.log.2 -> aa.log.3
aa.log.1 -> aa.log.2

aa.log.0 -> aa.log.1

aa.log -> aa.log.0

��������ÿ���д�ĸ�������һ��

*.*     "aa.log", 10MB * 0 ~ "aa.log.#r"
���ź��һ��������ʾ�ļ��ﵽ����ʼ����ת����

�ڶ���������ʾ�������ٸ��浵�ļ���0����ɾ���κδ浵�ļ�����

������������ʾת�����ļ���������#r��ʾ�浵�ļ�����ţ�r��rolling����д�������Է�#s����sequence����д��ת���ļ����������#r����#s��

������־��С�з֣���ͬʱ����ʱ���ǩ
aa.log 
aa.log-20070305.00.log 

aa.log-20070501.00.log 

aa.log-20070501.01.log

aa.log-20071008.00.log

��������ʺ��ڳ��������־һ�㲻�Ǻ��ܹ�ע����������ĳһ����Ҫ�ҳ��������������Ȼ������������£���һ��20070501��һ����־����������ָ��ֵ������100MB������Ҫ�˻ص��ڶ���״̬�����ļ����мӺ�׺��

zlog��Ӧ��������

*.*     "aa.log", 100MB ~ "aa-%d(%Y%m%d).#2s.log"
ÿ��100MB��ʱ��ת����ת���ļ���Ҳ֧��ת���ַ������԰�ת����ʱ��ʱ�䴮��Ϊת���ļ�����һ���֡�#2s����˼����ŵĳ�������Ϊ2λ����00��ʼ��ţ�Sequenceת��������zlog��ת����ӵ�֧���ˣ�

ѹ�����ƶ���ɾ���ɵ���־
���ȣ�ѹ����Ӧ������־������ɣ���Ϊѹ������ʱ���CPU����־������������ѹ����

���ڵ�һ�ֺ͵����֣������Ϊ�򵥣�ֻҪ����ĳЩ�ļ���������޸����ڵģ�������shell�ű�+crontab���׵�ѹ�����ƶ���ɾ����

���ڵڶ��֣���ʵ���Ƿǳ���Ҫѹ����ֻ��Ҫɾ���Ϳ����ˡ�

���һ����Ҫת����ͬʱ����ѹ����ֻ��logrotate�ܸ��������Ͼ����Ƕ����ĳ�������ת��ͬʱ��ѹ���������л��������⡣

zlog���ⲿת�����ߣ�����logrotate��֧��
zlog��ת�������Ѿ���Ϊǿ�󣬵�ȻҲ�м��������zlog�޷�����ģ����簴ʱ����������ת����ת��ǰ�����һЩ���Ƶ�shell�ű���������zlog�����úͱ��Ū�ù��ڸ��Ӷ�ȱ�����С�

��ʱ����Ҳ��ϲ����һЩ�ⲿת�����ߣ�����logrotate����ɹ����������ǣ���linux����ϵͳ�£�ת��������������־�ļ�����Ӧ�ý��̻�����ԭ�����ļ�������д��־��û�취���´���־�ļ�д�µ���־����׼�������Ǹ�Ӧ�ó���һ���źţ��������´���־�ļ�������syslogd��

kill -SIGHUP ��cat /var/run/syslogd.pid��
����zlog����Ϊ�Ǹ������⣬���ʺϽ����źš�zlog�ṩ�˺����ӿ�zlog_reload()��������������������ļ������´����е���־�ļ���Ӧ�ó�����logrotate���źţ���������;��������ͻ��˵�����󣬿��Ե�����������������´����е���־�ļ���

5.7  �����ļ�����

$ zlog-chk-conf -h 
Useage: zlog-chk-conf [conf files]... 

-q,	suppress non-error message 

-h,	show help message

zlog-chk-conf ���Զ�ȡ�����ļ�������﷨��Ȼ������Ļ�������Щ�����ļ��Ƿ���ȷ���ҽ���ÿ�δ������߸Ķ�һ�������ļ�֮����һ��������ߡ����������������

$ ./zlog-chk-conf zlog.conf
03-08 15:35:44 ERROR (10595:rule.c:391) sscanf [aaa] fail, category or level is null 

03-08 15:35:44 ERROR (10595:conf.c:155) zlog_rule_new fail [aaa] 

03-08 15:35:44 ERROR (10595:conf.c:258) parse configure file[zlog.conf] line[126] fail 

03-08 15:35:44 ERROR (10595:conf.c:306) zlog_conf_read_config fail 

03-08 15:35:44 ERROR (10595:conf.c:366) zlog_conf_build fail 

03-08 15:35:44 ERROR (10595:zlog.c:66) conf_file[zlog.conf], init conf fail 

03-08 15:35:44 ERROR (10595:zlog.c:131) zlog_init_inner[zlog.conf] fail

  

---[zlog.conf] syntax error, see error message above

��������������ļ�zlog.conf��126�У��Ǵ�ġ���һ�н�һ��������[aaa]����һ����ȷ�Ĺ���

zlog-chk-conf����ͬʱ������������ļ���������

$ zlog-chk-conf zlog.conf ylog.conf 
--[zlog.conf] syntax right 

--[ylog.conf] syntax right 

Chapter 6  zlog�ӿ�(API)

zlog�����к��������̰߳�ȫ�ģ�ʹ�õ�ʱ��ֻ��Ҫ

#include "zlog.h"
6.1  ��ʼ��������

����
 
int zlog_init(const char *confpath);
int zlog_reload(const char *confpath);

void zlog_fini(void);

����
 
zlog_init()�������ļ�confpath�ж�ȡ������Ϣ���ڴ档���confpathΪNULL����Ѱ�һ�������ZLOG_CONF_PATH��ֵ��Ϊ�����ļ����������������ZLOG_CONF_PATHҲû�У�������־�����ø�ʽд����׼����ϡ�ÿ������ֻ�е�һ�ε���zlog_init()����Ч�ģ�����Ķ�����ö���ʧ�ܲ������κ����顣

zlog_reload()��confpath�������ã���������������ļ����ؼ����ڲ��ķ������ƥ�䡢�ؽ�ÿ���̵߳Ļ��桢������ԭ�е��û��Զ�����������������������ļ������ı���������������������ʹ�ô������ޡ����confpathΪNULL����������һ��zlog_init()����zlog_reload()ʹ�õ������ļ������zlog_reload()ʧ�ܣ���һ�ε�������Ȼ��Ч������zlog_reload()����ԭ���ԡ�

zlog_fini()��������zlog API������ڴ棬�ر����Ǵ򿪵��ļ���ʹ�ô������ޡ�

����ֵ
 
����ɹ���zlog_init()��zlog_reload()����0��ʧ�ܵĻ���zlog_init()��zlog_reload()����-1����ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.2  ����(Category)����

����
 
typedef struct zlog_category_s zlog_category_t;
zlog_category_t *zlog_get_category(const char *cname);

����
 
zlog_get_category()��zlog��ȫ�ַ���������ҵ����࣬�����Ժ������־�����û�еĻ����ͽ�һ����Ȼ������������еĹ���Ѱ�Һ�cnameƥ��Ĺ��򲢰󶨡�

�����ļ������еķ�����ƥ��cname�Ĺ����������£�

* ƥ������cname��
���»���_��β�ķ�����ͬʱƥ�䱾��������¼����ࡣ����aa_ƥ��aa, aa_, aa_bb, aa_bb_cc�⼸��cname��
�����»���_��β�ķ�������ȷƥ��cname������aa_bbƥ��aa_bb���cname��
! ƥ��Ŀǰ��û�й����cname��
ÿ��zlog_category_t *��Ӧ�Ĺ�����zlog_reload()��ʱ��ᱻ�Զ����¼��㡣���õ����ڴ��ͷţ�zlog_fini() ��������һ�С�

����ֵ
 
����ɹ�������zlog_category_t��ָ�롣���ʧ�ܣ�����NULL����ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.3  д��־��������

����
 
void zlog(zlog_category_t * category, 
          const char *file, size_t filelen,

          const char *func, size_t funclen, 

          long line, int level,

          const char *format, ...); 

void vzlog(zlog_category_t * category,

          const char *file, size_t filelen,

          const char *func, size_t funclen, 

          long line, int level,

          const char *format, va_list args); 

void hzlog(zlog_category_t * category,

          const char *file, size_t filelen,

          const char *func, size_t funclen, 

          long line, int level,

          const void *buf, size_t buflen); 

����
 
��3��������ʵ��д��־�ĺ�������������ݶ�Ӧ�������ļ��е�%m��category�����ڵ���zlog_get_category()��

zlog()��vzlog()����format���������printf(3)��vprintf(3)��

vzlog()�൱��zlog()��ֻ������һ��va_list���͵Ĳ���args��������һ�����Ͳ�ͬ�Ĳ�����vzlog() �ڲ�ʹ���� va_copy �꣬args��������vzlog()�󱣳ֲ��䣬���Բο�stdarg(3)��

hzlog()�е㲻һ�����������������������������Ϊbuf_len���ڴ�buf��16���Ƶ���ʽ��ʾ������

hex_buf_len=[5365]  
             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F      0123456789ABCDEF

0000000001   23 21 20 2f 62 69 6e 2f 62 61 73 68 0a 0a 23 20   #! /bin/bash..#

0000000002   74 65 73 74 5f 68 65 78 20 2d 20 74 65 6d 70 6f   test_hex - tempo

0000000003   72 61 72 79 20 77 72 61 70 70 65 72 20 73 63 72   rary wrapper scr

����file��line��дΪ__FILE__��__LINE__�������ꡣ���������ʶ��־�������﷢���ġ�����func ��дΪ__func__����__FUNCTION__�����������֧�ֵĻ��������֧�֣�����дΪ"<unkown>"��

level��һ��������Ӧ���������漸������ȡֵ��

typedef enum {                 
ZLOG_LEVEL_DEBUG = 20,
ZLOG_LEVEL_INFO = 40,

ZLOG_LEVEL_NOTICE = 60,

ZLOG_LEVEL_WARN = 80,

ZLOG_LEVEL_ERROR = 100,

ZLOG_LEVEL_FATAL = 120

} zlog_level;

ÿ���������ж�Ӧ�ĺ꣬��ʹ�á����磺

#define zlog_fatal(cat, format, args...) \         
zlog(cat, __FILE__, sizeof(__FILE__)-1, \

__func__, sizeof(__func__)-1, __LINE__, \ 

ZLOG_LEVEL_FATAL, format, ##args) 

���еĺ��б�

/* zlog macros */
zlog_fatal(cat, format, ...)

zlog_error(cat, format, ...)

zlog_warn(cat, format, ...)

zlog_notice(cat, format, ...)

zlog_info(cat, format, ...)

zlog_debug(cat, format, ...)

 

/* vzlog macros */

vzlog_fatal(cat, format, args)

vzlog_error(cat, format, args)

vzlog_warn(cat, format, args)

vzlog_notice(cat, format, args)

vzlog_info(cat, format, args)

vzlog_debug(cat, format, args)

 

/* hzlog macros */

hzlog_fatal(cat, buf, buf_len)

hzlog_error(cat, buf, buf_len)

hzlog_warn(cat, buf, buf_len)

hzlog_notice(cat, buf, buf_len)

hzlog_info(cat, buf, buf_len)

hzlog_debug(cat, buf, buf_len) 

����ֵ
 
��Щ���������ء�����д���������ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.4  MDC����

����
 
int zlog_put_mdc(const char *key, const char *value);
char *zlog_get_mdc(const char *key);

void zlog_remove_mdc(const char *key);

void zlog_clean_mdc(void);

����
 
MDC(Mapped Diagnostic Context)��һ��ÿ�߳�ӵ�еļ�-ֵ�����Ժͷ���ûʲô��ϵ��

key��value���ַ��������Ȳ��ܳ���MAXLEN_PATH(1024)���������MAXLEN_PATH(1024)�Ļ����ᱻ�ضϡ�

��ס������Ǻ��̰߳󶨵ģ�ÿ���߳����Լ��ı�������һ���߳��ڵĵ��ò���Ӱ�������̡߳�

����ֵ
 
zlog_put_mdc()�ɹ�����0��ʧ�ܷ���-1��zlog_get_mdc()�ɹ�����value��ָ�룬ʧ�ܻ���û����Ӧ��key����NULL������д���������ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.5  dzlog�ӿ�

����
 
int dzlog_init(const char *confpath, const char *cname);
int dzlog_set_category(const char *cname); 

void dzlog(const char *file, size_t filelen,

           const char *func, size_t funclen,

           long line, int level,

           const char *format, ...); 

void vdzlog(const char *file, size_t filelen,

            const char *func, size_t funclen,

            long line, int level,

            const char *format, va_list args); 

void hdzlog(const char *file, size_t filelen,

            const char *func, size_t funclen,

            long line, int level,

            const void *buf, size_t buflen);

����
 
dzlog�Ǻ��Է���(zlog_category_t)��һ���zlog�ӿڡ����������õ�һ��Ĭ�Ϸ��࣬��������������ı����¡���Щ�ӿ�Ҳ���̰߳�ȫ�ġ������˷��࣬��ζ���û�����Ҫ���Ĵ������洢������zlog_category_t���͵ı�������ȻҲ��������dzlog�ӿڵ�ͬʱ��һ���zlog�ӿں������������ˬ��

dzlog_init()��zlog_init()һ������ʼ�������Ƕ���Ҫһ��Ĭ�Ϸ�����cname�Ĳ�����zlog_reload()�� zlog_fini() ���Ժ���ǰһ��ʹ�ã�����ˢ�����ã���������

dzlog_set_category()�������ı�Ĭ�Ϸ����õġ���һ������ᱻ�滻���µġ�ͬ�����õ����ڴ��ͷŵ����⣬zlog_fini()��������

dzlog�ĺ�Ҳ������zlog.h���档���򵥵�д����

dzlog_fatal(format, ...)
dzlog_error(format, ...)

dzlog_warn(format, ...)

dzlog_notice(format, ...)

dzlog_info(format, ...)

dezlog_debug(format, ...)

 

vdzlog_fatal(format, args)

vdzlog_error(format, args)

vdzlog_warn(format, args)

vdzlog_notice(format, args)

vdzlog_info(format, args)

vdzlog_debug(format, args)

 

hdzlog_fatal(buf, buf_len)

hdzlog_error(buf, buf_len)

hdzlog_warn(buf, buf_len)

hdzlog_noticebuf, buf_len)

hdzlog_info(buf, buf_len)

hdzlog_debug(buf, buf_len)

����ֵ
 
�ɹ������dzlog_init()��dzlog_set_category()����0��ʧ�������dzlog_init()�� dzlog_set_category()����-1����ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.6  �û��Զ������

����
 
typedef struct zlog_msg_s {
        char *buf;

        size_t len;

        char *path;

} zlog_msg_t;

typedef int (*zlog_record_fn)(zlog_msg_t *msg); 

int zlog_set_record(const char *rname, zlog_record_fn record); 

����
 
zlog�����û��Զ���������������������Ҫ�󶨵�ĳ������Ĺ����ϡ����ֹ���������ǣ�

*.*     $name, "record path %c %d"; simple
zlog_set_record()���󶨶������������������$name�ģ��ᱻ�������û��Զ���������������Ϊrecord�����������ҪΪzlog_record_fn�ĸ�ʽ��

zlog_msg_t�ṹ�ĸ�����Ա�������£�

path���Թ���Ķ��ź���ַ���������ַ����ᱻ��̬�Ľ����������ǰ��path������̬�ļ�·��һ����

buf��len ��zlog��ʽ�������־��Ϣ�ͳ��ȡ�

����zlog_set_record()���İ���zlog_reload()ʹ�ú������Ч��

����ֵ
 
�ɹ������zlog_set_record()����0��ʧ�������zlog_set_record()����-1����ϸ����ᱻд���ɻ�������ZLOG_PROFILE_ERRORָ���Ĵ�����־���档

6.7  ���Ժ����

����
 
void zlog_profile(void);
����
 
��������ZLOG_PROFILE_ERRORָ��zlog����Ĵ�����־��

��������ZLOG_PROFILE_DEBUGָ��zlog����ĵ�����־��

zlog_profile()��ӡ�����ڴ��е�������Ϣ��ZLOG_PROFILE_ERROR��������ʱ�����԰�����������ļ��Ƚϣ�������û�����⡣

Chapter 7  �߽�ʹ��

7.1  MDC

MDC��ʲô����log4j�������ΪMapped Diagnostic Context���������Ǹ��ܸ��ӵļ�������ʵMDC����һ����-ֵ�Ա�һ��ĳ���������ˣ��������԰����Զ���ӡ���������߳�Ϊ�ļ�����һ���֡������ǿ�һ�����ӣ�������$(top_builddir)/test/test_mdc.c.

$ cat test_mdc.c
#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <unistd.h>

#include <string.h>

#include "zlog.h"

int main(int argc, char** argv)

{

int rc;
zlog_category_t *zc;

rc = zlog_init("test_mdc.conf");

if (rc) { 
printf("init failed\n");

return -1;

}

zc = zlog_get_category("my_cat");

if (!zc) {

printf("get cat fail\n");
zlog_fini();

return -2;

}

zlog_info(zc, "1.hello, zlog");

zlog_put_mdc("myname", "Zhang");

zlog_info(zc, "2.hello, zlog");

zlog_put_mdc("myname", "Li");

zlog_info(zc, "3.hello, zlog"); 

zlog_fini(); 

return 0;

} 

�����ļ�

$ cat test_mdc.conf
[formats]

mdc_format=     "%d(%F %X.%ms) %-6V (%c:%F:%L) [%M(myname)] - %m%n"

[rules]

*.*             >stdout; mdc_format

���

$ ./test_mdc
2012-03-12 09:26:37.740 INFO   (my_cat:test_mdc.c:47) [] - 1.hello, zlog 

2012-03-12 09:26:37.740 INFO   (my_cat:test_mdc.c:51) [Zhang] - 2.hello, zlog 

2012-03-12 09:26:37.740 INFO   (my_cat:test_mdc.c:55) [Li] - 3.hello, zlog

����Կ���zlog_put_mdc()�ڱ��������ü�"myname"��Ӧֵ"Zhang"��Ȼ���������ļ�����%M(myname)ָ��������־���ĸ�λ����Ҫ��ֵ��������ڶ��Σ���"myname"��ֵ������д��"Li"��Ȼ����־����Ҳ����Ӧ�ı仯��

MDCʲôʱ�������أ��������û���Ҫ��ͬ������־��Ϊ���ֲ�ͬ��ҵ�����ݵ�ʱ�򡣱���˵��cԴ������

zlog_put_mdc("customer_name", get_customer_name_from_db() );
zlog_info("get in"); 

zlog_info("pick product"); 

zlog_info("pay");

zlog_info("get out");

�������ļ�������

&format  "%M(customer_name) %m%n"
������ͬʱ���������û���ʱ�򣬴��������־������

Zhang get in
Li get in

Zhang pick product

Zhang pay

Li pick product

Li pay

Zhang get out

Li get out

��������Ϳ�����grep������������û�����־�ֿ�����

$ grep Zhang aa.log > Zhang.log
$ grep Li aa.log >Li.log

���ߣ���������һ��·��һ��ʼ���ļ������������֣��������ļ���

*.* "mdc_%M(customer_name).log";
��ͻ����3����־�ļ���

mdc_.log mdc_Zhang.log mdc_Li.log
����һ����·������û�֪���Լ��ڸ�ʲô��

MDC��ÿ���̶߳��еģ����Կ��԰�һЩ�߳�ר�еı������ý�ȥ���������Ϊ�������̣߳�������ת���ַ������%t���㶨��

7.2  ���zlog����

OK������Ϊֹ���Ҽٶ�zlog�Ȿ���ǲ���ë���ġ�zlog�����û�����д��־����������Աdebug���򡣵������zlog�ڲ��������أ���ô֪�����������أ������ĳ����������־����debug������־���Լ���ôdebug���𰸺ܼ򵥣�zlog���Լ�����־���������־�������־ͨ���ǹرյģ�����ͨ�������������򿪡�

$ export ZLOG_PROFILE_DEBUG=/tmp/zlog.debug.log
$ export ZLOG_PROFILE_ERROR=/tmp/zlog.error.log

�����־ֻ����������debug��error�����úû���������. ����test_hello����3.3��Ȼ��debug��־Ϊ

$ more zlog.debug.log 
03-13 09:46:56 DEBUG (7503:zlog.c:115) ------zlog_init start, compile time[Mar 13 2012 11:28:56]------ 

03-13 09:46:56 DEBUG (7503:spec.c:825) spec:[0x7fdf96b7c010][%d(%F %T)][%F %T 29][] 

03-13 09:46:56 DEBUG (7503:spec.c:825) spec:[0x7fdf96b52010][ ][ 0][] 

......

03-13 09:52:40 DEBUG (8139:zlog.c:291) ------zlog_fini end------

zlog.error.log��־û��������Ϊû�д�������

����Կ�������debug��־չʾ��zlog����ô��ʼ����������ġ�������zlog_info()ִ�е�ʱ��û����־�����������Ϊ��Ч�ʡ�

���zlog�����κ����⣬�������־��ZLOG_PROFILE_ERROR��ָ��Ĵ�����־������˵����zlog_info()����һ�������printf���﷨��

zlog_info(zc, "%l", 1);
Ȼ�����ִ�г���ZLOG_PROFILE_ERROR����־����

$ cat zlog.error.log 
03-13 10:04:58 ERROR (10102:buf.c:189) vsnprintf fail, errno[0] 

03-13 10:04:58 ERROR (10102:buf.c:191) nwrite[-1], size_left[1024], format[%l] 

03-13 10:04:58 ERROR (10102:spec.c:329) zlog_buf_vprintf maybe fail or overflow 

03-13 10:04:58 ERROR (10102:spec.c:467) a_spec->gen_buf fail 

03-13 10:04:58 ERROR (10102:format.c:160) zlog_spec_gen_msg fail 

03-13 10:04:58 ERROR (10102:rule.c:265) zlog_format_gen_msg fail 

03-13 10:04:58 ERROR (10102:category.c:164) hzb_log_rule_output fail 

03-13 10:04:58 ERROR (10102:zlog.c:632) zlog_output fail, srcfile[test_hello.c], srcline[41]

�������û�����֪��Ϊɶ�ڴ������û�в�����Ȼ��㶨������⡣

����ʱ��ϻ����һ����������ʧ��һ����˵����������������ZLOG_PROFILE_ERROR�򿪣�ZLOG_PROFILE_DEBUG�رա�

��������һ���취�����zlog�����Ƕ�֪����zlog_init()���������Ϣ�����ڴ档������д��־�Ĺ����У�����ڴ汣�ֲ��䡣����û�������Ϊĳ��ԭ����������ڴ棬��ô�ͻ�������⡣���п������ڴ��е���Ϣ�������ļ�����Ϣ��ƥ�䡣�����������һ�����������ڴ����Ϣչ�ֵ�ZLOG_PROFILE_ERRORָ��Ĵ�����־��

�����$(top_builddir)/test/test_profile.c

$ cat test_profile.c
#include <stdio.h>

#include "zlog.h"

 

int main(int argc, char** argv)

{

int rc;
rc = dzlog_init("test_profile.conf", "my_cat");

if (rc) { 

printf("init failed\n");
return -1;

}

dzlog_info("hello, zlog");

zlog_profile();

zlog_fini(); 

return 0;

} 

zlog_profile()������������������ļ��ܼ򵥡�

$ cat test_profile.conf 
[formats] 

simple = "%m%n"   

[rules]

my_cat.*                >stdout; simple

Ȼ��zlog.error.log����

$ cat /tmp/zlog.error.log
06-01 11:21:26 WARN  (7063:zlog.c:783) ------zlog_profile start------ 

06-01 11:21:26 WARN  (7063:zlog.c:784) init_flag:[1] 

06-01 11:21:26 WARN  (7063:conf.c:75) -conf[0x2333010]- 

06-01 11:21:26 WARN  (7063:conf.c:76) --global-- 

06-01 11:21:26 WARN  (7063:conf.c:77) ---file[test_profile.conf],mtime[2012-06-01 11:20:44]--- 

06-01 11:21:26 WARN  (7063:conf.c:78) ---strict init[1]--- 

06-01 11:21:26 WARN  (7063:conf.c:79) ---buffer min[1024]--- 

06-01 11:21:26 WARN  (7063:conf.c:80) ---buffer max[2097152]--- 

06-01 11:21:26 WARN  (7063:conf.c:82) ---default_format--- 

06-01 11:21:26 WARN  (7063:format.c:48) ---format[0x235ef60][default = %d(%F %T) %V [%p:%F:%L] %m%n(0x233b810)]--- 

06-01 11:21:26 WARN  (7063:conf.c:85) ---file perms[0600]--- 

06-01 11:21:26 WARN  (7063:conf.c:87) ---rotate lock file[/tmp/zlog.lock]--- 

06-01 11:21:26 WARN  (7063:rotater.c:48) --rotater[0x233b7d0][0x233b7d0,/tmp/zlog.lock,4]-- 

06-01 11:21:26 WARN  (7063:level_list.c:37) --level_list[0x2335490]-- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x23355c0][0,*,*,1,6]--- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x23375e0][20,DEBUG,debug,5,7]--- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x2339600][40,INFO,info,4,6]--- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x233b830][60,NOTICE,notice,6,5]--- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x233d850][80,WARN,warn,4,4]--- 

06-01 11:21:26 WARN  (7063:level.c:37) ---level[0x233fc80][100,ERROR,error,5,3]--- 

7.3  �û��Զ���ȼ�

�����Ұ��û��Զ���ȼ��ļ�������д������

�������ļ��ж����µĵȼ�
$ cat $(top_builddir)/test/test_level.conf
[global]

default format  =               "%V %v %m%n"

[levels]

TRACE = 30, LOG_DEBUG

[rules]

my_cat.TRACE            >stdout;

���õ�Ĭ�ϵȼ���(��Щ����Ҫд�������ļ�����)

DEBUG = 20, LOG_DEBUG
INFO = 40, LOG_INFO

NOTICE = 60, LOG_NOTICE

WARN = 80, LOG_WARNING

ERROR = 100, LOG_ERR

FATAL = 120, LOG_ALERT

UNKNOWN = 254, LOG_ERR

������zlog������һ������(30)����һ���ȼ��ַ���(TRACE)�����˵ȼ��������������λ��[1,253]֮�䣬���������ǷǷ��ġ�����Խ�����Խ��Ҫ������TRACE��DEBUG��Ҫ(30>20)����INFO�ȼ���(30<40)���������Ķ����TRACE�Ϳ���������������ļ��������ˡ�������仰��

my_cat.TRACE	>stdout; 
��ζ�ŵȼ�>=TRACE�ģ�����INFO, NOTICE, WARN, ERROR, FATAL�ᱻд����׼�����

��ʽ�����ת���ַ�%V������ȼ��ַ����Ĵ�д�����%v�����Сд�ĵȼ��ַ��������

���⣬�ڵȼ��Ķ������棬LOG_DEBUG��ָ����Ҫ�����syslog��ʱ���Զ����TRACE�ȼ�����LOG_DEBUG�����syslog��

��Դ��������ֱ�����µĵȼ�����ô���
zlog(cat, __FILE__, sizeof(__FILE__)-1, \
__func__, sizeof(__func__)-1,__LINE__, \

30, "test %d", 1);

Ϊ�˼�ʹ�ã�����һ��.hͷ�ļ�

$ cat $(top_builddir)/test/test_level.h
#ifndef __test_level_h

#define __test_level_h

 

#include "zlog.h"

 

enum {

ZLOG_LEVEL_TRACE = 30,
/* must equals conf file setting */ 

};

#define zlog_trace(cat, format, ...) \

        zlog(cat, __FILE__, sizeof(__FILE__)-1, \

        __func__, sizeof(__func__)-1, __LINE__, \

        ZLOG_LEVEL_TRACE, format, ## __VA_ARGS__) 

#endif

����zlog_trace������.c�ļ���������
$ cat $(top_builddir)/test/test_level.c
#include <stdio.h> 

#include "test_level.h"

int main(int argc, char** argv)

{

int rc;
zlog_category_t *zc;

 

rc = zlog_init("test_level.conf");

if (rc) {

printf("init failed\n");
return -1;

}

zc = zlog_get_category("my_cat");

if (!zc) {

printf("get cat fail\n");
zlog_fini();

return -2;

}

zlog_trace(zc, "hello, zlog - trace");

zlog_debug(zc, "hello, zlog - debug");

zlog_info(zc, "hello, zlog - info");

zlog_fini(); 

return 0;

} 

��������ܿ������
$ ./test_level
TRACE trace hello, zlog - trace 

INFO info hello, zlog - info 

�����������ڴ��ģ������ļ�ֻ����>=TRACE�ȼ�����־�������Ļ�ϡ�%V��%vҲ��ʾ����ȷ�Ľ����

7.4  �û��Զ������

�û��Զ��������������zlog����һЩȨ����zlogֻ����̬���ɵ�����־���ļ�·��������ô�����ת��������ȵȹ������û������Լ�����������д������ɡ�д�꺯��ֻҪ�󶨵�ĳ������Ϳ��ԡ������Ұ��û��Զ�������ļ�������д������

�������ļ����涨�����
$ cat test_record.conf
[formats]

simple = "%m%n"

[rules]

my_cat.*      $myoutput, " mypath %c %d";simple

��һ��������$myoutput����ʹ��֮
#include <stdio.h>
#include "zlog.h"

int output(zlog_msg_t *msg)

{

printf("[mystd]:[%s][%s][%ld]\n", msg->path, msg->buf, (long)msg->len);
return 0;

}

 

int main(int argc, char** argv)

{

int rc;
zlog_category_t *zc;

rc = zlog_init("test_record.conf");
if (rc) {

printf("init failed\n");

return -1;

}

zlog_set_record("myoutput", output);

zc = zlog_get_category("my_cat");

if (!zc) {

printf("get cat fail\n");
zlog_fini();

return -2;

}

zlog_info(zc, "hello, zlog");

zlog_fini();

return 0;

} 

������Ƿ����û��Զ�������ĺ��������ˣ�
$ ./test_record  
[mystd]:[ mypath my_cat 2012-07-19 12:23:08][hello, zlog

][12] 

������������msglen��12��zlog���ɵ�msg�������һ�����з���

�û��Զ���������Ըɺܶ���������飬����ĳ���û�(flw@newsmth.net)������
��־�ļ���Ϊ foo.log
��� foo.log ���� 100M��������һ�����ļ������а����ľ��� foo.log Ŀǰ������ �� foo.log ���Ϊ�գ����¿�ʼ����
��������ϴ������ļ�ʱ�Ѿ����� 5 ���ӣ���ʹ���� 100M��ҲӦ������һ�����ļ�
���ļ����ƿ����Զ��壬��������豸����Ϊǰ׺������ʱ�䴮��Ϊ��׺
��ϣ��������ļ����Ա�ѹ�����Խ�ʡ���̿ռ�����������