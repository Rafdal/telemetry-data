#include "TelemetryData.h"

TelemetryDataHandler::TelemetryDataHandler() {}

// TODO: Implementar
void TelemetryDataHandler::write_bytes(uint8_t* destination) {
    // este buffer viene en el formato grande, la idea es pasarlo a la estructura compacta.
    // Se debe hacer una conversion de tipos y escalas.
    to_compact(); // Convert the data to compact format
    memcpy(destination, &compact_data, TELEMETRY_DATA_COMPACT_SIZE); // Copy the compact data to the destination buffer
}

// TODO: Implementar
void TelemetryDataHandler::read_bytes(uint8_t* buffer_src) {
    // este buffer viene en el formato compacto, la idea es pasarlo a la estructura grande.
    memcpy(&compact_data, buffer_src, TELEMETRY_DATA_COMPACT_SIZE); // Copy the compact data from the source buffer
    to_user_friendly(); // Convert the compact data to user-friendly format
}

unsigned int TelemetryDataHandler::format_string(char* buffer, size_t buffer_size) {
    memset(buffer, 0, buffer_size); // Clear the buffer

    char mode_value = data.mode ? 'S' : 'F';

    strcat(buffer, "3165,");
    data.rtc.to_string(buffer + strlen(buffer));
    sprintf(buffer + strlen(buffer), ",%u,%c,%s", data.packet_count, mode_value, data.state_name_ptr);
    sprintf(buffer + strlen(buffer), ",%0.1f,%0.1f,%0.1f,%0.1f,", data.altitude, data.temperature, data.pressure, data.voltage);
    data.gyro.to_string(buffer + strlen(buffer));
    strcat(buffer, ",");
    data.accel.to_string(buffer + strlen(buffer));
    strcat(buffer, ",");
    data.mag.to_string(buffer + strlen(buffer));
    sprintf(buffer + strlen(buffer), ",%u,", data.ag_rate);
    data.gps_time.to_string(buffer + strlen(buffer));
    sprintf(buffer + strlen(buffer), ",%0.1f,%0.4f,%0.4f,%u,", data.gps_alt, data.gps_lat, data.gps_long, data.gps_sats);
    if (data.cmd_echo_ptr) {
        strncat(buffer, data.cmd_echo_ptr, MAX_COMMAND_ECHO_LENGTH);
    } else {
        strcat(buffer, "N/A");
    }
    sprintf(buffer + strlen(buffer), ",,%u", data.ag_rate_2);
    return strlen(buffer);
}

void TelemetryDataHandler::to_compact() {
    // mission_time_rtc: same type and layout
    compact_data.mission_time_rtc = data.rtc;

    // packet_count, mode, state: same type and layout
    compact_data.packet_count = data.packet_count;
    compact_data.mode = data.mode;
    compact_data.state = data.state_id;

    // altitude, temperature, pressure, voltage: type changes (float -> uint16_t/uint8_t)
    compact_data.altitude = static_cast<int16_t>((300.0f + data.altitude)*10.0f);
    compact_data.temperature = static_cast<uint16_t>(data.temperature*10.0f);
    compact_data.pressure = static_cast<uint16_t>(data.pressure*10.0f);
    compact_data.voltage = static_cast<uint8_t>(data.voltage*10.0f);

    // Gyro, Accel, Mag: type changes (Axis_Data_t -> Axis_Data_Compact_t)
    compact_data.gyro.x_pitch = static_cast<int16_t>(data.gyro.x_pitch*10.0f);
    compact_data.gyro.y_roll  = static_cast<int16_t>(data.gyro.y_roll*10.0f);
    compact_data.gyro.z_yaw   = static_cast<int16_t>(data.gyro.z_yaw*10.0f);

    compact_data.accel.x_pitch = static_cast<int16_t>(data.accel.x_pitch*10.0f);
    compact_data.accel.y_roll  = static_cast<int16_t>(data.accel.y_roll*10.0f);
    compact_data.accel.z_yaw   = static_cast<int16_t>(data.accel.z_yaw*10.0f);

    compact_data.mag.x_pitch = static_cast<int16_t>(data.mag.x_pitch*100.0f);
    compact_data.mag.y_roll  = static_cast<int16_t>(data.mag.y_roll*100.0f);
    compact_data.mag.z_yaw   = static_cast<int16_t>(data.mag.z_yaw*100.0f);

    // ag1_rot_rate, ag2_rot_rate: same type and layout
    compact_data.ag1_rot_rate = data.ag_rate;
    compact_data.ag2_rot_rate = data.ag_rate_2;

    // gps_time: same type and layout
    compact_data.gps_time = data.gps_time;

    // gps_altitude: type changes (float -> uint16_t)  // Las dos toemdas ya esta soldada


    compact_data.gps_altitude = static_cast<uint16_t>(data.gps_alt*10.0f);

    compact_data.gps_latitude = data.gps_lat;
    compact_data.gps_longitude = data.gps_long;
    compact_data.gps_sats = data.gps_sats;

    strncpy(compact_data.cmd_echo, data.cmd_echo_ptr, MAX_COMMAND_ECHO_LENGTH);
}

void TelemetryDataHandler::to_user_friendly() {
    // mission_time_rtc: same type and layout
    data.rtc = compact_data.mission_time_rtc;
// 
    // packet_count, mode, state: same type and layout
    data.packet_count = compact_data.packet_count;
    data.mode = compact_data.mode;
    data.state_id = compact_data.state;

    // altitude, temperature, pressure, voltage: type changes (uint16_t/uint8_t -> float)
    data.altitude = static_cast<float>(compact_data.altitude) / (10.0f) - 300.0f;
    data.temperature = static_cast<float>(compact_data.temperature) / (10.0f);;
    data.pressure = static_cast<float>(compact_data.pressure) / (10.0f);
    data.voltage = static_cast<float>(compact_data.voltage) / (10.0f);

    // Gyro, Accel, Mag: type changes (Axis_Data_Compact_t -> Axis_Data_t)
    data.gyro.x_pitch = static_cast<float>(compact_data.gyro.x_pitch) / (10.0f);
    data.gyro.y_roll  = static_cast<float>(compact_data.gyro.y_roll)  / (10.0f);
    data.gyro.z_yaw   = static_cast<float>(compact_data.gyro.z_yaw)   / (10.0f);

    data.accel.x_pitch = static_cast<float>(compact_data.accel.x_pitch) / (10.0f);
    data.accel.y_roll  = static_cast<float>(compact_data.accel.y_roll)  / (10.0f);
    data.accel.z_yaw   = static_cast<float>(compact_data.accel.z_yaw)   / (10.0f);

    data.mag.x_pitch = static_cast<float>(compact_data.mag.x_pitch) / (100.0f);
    data.mag.y_roll  = static_cast<float>(compact_data.mag.y_roll)  / (100.0f);
    data.mag.z_yaw   = static_cast<float>(compact_data.mag.z_yaw)   / (100.0f);

    // ag1_rot_rate, ag2_rot_rate: same type and layout
    data.ag_rate = compact_data.ag1_rot_rate;
    data.ag_rate_2 = compact_data.ag2_rot_rate;

    // gps_time: same type and layout
    data.gps_time = compact_data.gps_time;

    // gps_altitude: type changes (uint16_t -> float)
    data.gps_alt = static_cast<float>(compact_data.gps_altitude) / (10.0f * DECIMAL_POINTS);

    // gps_latitude, gps_longitude: same type and layout (float)
    data.gps_lat = compact_data.gps_latitude;
    data.gps_long = compact_data.gps_longitude;

    // gps_sats: same type and layout
    data.gps_sats = compact_data.gps_sats;

    // cmd_echo: char array, safe copy
    // memset(compact_data.cmd_echo, 0, MAX_COMMAND_ECHO_LENGTH);
    strncpy(data.cmd_echo_ptr, compact_data.cmd_echo, MAX_COMMAND_ECHO_LENGTH - 1);
}