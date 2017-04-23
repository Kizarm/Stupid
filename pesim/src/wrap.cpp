#include "iogbl.h"
#include "wrap.h"
#include "../../simple/llvm/PesApi.h"

QtWrap::QtWrap (QObject * parent) : QObject (parent) {
  port   = 4747;
  socket = new QUdpSocket (parent);
  if (socket->bind (port, QUdpSocket::ShareAddress)) {
    printf ("bind ok\n");
  }
  connect (socket, SIGNAL (readyRead()), this, SLOT (readPendingDatagrams()));
}
QtWrap::~QtWrap () {
  delete socket;
}
unsigned QtWrap::send (const char * data, unsigned len) {
  unsigned n = socket->writeDatagram(data, len, QHostAddress::Broadcast, port);
  //printf("send %d\n", n);
  return n;
}
void QtWrap::readPendingDatagrams (void) {
  //printf("pending\n");
  while (socket->hasPendingDatagrams()) {
    QByteArray datagram;
    datagram.resize (socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram (datagram.data(), datagram.size(),
                              &sender, &senderPort);
    unsigned size = datagram.size();
    char * sdata = new char [size+1];
    memcpy (sdata, datagram.data(), size);
    sdata[size] = '\0';
    rcvd  (sdata, size);
    //printf("Rcvd: %d : %s\n", size, sdata);
    delete [] sdata;
  }
}
uint16_t swap_bytes (uint16_t word) {
  uint16_t result = (word & 0xFF) << 8;
  result += (word >> 8) & 0xFF;
  return result;
}

unsigned int QtWrap::rcvd (const char* data, unsigned int) {
  unsigned naddr, addr, value; 
  unsigned n = sscanf(data, "#%02X:%02X%04X\r\n", &naddr, &addr, &value);
  if (n!=3)          return 0; // chyba
  unsigned hdr = GIODescriptor.NetAddr;
  if (naddr == hdr) return 0;
  // printf ("N=%d, %02X:%04X:%04X\n", n, naddr, addr, value);
  Variables.Words[addr] = /*swap_bytes*/ (value);
  return 0;
}
