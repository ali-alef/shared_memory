package main

import "C"
import "fmt"

func main() {
	shm := C.create_shared_memory()
	data := C.read_from_shared_memory(shm)
	fmt.Println(data)
}
