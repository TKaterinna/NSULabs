package main

import (
    "encoding/binary"
    "io"
	"log"
	"net"
	"os"
    "sync"
    "strconv"
)

const (
	SOCKS5Version = 0x05
	NoAuth        = 0x00
	ConnectCmd    = 0x01
	IPv4          = 0x01
	Domain        = 0x03
	SuccessReply  = 0x00
    NoAcceptable  = 0xFF
    RSV           = 0x00

    RequestGranted = 0x00
    GeneralFailure = 0x01
    ConnectionNotAllowedByRuleset = 0x02
    NetworkUnreachable = 0x03
    HostUnreachable = 0x04
    ConnectionRefusedByDestinationHost = 0x05
    TTLExpired = 0x06
    CommandNotSupported = 0x07
    AddressTypeNotSupported = 0x08
)

func main() {
	if len(os.Args) != 2 {
		log.Fatalf("Usage: %s <port>", os.Args[0])
	}

	strPort := os.Args[1]
    port, _ := strconv.Atoi(strPort)
	listener, err := net.ListenTCP("tcp4", &net.TCPAddr{Port: port})
	if err != nil {
		log.Fatalf("Failed to start listener: %v", err)
	}

	log.Printf("SOCKS5 proxy listening on port %s", strPort)

	for {
        conn, err := listener.AcceptTCP()
		if err != nil {
			log.Printf("Failed to accept connection: %v", err)
			continue
		}
		go handleClient(conn)
	}
}

func handleClient(clientConn *net.TCPConn) {
	defer clientConn.Close()

	// SOCKS5 handshake
	versionBuf := make([]byte, 1)
	_, err := clientConn.Read(versionBuf)
	if err != nil {
		log.Printf("Failed to read handshake: %v", err)
		return
	}

	if versionBuf[0] != SOCKS5Version {
		log.Printf("Unsupported SOCKS version: %d", versionBuf[0])
		return
	}
    log.Printf("SOCKS version: %d", versionBuf[0])

    nauthBuf := make([]byte, 1)
	_, err = clientConn.Read(nauthBuf)
    if err != nil {
		log.Printf("Failed to read handshake: %v", err)
		return
	}

    authBuf := make([]byte, nauthBuf[0])
    _, err = clientConn.Read(authBuf)
    if err != nil {
		log.Printf("Failed to read handshake: %v", err)
		return
	}

	authMethod := NoAcceptable
	for _, method := range authBuf {
		if method == NoAuth {
			authMethod = NoAuth
			break
		}
	}

    _, err = clientConn.Write([]byte{SOCKS5Version, byte(authMethod)})
	if err != nil {
		log.Printf("Failed to send auth method: %v", err)
		return
	}

    if authMethod == NoAcceptable {
        log.Printf("Acceptable only \"No authentication\" method")
        return
    }

	// SOCKS5 CONNECT
	_, err = clientConn.Read(versionBuf)
	if err != nil {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Failed to read connect: %v", err)
		return
	}

	if versionBuf[0] != SOCKS5Version {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Unsupported SOCKS version: %d", versionBuf[0])
		return
	}

    cmdBuf := make([]byte, 1)
	_, err = clientConn.Read(cmdBuf)
    if err != nil {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Failed to read connect: %v", err)
		return
	}

    if cmdBuf[0] != ConnectCmd {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Unsupported command: %d", cmdBuf[0])
		return
	}

    rsvBuf := make([]byte, 1)
	_, err = clientConn.Read(rsvBuf)
    if err != nil {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Failed to read connect: %v", err)
		return
	}

    if rsvBuf[0] != RSV {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Reserved must be 0x00, but rsv = %d", rsvBuf[0])
		return
	}

    addrTypeBuf := make([]byte, 1)
	_, err = clientConn.Read(addrTypeBuf)
	if err != nil {
        sendAfterConnection(clientConn, CommandNotSupported)
		log.Printf("Failed to read command: %v", err)
		return
	}

	var dstIp net.IP
    var dstDomain string
	var dstPort uint16
    var serverConn *net.TCPConn
    var status byte

	switch addrTypeBuf[0] {
	case IPv4:
		dstIp, dstPort, err = getIPv4(clientConn)
        if err != nil {
            sendAfterConnection(clientConn, CommandNotSupported)
            return
        }
        serverConn, status, err = getDestConn(dstIp, dstPort)
        if err != nil {
            sendAfterConnection(clientConn, status)
            log.Printf("Failed to connect to server: %v", err)
            return
        }
	case Domain:
        dstDomain, dstPort, err = getDomain(clientConn)
        if err != nil {
            sendAfterConnection(clientConn, CommandNotSupported)
            return
        }
        serverConn, status, err = resolveDomain(dstDomain, dstPort)
        if err != nil {
            sendAfterConnection(clientConn, status)
            return
        }
	default:
		log.Printf("Unsupported address type: %d", addrTypeBuf[0])
        sendAfterConnection(clientConn, AddressTypeNotSupported)
		return
	}
	defer serverConn.Close()

    sendAfterConnection(clientConn, status)

    // SOCKS5 transfer data
    transferData(clientConn, serverConn)
}

func sendAfterConnection(conn net.Conn, status byte) {
    replyMsg := []byte{
		SOCKS5Version, status, RSV, IPv4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
    _, err := conn.Write(replyMsg)
	if err != nil {
		log.Printf("Failed to send after connection: %v", err)
		return
	}
}

func getIPv4(conn net.Conn) ([]byte, uint16, error) {
    ipBuf := make([]byte, 4)
	_, err := conn.Read(ipBuf)
    if err != nil {
		log.Printf("Failed to read ip: %v", err)
		return nil, 0, err
	}

    portBuf := make([]byte, 2)
    _, err = conn.Read(portBuf)
    if err != nil {
		log.Printf("Failed to read port: %v", err)
		return nil, 0, err
	}
	port := uint16(binary.BigEndian.Uint16(portBuf))

    return ipBuf, port, nil
}

func getDomain(conn net.Conn) (string, uint16, error) {
    lenBuf := make([]byte, 1)
	_, err := conn.Read(lenBuf)
    if err != nil {
		log.Printf("Failed to read domain: %v", err)
		return "", 0, err
	}

    domainBuf := make([]byte, lenBuf[0])
    _, err = conn.Read(domainBuf)
    if err != nil {
		log.Printf("Failed to read domain: %v", err)
		return "", 0, err
	}
    domain := string(domainBuf)

    portBuf := make([]byte, 2)
    _, err = conn.Read(portBuf)
    if err != nil {
		log.Printf("Failed to read port: %v", err)
		return "", 0, err
	}
	port := uint16(binary.BigEndian.Uint16(portBuf))

    return domain, port, nil
}

func getDestConn(ip net.IP, port uint16) (*net.TCPConn, byte, error) {
    addr := net.TCPAddr{IP: ip, Port: int(port)}
    server, err := net.DialTCP("tcp4", nil, &addr)

	if err != nil {
		if opErr, ok := err.(*net.OpError); ok {
			if opErr.Temporary() {
				return nil, TTLExpired, err
			}
			if opErr.Err.Error() == "network is unreachable" {
				return nil, NetworkUnreachable, err
			}
			if opErr.Err.Error() == "no route to host" {
				return nil, HostUnreachable, err
			}
			if opErr.Err.Error() == "connection refused" {
				return nil, ConnectionRefusedByDestinationHost, err
			}
		}
		return nil, GeneralFailure, err
	}

    return server, RequestGranted, err
}

func resolveDomain(domain string, port uint16) (*net.TCPConn, byte, error) {
    ips, err := net.LookupIP(domain)
    if err != nil {
        log.Printf("Failed to resolve domain: %v", err)
		return nil, HostUnreachable, err
    }

    for _, ip := range ips {
        if ipv4 := ip.To4(); ipv4 != nil {
            server, status, err := getDestConn(ipv4, port)
            if err == nil {
                return server, status, nil
            }
        }
    }
    return nil, HostUnreachable, nil
}

func transferData(client *net.TCPConn, server *net.TCPConn) {
	var wg sync.WaitGroup
	wg.Add(2)

	go copyData(client, server, &wg)
	go copyData(server, client, &wg)

	wg.Wait()
}


func copyData(dest *net.TCPConn, src *net.TCPConn, wg *sync.WaitGroup) {
	defer wg.Done()
	defer func(dest *net.TCPConn) { _ = dest.CloseWrite() }(dest)

    _, err := io.Copy(dest, src)
    if err != nil {
        log.Printf("ERROR in copy data: %v", err)
    }
}