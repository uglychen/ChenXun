activeMQ推送消息工具使用说明：
1，本目录下包含推送和接收处理两个小组件，分别在 producer 和 consumer 下，以producer下的推送组件为例
2，本目录下 ActiveMqProducer.h和ActiveMqProducer.cpp两个文件是推送消息的类
3，主要调用ActiveMqProducer类的两个函数： initProducer 和 sendMessage ，其中 initProducer 函数初始化 推送类，传入参数brokerURI为mq服务器的地址和端口，其余两个参数可忽略，后续用到了再补充说明， sendMessage 是推送数据用的， topic 参数是消息topic， str 参数为要推送的具体内容
4，调用之前先初始化：activemq::library::ActiveMQCPP::initializeLibrary();  调用之后，要关闭 activemq::library::ActiveMQCPP::shutdownLibrary(); 可参见ActiveMQProducer.cpp文件最后的调用demo
5，依赖：AvtiveMqcpp库及其依赖包CppUnit、apr、apr-util和apr-iconv，具体详见 http://blog.csdn.net/chenxun_2010/article/details/52709277
