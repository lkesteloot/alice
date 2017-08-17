#include "event_service.h"
#include "connection.h"

int32_t events_get_valuator(int32_t device)
{
    send_uint8(COMMAND_GET_VALUATOR);
    send_uint32(device);
    return receive_uint32();
}

void events_qdevice(int32_t device)
{
    send_uint8(COMMAND_QDEVICE);
    send_uint32(device);
}

uint32_t event_qread_start(int blocking)
{
    send_uint8(COMMAND_QREAD);
    send_uint8(blocking ? 1 : 0);
    int count = receive_uint8();
    return count;
}

int32_t events_qread_continue(int16_t *value)
{
    int32_t device = receive_uint32();
    *value = receive_uint16();
    return device;
}

int32_t events_winopen(char *title)
{
    return network_winopen(title);
}

void events_tie(int32_t button, int32_t val1, int32_t val2)
{
    send_uint8(COMMAND_TIE);
    send_uint32(button);
    send_uint32(val1);
    send_uint32(val2);
}
