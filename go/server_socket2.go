package main

import (
	"fmt"
	"net"
	"os"
	"io"
	"bytes"
//	"time"
)

func checkError(err error){
	if err != nil{
		fmt.Fprintf(os.Stderr,"Fata error: %s",err.Error())
		os.Exit(1)
	}
}

func printLog(format string, args ...interface{}){
	fmt.Printf("%s",fmt.Sprintf(format, args...))
}


func handleConnect(conn net.Conn,id int){
	defer conn.Close()

	readBytes := make([]byte,100)
	
	for{
		//conn.SetReadDeadline(time.Now().Add(10*time.Second))
		
		n,err := conn.Read(readBytes)
		var datBuffer bytes.Buffer
		if err != nil{
			if err == io.EOF{
				printLog("the connection close:%s\n", err)
			}else{
				printLog("read error%s\n",err)
			}
			break
		}
		datBuffer.Write(readBytes[:n])
		printLog("%d ,received data from %s\n", id, datBuffer.String())
	}
}
	
func main(){
	id := 0
	var listener net.Listener
	listener,err := net.Listen("tcp","127.0.0.1:9999")
	checkError(err)
	defer listener.Close()

	for {
		conn,err := listener.Accept()
		if err != nil{
			printLog("Accept Error:%s\n", err)
		}
		printLog("Get a connection with a client:%s\n",conn.RemoteAddr())
		
		id++
		go handleConnect(conn,id)
	}
}


