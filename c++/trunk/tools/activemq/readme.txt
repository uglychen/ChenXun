activeMQ������Ϣ����ʹ��˵����
1����Ŀ¼�°������ͺͽ��մ�������С������ֱ��� producer �� consumer �£���producer�µ��������Ϊ��
2����Ŀ¼�� ActiveMqProducer.h��ActiveMqProducer.cpp�����ļ���������Ϣ����
3����Ҫ����ActiveMqProducer������������� initProducer �� sendMessage ������ initProducer ������ʼ�� �����࣬�������brokerURIΪmq�������ĵ�ַ�Ͷ˿ڣ��������������ɺ��ԣ������õ����ٲ���˵���� sendMessage �����������õģ� topic ��������Ϣtopic�� str ����ΪҪ���͵ľ�������
4������֮ǰ�ȳ�ʼ����activemq::library::ActiveMQCPP::initializeLibrary();  ����֮��Ҫ�ر� activemq::library::ActiveMQCPP::shutdownLibrary(); �ɲμ�ActiveMQProducer.cpp�ļ����ĵ���demo
5��������AvtiveMqcpp�⼰��������CppUnit��apr��apr-util��apr-iconv��������� http://blog.csdn.net/chenxun_2010/article/details/52709277
