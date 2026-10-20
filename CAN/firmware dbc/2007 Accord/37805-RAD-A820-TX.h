// ENGINE_DATA_1
void parse_0x0A6(const uint8_t *d)
{
    bool    FUEL_CUT_ACTIVE            = (d[0] & 0x02) != 0; 
    bool    PCM_DTC_FLAG               = (d[1] & 0x04) != 0; 
    float   THROTTLE_BASE_TARGET       = d[2] * 0.4f; // deg, pedal/cruise target before rate limits and clamps
    float   THROTTLE_VLV               = d[3] * 0.4f; // degrees, raw 255 may be upper clipped
    float   THROTTLE_TARGET_ABOVE_IDLE = d[4] * 0.4f; // degrees, THROTTLE_BASE_TARGET minus Idle Target TH
    uint8_t COUNTER                    = (d[7] >> 6);
    uint8_t CHECKSUM                   = (d[7] & 0x0F);
}

// ENGINE_DATA_2
void parse_0x0C8(const uint8_t *d)
{
    float    VEHICLE_SPEED     = (((uint32_t)d[0] << 8) | d[1]) * 0.01f; // km/h
    float    ALT_VEHICLE_SPEED = (((uint32_t)d[4] << 8) | d[5]) * 0.01f; // km/h
    uint16_t DISTANCE_TRAVELED = d[6] * 10;                              // meters, overflows 255 -> 0
    uint8_t  COUNTER           = (d[7] >> 6);
    uint8_t  CHECKSUM          = (d[7] & 0x0F);
}

// ENGINE_DATA_3
void parse_0x12C(const uint8_t *d)
{
    int16_t  ECT_SENSOR_1         = (int32_t)d[0] - 40;           // deg C, raw 255 = invalid
    int16_t  IAT_SENSOR_1         = (int32_t)d[1] - 40;           // deg C, raw 255 = invalid
    uint16_t ENGINE_SPEED         = ((uint32_t)d[4] << 8) | d[5]; // rpm
    bool     MIL                  = (d[6] & 0x10) != 0;           // check-engine lamp
    bool     CRUISE_INDICATOR     = (d[6] & 0x08) != 0; 
    bool     CRUISE_CONTROL_FAULT = (d[6] & 0x04) != 0;           // throttle failsafe/cruise set-speed fault
    bool     BRAKE_SWITCH         = (d[6] & 0x01) != 0; 
    uint8_t  COUNTER              = (d[7] >> 6);
    uint8_t  CHECKSUM             = (d[7] & 0x0F);
}

// TRANSMISSIONA_DATA
void parse_0x188(const uint8_t *d)
{
    uint8_t REPORTED_GEAR              = d[0];                 // 0=Unavailable, 1, 2, 3, 4, 5, 6=Reverse, 7=Transition
    uint8_t MANUAL_GEAR_DISPLAY        = ((d[1] >> 3) & 0x07); // 0=Inactive, 1, 2, 3, 4, 5, 7=Unavailable
    uint8_t SHIFT_CONTROL              = (d[1] & 0x07);        // 0=P/N, 1=1st, 2=2nd, 3=3rd, 4=4th, 5=5th, 6=R
    bool    D_INDICATOR                = (d[2] & 0x80) != 0; 
    bool    TRANSMISSION_FAULT_LATCHED = (d[2] & 0x40) != 0; 
    uint8_t GEAR_SELECTOR              = d[3];                 // 0=BetweenOrInvalid, 1=P, 2=R, 4=N, 8=D, 32=D3, 64=2, 128=1
    uint8_t COUNTER                    = (d[7] >> 6);
    uint8_t CHECKSUM                   = (d[7] & 0x0F);
}

// ENGINE_DATA_4
void parse_0x20C(const uint8_t *d)
{
    bool     EVAP_MONITOR_CONTINUE    = (d[0] & 0x02) != 0; 
    bool     EVAP_LEAK_CHECK_FUEL_CAP = (d[0] & 0x01) != 0; 
    uint8_t  ENGINE_OIL_LIFE          = d[1];                         // %
    uint16_t ENGINE_SPEED             = ((uint32_t)d[4] << 8) | d[5]; // rpm
    uint8_t  COUNTER                  = (d[7] >> 6);
    uint8_t  CHECKSUM                 = (d[7] & 0x0F);
}

// ENGINE_DATA_5
void parse_0x224(const uint8_t *d)
{
    float   BARO_SENSOR                    = d[1] * 0.357105f + 12.3347525f;             // kPa, 0=lower clipped, 255=fault or upper saturation
    float   FUEL_CONSUMED                  = (((uint32_t)d[2] << 8) | d[3]) * 0.105585f; // mL, wraps 65536
    bool    CHARGING_WARNING               = (d[6] & 0x02) != 0;                         // 0=OFF, 1=ON
    bool    SERVICE_CHECK_SIGNAL           = (d[6] & 0x01) != 0;                         // 0=OPEN, 1=SHORT
    uint8_t COUNTER                        = (d[7] >> 6); 
    bool    PROGRAMMING_HANDSHAKE_COMPLETE = (d[7] & 0x10) != 0;                         // PGMFI diagnostic handshake
    uint8_t CHECKSUM                       = (d[7] & 0x0F);
}
