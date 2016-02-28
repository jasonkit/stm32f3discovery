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

	datalen := 4096
	data := make([]byte, datalen)
	rxBuf := make([]byte, datalen)
	rand.Read(data)

	recvIdx := 0

	chunkSize := 16

	go func() {
		sendIdx := 0
		for sendIdx+chunkSize <= datalen {

			//reader := bufio.NewReader(os.Stdin)
			//reader.ReadByte()

			if n, err := s.Write(data[sendIdx : sendIdx+chunkSize]); err == nil {
				if n != chunkSize {
					fmt.Printf("Incorrect sent size %d\n", n)
				}
				sendIdx += n
				fmt.Printf("Sent %v bytes | %v\n", sendIdx, data[sendIdx-chunkSize:sendIdx])
			} else {
				fmt.Printf("Send error:%v\n", err)
			}
			time.Sleep(10 * time.Millisecond)
		}
	}()

	fmt.Printf("Start\n")
	for recvIdx+chunkSize <= datalen {
		if n, err := s.Read(rxBuf[recvIdx : recvIdx+chunkSize]); err == nil {
			if n != chunkSize {
				fmt.Printf("Incorrect recv size %d\n", n)
			}
			recvIdx += n
			fmt.Printf("Recv %v bytes | %v\n", recvIdx, rxBuf[recvIdx-chunkSize:recvIdx])
		} else {
			fmt.Printf("Recv error:%v\n", err)
		}
	}
	fmt.Printf("Done\n")

	pass := true
	for i := 0; i < datalen; i++ {
		if data[i] != rxBuf[i] {
			pass = false
			break
		}
	}
	if pass {
		fmt.Printf("Passed\n")
	} else {
		fmt.Printf("Failed\n")
	}
}
