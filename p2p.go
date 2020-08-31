package main

import (
	"crypto/rand"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

var connection *net.UDPConn = nil
var remoteAddress *net.UDPAddr = nil

func tokenGenerator() string {
	b := make([]byte, 5)
	rand.Read(b)
	return fmt.Sprintf("%x", b)
}

func newConnection(remote *net.UDPAddr, local *net.UDPAddr) error {

	c, err := net.ListenUDP("udp", local)
	if err != nil {
		return err
	}

	log.Println("Listening on: ", c.LocalAddr().String())

	fmt.Print("Starting connection")

	i := 0

	ourToken := tokenGenerator()
	currentToken := ourToken
	for {

		c.WriteToUDP([]byte(currentToken), remote)
		if i%4 == 0 {
			fmt.Print(".")
		}

		c.SetDeadline(time.Now().Add(50 * time.Millisecond))

		buf := make([]byte, 1024)
		n, _, _ := c.ReadFromUDP(buf)
		if n > 0 {
			if strings.Contains(string(buf[0:n]), ourToken) {
				//They have recieved our communication which means their nat has a hole

				break
			}

			currentToken = ourToken + string(buf[0:n])

		}

		c.SetDeadline(time.Time{})

		i++
	}

	go func() {

		for {
			<-time.After(time.Millisecond * 100)
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
