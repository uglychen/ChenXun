package main

import (
	"fmt"
)
	
var fibs [10]int

func fibonacci(n int)(ret int){

	if fibs[n]!=0{
		ret = fibs[n]
		return
	}
	
	if n <=1{
		ret = 1
	}else{
		fibs[n] = fibonacci(n-1) + fibonacci(n-2)	
		 ret= fibs[n]
	}
	return ret 

}	
	
func main(){
		var result int= 0
		var i int = 0
		for ;i<10;i++{
			result=fibonacci(i)
			fmt.Printf("fibonaccif (%d) is: %d\n",i,result)
		}
}

