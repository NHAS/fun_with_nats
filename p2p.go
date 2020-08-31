package main

import (
	"fmt"
	"log"
	"net"
	"time"
)

var connection *net.UDPConn = nil
var remoteAddress *net.UDPAddr = nil

func newConnection(remote *net.UDPAddr, local *net.UDPAddr) error {

	c, err := net.ListenUDP("udp", local)
	if err != nil {
		return err
	}
	defer c.Close()

	fmt.Print("Starting connection")
	i := 0
	for {
		buf := make([]byte, 1024)

		c.SetDeadline(time.Now().Add(100 * time.Millisecond))
		n, _, _ := c.ReadFromUDP(buf)
		if n > 0 {
			log.Println("Startup successful, got data")
			break
		}
		c.SetDeadline(time.Time{})

		c.WriteToUDP([]byte("|heartbeat|"), remote)
		if i%4 == 0 {
			fmt.Print(".")
		}

		i++
	}

	go func() {

		for {
			<-time.After(time.Millisecond * 200)
			c.WriteToUDP([]byte("|heartbeat|"), remote)
		}
	}()

	fmt.Print("Connection Established!\n")

	connection = c
	remoteAddress = remote
	return nil
}

func readData(output chan<- string) {
	if connection == nil {
		panic("Tried to start reading before connection was started")
	}

	go func() {
		for {
			buf := make([]byte, 1024)
			n, _, err := connection.ReadFromUDP(buf)
			if err != nil {
				continue
			}
			output <- string(buf[0:n])
		}
	}()
}

func writeData(data string) {
	if connection == nil || remoteAddress == nil {
		panic("Tried to call write before connection was established")
	}

	connection.WriteToUDP([]byte(data), remoteAddress)
}
