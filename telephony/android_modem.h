/* Copyright (C) 2007-2008 The Android Open Source Project
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/
#ifndef _android_modem_h_
#define _android_modem_h_

#include "sim_card.h"
#include "sms.h"

#define IMEI_LENGTH 15 // Excluding the '\0' char.
char imei[IMEI_LENGTH + 1];
#define IMSI_LENGTH 15 // Excluding the '\0' char.
char imsi[IMSI_LENGTH + 1];
#define MCC_LENGTH 3 // Excluding the '\0' char.
char mcc[MCC_LENGTH + 1];
#define MNC_LENGTH 3 // Max length of MNC, excluding the '\0' char.
char mnc[MNC_LENGTH + 1];
#define CARRIER_NAME_LENGTH 100 // Max length of carrier name, excluding '\0'
char carrier_name[CARRIER_NAME_LENGTH + 1];
#define MAX_PHONE_NUMBER_LENGTH 20  // Excluding the '\0'char.
char phone_number[MAX_PHONE_NUMBER_LENGTH + 1];

#define DEFAULT_IMEI "0000" "0000" "0000" "000"
#define DEFAULT_MCC "310"
#define DEFAULT_MNC "260"
#define DEFAULT_IMSI DEFAULT_MCC DEFAULT_MNC "000" "000" "000"
#define DEFAULT_CARRIER_NAME "Android"
// This is not completely accurate, it should have port number on which
// emulator's QEMU port is opened as last 4 digits which defaults to 5554.
#define DEFAULT_PHONE_NUMBER "15555215554f"

/** MODEM OBJECT
 **/
typedef struct AModemRec_*    AModem;

/* a function used by the modem to send unsolicited messages to the channel controller */
typedef void (*AModemUnsolFunc)( void*  opaque, const char*  message );

extern AModem      amodem_create( int  base_port, AModemUnsolFunc  unsol_func, void*  unsol_opaque );
extern void        amodem_set_legacy( AModem  modem );
extern void        amodem_destroy( AModem  modem );

/* send a command to the modem */
extern const char*  amodem_send( AModem  modem, const char*  cmd );

/* simulate the receipt on an incoming SMS message */
extern void         amodem_receive_sms( AModem  modem, SmsPDU  pdu );

/** RADIO STATE
 **/
typedef enum {
    A_RADIO_STATE_OFF = 0,          /* Radio explictly powered off (eg CFUN=0) */
    A_RADIO_STATE_ON,               /* Radio on */
} ARadioState;

extern ARadioState  amodem_get_radio_state( AModem modem );
extern void         amodem_set_radio_state( AModem modem, ARadioState  state );

/* Set the received signal strength indicator and bit error rate */
extern void         amodem_set_signal_strength( AModem modem, int rssi, int ber );

/** SIM CARD STATUS
 **/
extern ASimCard    amodem_get_sim( AModem  modem );

/** VOICE AND DATA NETWORK REGISTRATION
 **/

/* 'stat' for +CREG/+CGREG commands */
typedef enum {
    A_REGISTRATION_UNREGISTERED = 0,
    A_REGISTRATION_HOME = 1,
    A_REGISTRATION_SEARCHING,
    A_REGISTRATION_DENIED,
    A_REGISTRATION_UNKNOWN,
    A_REGISTRATION_ROAMING
} ARegistrationState;

typedef enum {
    A_DATA_NETWORK_UNKNOWN = 0,
    A_DATA_NETWORK_GPRS,
    A_DATA_NETWORK_EDGE,
    A_DATA_NETWORK_UMTS,
    A_DATA_NETWORK_LTE,
    A_DATA_NETWORK_CDMA1X,
    A_DATA_NETWORK_EVDO, // TODO: Should REV0, REVA and REVB be added?
} ADataNetworkType;
// TODO: Merge the usage of these two structs and rename ADataNetworkType
typedef enum {
    A_TECH_GSM = 0,
    A_TECH_WCDMA,
    A_TECH_CDMA,
    A_TECH_EVDO,
    A_TECH_LTE,
    A_TECH_UNKNOWN // This must always be the last value in the enum
} AModemTech;

typedef enum {
    A_SUBSCRIPTION_NVRAM = 0,
    A_SUBSCRIPTION_RUIM,
    A_SUBSCRIPTION_UNKNOWN // This must always be the last value in the enum
} ACdmaSubscriptionSource;

typedef enum {
    A_ROAMING_PREF_HOME = 0,
    A_ROAMING_PREF_AFFILIATED,
    A_ROAMING_PREF_ANY,
    A_ROAMING_PREF_UNKNOWN // This must always be the last value in the enum
} ACdmaRoamingPref;

extern ARegistrationState  amodem_get_voice_registration( AModem  modem );
extern void                amodem_set_voice_registration( AModem  modem, ARegistrationState    state );

extern ARegistrationState  amodem_get_data_registration( AModem  modem );
extern void                amodem_set_data_registration( AModem  modem, ARegistrationState    state );
extern void                amodem_set_data_network_type( AModem  modem, ADataNetworkType   type );

extern ADataNetworkType    android_parse_network_type( const char*  speed );
extern AModemTech          android_parse_modem_tech( const char*  tech );
extern void                amodem_set_cdma_subscription_source( AModem modem, ACdmaSubscriptionSource ssource );
extern void                amodem_set_cdma_prl_version( AModem modem, int prlVersion);


/** OPERATOR NAMES
 **/
typedef enum {
    A_NAME_LONG = 0,
    A_NAME_SHORT,
    A_NAME_NUMERIC,
    A_NAME_MAX  /* don't remove */
} ANameIndex;

/* retrieve operator name into user-provided buffer. returns number of writes written, including terminating zero */
extern int   amodem_get_operator_name ( AModem  modem, ANameIndex  index, char*  buffer, int  buffer_size );

/* reset one operator name from a user-provided buffer, set buffer_size to -1 for zero-terminated strings */
extern void  amodem_set_operator_name( AModem  modem, ANameIndex  index, const char*  buffer, int  buffer_size );

/** CALL STATES
 **/

typedef enum {
    A_CALL_OUTBOUND = 0,
    A_CALL_INBOUND  = 1,
} ACallDir;

typedef enum {
    A_CALL_ACTIVE = 0,
    A_CALL_HELD,
    A_CALL_DIALING,
    A_CALL_ALERTING,
    A_CALL_INCOMING,
    A_CALL_WAITING
} ACallState;

typedef enum {
    A_CALL_VOICE = 0,
    A_CALL_DATA,
    A_CALL_FAX,
    A_CALL_UNKNOWN = 9
} ACallMode;

#define  A_CALL_NUMBER_MAX_SIZE  16

typedef struct {
    int         id;
    ACallDir    dir;
    ACallState  state;
    ACallMode   mode;
    int         multi;
    char        number[ A_CALL_NUMBER_MAX_SIZE+1 ];
} ACallRec, *ACall;

extern int    amodem_get_call_count( AModem  modem );
extern ACall  amodem_get_call( AModem  modem,  int  index );
extern ACall  amodem_find_call_by_number( AModem  modem, const char*  number );
extern int    amodem_add_inbound_call( AModem  modem, const char*  number );
extern int    amodem_update_call( AModem  modem, const char*  number, ACallState  state );
extern int    amodem_disconnect_call( AModem  modem, const char*  number );

/**/

#endif /* _android_modem_h_ */
