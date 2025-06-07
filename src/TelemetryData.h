#ifndef TELEMETRY_DATA_H
#define TELEMETRY_DATA_H

#include <stdint.h>
#include <cstring>
#include <stdio.h>
// #include <Arduino.h>

#define DECIMAL_POINTS 1
#define MAX_COMMAND_ECHO_LENGTH 16


typedef struct { 
    int16_t x_pitch;
    int16_t y_roll;
    int16_t z_yaw;
} Axis_Data_Compact_t;

class Time_Data
{
public:
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
    void to_string(char* buffer) { sprintf(buffer, "%02d:%02d:%02d", hh, mm, ss); }
    Time_Data() : hh(0), mm(0), ss(0) {} // Default constructor
    Time_Data(uint8_t hour, uint8_t minute, uint8_t second)
        : hh(hour), mm(minute), ss(second) {} // Parameterized constructor
};

class Axis_Data
{ 
public:
    float x_pitch;
    float y_roll;
    float z_yaw;
    void to_string(char* buffer) { sprintf(buffer, "%0.2f,%0.2f,%0.2f", x_pitch, y_roll, z_yaw); }
    Axis_Data() : x_pitch(0), y_roll(0), z_yaw(0) {} // Default constructor
    Axis_Data(float pitch, float roll, float yaw)
        : x_pitch(pitch), y_roll(roll), z_yaw(yaw) {} // Parameterized constructor
};

typedef struct TelemetryDataCompact {
    Time_Data mission_time_rtc;
    uint16_t packet_count;
    uint8_t mode;
    uint8_t state;  
    uint16_t altitude;          // -300.0 a 6253.5 m    // NECESITA CONVERSION CON OFFSET
    uint16_t temperature;       // 0.0 a 6553.5 °C 
    uint16_t pressure;          // 0.0 a 6553.5 hPa
    uint8_t voltage;            // 0.0 a 25.5 V
    Axis_Data_Compact_t gyro;   // -3270.0 a 3270.0 °/s
    Axis_Data_Compact_t accel;
    Axis_Data_Compact_t mag;    // -327.67 a 327.67 gauss
    uint16_t ag1_rot_rate;      // igual
    uint16_t ag2_rot_rate;  // ESTE AL FINAL
    Time_Data gps_time;
    uint16_t gps_altitude;  // -300.0 a 6253.5 m
    float gps_latitude;
    float gps_longitude;
    uint8_t gps_sats;
    char cmd_echo[MAX_COMMAND_ECHO_LENGTH]; // Buffer for command echo
} TelemetryDataCompact_t;
#define TELEMETRY_DATA_COMPACT_SIZE sizeof(TelemetryDataCompact_t)

typedef struct TelemetryData {
    Time_Data rtc;
    uint16_t packet_count;
    bool mode;
    char* state_name_ptr; // Pointer to state name string
    uint8_t state_id; // FSM state ID
    float altitude;
    float temperature;
    float pressure;
    float voltage;
    Axis_Data gyro;
    Axis_Data accel;
    Axis_Data mag;
    uint16_t ag_rate;
    uint16_t ag_rate_2;  // ESTE AL FINAL
    Time_Data gps_time;
    float gps_alt;
    float gps_lat;
    float gps_long;
    uint8_t gps_sats;
    char* cmd_echo_ptr; // Buffer for command echo
} TelemetryData_t;
#define TELEMETRY_DATA_SIZE sizeof(TelemetryData_t)


class TelemetryDataHandler {
public:
    TelemetryDataHandler();

    /**
     * @brief Write the telemetry data to a RAW byte array.
     * @param destination Pointer to the byte array where the data will be written.
     */
    void write_bytes(uint8_t* destination);

    /**
     * @brief Read the telemetry data from a byte array.
     */
    void read_bytes(uint8_t* buffer_src);

    /**
     * @brief Format the telemetry data as a CSV string.
     * @param buffer Pointer to the buffer where the formatted string will be stored.
     * @param size Size of the buffer.
     * @return The number of characters written to the buffer.
     */
    unsigned int format_string(char* buffer, size_t size);

    /**
     * @brief Converts the data member (TelemetryData_t) to compact_data (TelemetryDataCompact_t)
     */
    void to_compact();

    /**
     * @brief Converts the compact_data member (TelemetryDataCompact_t) to user-friendly data (TelemetryData_t).
     */
    void to_user_friendly();

    TelemetryData_t data; // user friendly data structure
    TelemetryDataCompact_t compact_data; // compact data structure

    /**
     * @brief Get the size of the compact telemetry data structure to write in memory.
     * @return The size in bytes of the compact telemetry data structure.
     */
    inline unsigned int compact_size() const {
        return TELEMETRY_DATA_COMPACT_SIZE;
    }

private:

};

 /*
TEAM_ID,                    3165,       //  [const]     Team ID
MISSION_TIME,               01:22:10,   //  (3 bytes)   RTC time, UTC [hh:mm:ss]
PACKET_COUNT,               50,         //  (2 bytes)   packet count
MODE,                       F,          //  (1 byte)    'F' or 'S' flight/sim mode
STATE,                      ASCENT,     //  (1 byte)    FSM state
ALTITUDE,                   500.3,      //  (2 bytes)   Altitude %.1f [m]
TEMPERATURE,                25.7,       //  (2 bytes)   Temp [°C]
PRESSURE,                   101.2,      //  (2 bytes)   Air pressure [kPa]
VOLTAGE,                    8.3,        //  (1 byte)    Battery Voltage [V]
GYRO_R,                     18,         //  (2 bytes)   Gyro Roll   [°/s]
GYRO_P,                     21,         //  (2 bytes)   Gyro Pitch  [°/s]
GYRO_Y,                     20,         //  (2 bytes)   Gyro Yaw    [°/s]
ACCEL_R,                    30,         //  (2 bytes)   Accel Roll  [°/s]
ACCEL_P,                    35,         //  (2 bytes)   Accel Pitch [°/s]
ACCEL_Y,                    33,         //  (2 bytes)   Accel Yaw   [°/s]
MAG_R,                      0.22,       //  (2 bytes)   Mag Roll    [gauss]
MAG_P,                      0.03,       //  (2 bytes)   Mag Pitch   [gauss]
MAG_Y,                      0.09,       //  (2 bytes)   Mag Yaw     [gauss]
AUTO_GYRO_ROTATION_RATE,    2165,       //  (2 byte)    Rotation rate [°/s] for auto-rotation
GPS_TIME,                   13:14:02,   //  (3 bytes)   GPS time, UTC [hh:mm:ss]
GPS_ALTITUDE,               200.8,      //  (2 bytes)   GPS Altitude %.1f [m]
GPS_LATITUDE,               3.8793,     //  (4 bytes)   GPS Latitude in degrees
GPS_LONGITUDE,              18.3672,    //  (4 bytes)   GPS Longitude in degrees
GPS_SATS,                   5,          //  (1 byte)    Num of Sats tracked by GPS
CMD_ECHO,                   CXON,       //  (N bytes)   Text of the last command received and processed
*/

#endif // TelemetryData.h