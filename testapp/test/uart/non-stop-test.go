package main

import (
	"crypto/rand"
	"fmt"
	"time"

	"github.com/tarm/serial"
)

func main() {
	cfg := &serial.Config{Name: "/dev/tty.SLAB_USBtoUART", Baud: 115200}
	s, err := serial.OpenPort(cfg)

	if err != nil {
		fmt.Printf("%v\n", err)
		return
	}

	chunkSize := 16

	go func() {
		data := make([]byte, chunkSize)
		sendIdx := 0
		for {
			rand.Read(data)
			if n, err := s.Write(data); err == nil {
				if n != chunkSize {
					fmt.Printf("Incorrect sent size %d\n", n)
				}
				sendIdx += n
				fmt.Printf("Sent %v bytes | %v\n", sendIdx, data)
			} else {
				fmt.Printf("Send error:%v\n", err)
			}
			time.Sleep(2 * time.Millisecond)
		}
	}()

	fmt.Printf("Start\n")
	rxBuf := make([]byte, chunkSize)
	recvIdx := 0
	for {
		if n, err := s.Read(rxBuf); err == nil {
			if n != chunkSize {
				fmt.Printf("Incorrect recv size %d\n", n)
			}
			recvIdx += n
			fmt.Printf("Recv %v bytes | %v\n", recvIdx, rxBuf)
		} else {
			fmt.Printf("Recv error:%v\n", err)
		}
	}
}
