
#ifndef RTUMASTER_H
#define RTUMASTER_H

#include "Arduino.h"
#include "mbTCPslave.h"


#define RTU_BUFFER_SIZE  264




class RtuMaster
{

public:
  RtuMaster();
  ~RtuMaster(void);
  void task(void);

private:
  uint16_t calculateCRC(uint8_t * buff, uint16_t len);
  uint16_t CRC16(uint8_t *puchMsg, uint16_t usDataLen);
};

extern  RtuMaster _RtuMaster;
#endif //RTUMASTER_H
