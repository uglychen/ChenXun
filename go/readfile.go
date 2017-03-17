package main  
  
import (  
    "fmt"  
    "os"  
    "bufio"  
    "io"  
    "io/ioutil"  
)  
  
func readFile(path string){  
    file,err := os.Open(path)  
    if err != nil{  
        fmt.Printf("read file error\n")  
        return  
    }  
      
    defer file.Close()  
      
    inputReader := bufio.NewReader(file)  
    for{  
        str,err :=inputReader.ReadString('\n')  
          
        if err == io.EOF{  
            fmt.Printf("read eof")  
        }  
          
        if err != nil{  
            return //err or EOF  
        }  
          
        fmt.Printf("the input is:%s",str)  
    }  
}  
  
func main(){  
      
    readFile("C:/Users/chen/Desktop/myTest.go")  
      
    buf, _ := ioutil.ReadFile("C:/Users/chen/Desktop/myTest.go")  
    fmt.Printf("use io.ioutil read file:%s",buf)  
  
}  

[cpp] view plain copy 在CODE上查看代码片派生到我的代码片
package main  
  
import(  
    "fmt"  
    "os"  
    "bufio"  
)  
  
func readFile(path string){  
      
    input,_  := os.Open(path)  
      
    reader := bufio.NewReader(input)  
      
    buff := make([]byte,1)  
      
    for {  
        n,_ := reader.Read(buff)  
        if n == 0{  
            break  
        }  
          
        fmt.Printf("%s",string(buff[0:n]))  
    }  
  
}  
  
func main(){  
  
    readFile("C:/Users/Administrator/Desktop/chenxun.txt")  
  
}

/*
package main

import(
	"fmt"
	"os"
	"bufio"
)

func readFile(path string){
	
	input,_  := os.Open(path)
	
	defer input.Close()
	
	reader := bufio.NewReader(input)
	
	buff := make([]byte,1)
	
	for {
		n,_ := reader.Read(buff)
		if n == 0{
			break
		}
		
		fmt.Printf("%s",string(buff[0:n]))
	}

}

func main(){

	readFile("C:/Users/Administrator/Desktop/chenxun.txt")

}*/
