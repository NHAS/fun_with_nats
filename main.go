package main

import (
	"flag"
	"fmt"
	"log"
	"net"
)

func check(reason string, err error) {
	if err != nil {
		log.Fatalln(reason, " [", err.Error(), "]")
	}
}

func main() {

	raddr := flag.String("raddr", "", "Remote Address")
	port := flag.Int("port", 5580, "Port (optional)")

	flag.Parse()
	// listen to incoming udp packets

	if *raddr == "" {
		log.Fatalln("Please enter a remote address")
	}

	log.SetFlags(log.Flags() &^ (log.Ldate | log.Ltime))

	remoteAddress := net.UDPAddr{IP: net.ParseIP(*raddr), Port: *port}

	laddr, _ := net.ResolveUDPAddr("udp", ":"+fmt.Sprintf("%d", port))

	err := newConnection(&remoteAddress, laddr)
	check("Setting up connection failed", err)

	drawchat()

}
