#源镜像
FROM ubuntu:latest
#作者
MAINTAINER Razil "wilsonyx19@gmail.com"
#设置工作目录
WORKDIR /

COPY ./api-server /


#RUN source /etc/profile

#暴露端口
EXPOSE 5688
#最终运行docker的命令
ENTRYPOINT  ["/api-server"]


#sudo docker build -t browserapi .

#sudo docker run -it browserapi:latest


