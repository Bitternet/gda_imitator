#ifndef REQUEST_H
#define REQUEST_H

#include    <QtGlobal>
#include    <QIODevice>

/*!
 * \struct
 * \brief Request data structure
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct request_t
{
    quint8      id;
    quint8      func;
    quint16     address;
    quint16     count;
    quint8      *data;
    quint16     crc;

    request_t()
    {
        id = 0;
        func = 0;
        address = 0;
        count = 0;
        data = nullptr;
        crc = 0;
    }
};

/*!
 * \enum
 * \brief Modbus functions codes
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
enum
{
    MB_FUNC_NONE = 0x00,
    MB_FUNC_READ_COILS = 0x01,
    MB_FUNC_READ_DISCRETE_INPUTS = 0x02,
    MB_FUNC_READ_HOLDING_REGISTERS = 0x03,
    MB_FUNC_READ_INPUT_REGISTERS = 0x04,
    MB_FUNC_WRITE_COIL = 0x05,
    MB_FUNC_WRITE_HOLDING_REGISTER = 0x06,
    MB_FUNC_WRITE_MULTIPLE_COILS = 0x0F,
    MB_FUNC_WRITE_MULTIPLE_REGISTERS = 0x10
};

/*!
 * \enum
 * \brief
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
enum
{
    ID   = 0,
    FUNC = 1,
    HI_ADDRESS = 2,
    LO_ADDRESS = 3,
    HI_COUNT =  4,
    LO_COUNT = 5,
    DATA = 6
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
enum
{
    BYTE_SIZE = 8
};

/*!
 * \fn
 * \brief Get high byte from word
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint8  hiByte(quint16 word);

/*!
 * \fn
 * \brief Get low byte from word
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint8  loByte(quint16 word);

/*!
 * \fn
 * \brief Construct word from two bytes
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint16 word(quint8 hiByte, quint8 loByte);

/*!
 * \fn
 * \brief Set bit in byte
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void setBit(quint8 &byte, quint8 bit);

/*!
 *
 *
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool getBit(quint8 byte, quint8 bit);

/*!
 * \fn
 * \brief CRC16 calculation
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint16 calcCRC16(quint8 *buff, quint8 size);

/*!
 * \fn
 * \brief CRC16 calculation simple method
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint16 calcCRC16simple(quint8 *buff, quint8 size);

/*!
 * \fn
 * \brief CRC16 calculation table method
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
quint16 calcCRC16table(quint8 *buff, quint8 size);

quint32 fromIODevice( QIODevice * device );

quint32 fromByteArray( const QByteArray & array );

#endif // REQUEST_H
