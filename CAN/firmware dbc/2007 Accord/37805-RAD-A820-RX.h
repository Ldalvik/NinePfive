// CLUSTER_DATA_1
void parse_0x0D4(const uint8_t *d)
{
    uint8_t  CRUISE_BUTTON             = (d[0] >> 6);                            // 0=none, 1=SET/DECEL, 2=RESUME/ACCEL, 3=CANCEL
    bool     CRUISE_MASTER_MAIN_SW     = (d[0] & 0x20) != 0;
    bool     PARKING_BRAKE_APPLIED     = (d[0] & 0x04) != 0;
    uint8_t  LIGHTING_MODE             = (d[0] & 0x03);                          // 0=off, 1=parking, 2=low beam, 3=high beam
    bool     OIL_WARNING               = (d[2] & 0x80) != 0;
    bool     AC_SWITCH                = (d[2] & 0x40) != 0;                      // A/C clutch compressor
    uint8_t  FUEL_SENDER               = d[3];
    float    FUEL_LEVEL_SENSOR_VOLTAGE = d[4] * (5.0f / 256.0f); 
    float    SECONDARY_VEHICLE_SPEED   = (((uint32_t)d[5] << 8) | d[6]) * 0.01f; // km/h
    uint8_t  COUNTER                   = (d[7] >> 6);
    uint8_t  CHECKSUM                  = (d[7] & 0x0F);
}

// WHEEL_SPEEDS
void parse_0x1C0(const uint8_t *d)
{
    float   WHEEL_SPEED_FRONT_LEFT  = (((uint32_t)d[0] << 7) | (d[1] >> 1)) * 0.01f;                                   // km/h
    float   WHEEL_SPEED_FRONT_RIGHT = (((uint32_t)(d[1] & 0x01) << 14) | ((uint32_t)d[2] << 6) | (d[3] >> 2)) * 0.01f; // km/h
    float   WHEEL_SPEED_REAR_LEFT   = (((uint32_t)(d[3] & 0x03) << 13) | ((uint32_t)d[4] << 5) | (d[5] >> 3)) * 0.01f; // km/h
    float   WHEEL_SPEED_REAR_RIGHT  = (((uint32_t)(d[5] & 0x07) << 12) | ((uint32_t)d[6] << 4) | (d[7] >> 4)) * 0.01f; // km/h
    uint8_t CHECKSUM                = (d[7] & 0x0F);
}

// CLUSTER_DATA_2
void parse_0x1F4(const uint8_t *d)
{
    bool    RIGHT_TURN_SIGNAL      = (d[0] & 0x40) != 0;
    bool    LEFT_TURN_SIGNAL       = (d[0] & 0x20) != 0;
    uint8_t WIPER_MODE             = ((d[0] >> 3) & 0x03);                                     // 0=off, 1=intermittent, 2=low, 3=high
    bool    ABS_STARTUP_WARNING    = (d[0] & 0x02) != 0;
    bool    DIMMER_OVERRIDE        = (d[1] & 0x80) != 0;                                       // possible maximum-brightness detent/night-dimming cancel
    uint8_t DIMMER_LEVEL           = (d[1] & 0x7F);                                            // possible instrument-illumination brightness, values 24,48,77,100
    double  ODOMETER               = (((uint32_t)d[3] << 16) | ((uint32_t)d[4] << 8) | d[5]);  // km 
    bool    OIL_LIFE_RESET_REQUEST = (d[6] & 0x01) != 0;
    uint8_t COUNTER                = (d[7] >> 6);
}

// EXTERNAL_THROTTLE_REQUEST
void parse_0x064(const uint8_t *d)
{
    float   EXTERNAL_THROTTLE_REQUEST = (((uint32_t)d[0] << 2) | (d[1] >> 6)) * 0.1f; // deg
    uint8_t THROTTLE_REQUEST_MODE     = ((d[1] >> 4) & 0x03);                         // 0=inactive, 1=minimum, 2=maximum, 3=inactive
    uint8_t EXTERNAL_CONTROL_STATE    = (d[1] & 0x0F);                               
    bool    CRUISE_INHIBIT_INPUT      = (d[2] & 0x01) != 0;
    uint8_t COUNTER                   = (d[7] >> 6);
    uint8_t CHECKSUM                  = (d[7] & 0x0F);
}

// CRUISE_CONTROL_DATA_1
// DISABLED IN THIS FIRMWARE
void parse_0x0A0(const uint8_t *d)
{
    bool    TARGET_RATE_CORRECTION_INHIBIT = (d[2] & 0x80) != 0;
    bool    CRUISE_SPEED_LIMIT_ENABLE      = (d[2] & 0x40) != 0;   // applies in cruise mode 0
    uint8_t EXTERNAL_CRUISE_MODE           = ((d[2] >> 4) & 0x03); // 0=external variant0; 1=external variant1; 2=ordinary cruise; 3=invalid or fallback
    bool    CRUISE_LATCHED_INHIBIT_INPUT   = (d[2] & 0x04) != 0;
    bool    EXTERNAL_BRAKE_INPUT           = (d[2] & 0x01) != 0;
    bool    CRUISE_AT_SHIFT_ASSIST_INPUT   = (d[3] & 0x10) != 0;
    uint8_t COUNTER                        = (d[7] >> 6);
    uint8_t CHECKSUM                       = (d[7] & 0x0F);
}

// CRUISE_CONTROL_DATA_2
// DISABLED IN THIS FIRMWARE
void parse_0x280(const uint8_t *d)
{
    float   EXTERNAL_CRUISE_SPEED_LIMIT = (((uint32_t)d[0] << 8) | d[1]) * 0.01f; // km/h
    float   EXTERNAL_CRUISE_GAIN        = d[2] * 0.005f;
    uint8_t COUNTER                     = (d[7] >> 6);
    uint8_t CHECKSUM                    = (d[7] & 0x0F);
}

// MISC
// DISABLED IN THIS FIRMWARE
void parse_0x429(const uint8_t *d)
{
    uint8_t  MODEL_SECONDARY_INPUT = d[0];
    uint8_t  THRESHOLD_PROFILE     = d[1];
    bool     RESET_INPUT           = (d[2] & 0x80) != 0;
    uint32_t MODEL_PRIMARY_INPUT   = ((uint32_t)d[3] << 16) | ((uint32_t)d[4] << 8) | d[5];
    uint8_t  COUNTER               = (d[7] >> 6);
    uint8_t  CHECKSUM              = (d[7] & 0x0F);
}
