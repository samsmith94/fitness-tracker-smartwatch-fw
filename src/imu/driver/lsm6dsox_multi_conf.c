#include "lsm6dsox_multi_conf.h"


static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;


uint16_t steps;
uint8_t mlc_out[8];

const ucf_line_t lsm6dsox_yoga_pose_recognition[] = {
  {.address = 0x10, .data = 0x00,},
  {.address = 0x11, .data = 0x00,},
  {.address = 0x01, .data = 0x80,},
  {.address = 0x05, .data = 0x00,},
  {.address = 0x17, .data = 0x40,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xEA,},
  {.address = 0x09, .data = 0x4C,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xEB,},
  {.address = 0x09, .data = 0x03,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xEC,},
  {.address = 0x09, .data = 0x56,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xED,},
  {.address = 0x09, .data = 0x03,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xEE,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xEF,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xF0,},
  {.address = 0x09, .data = 0x0A,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xF2,},
  {.address = 0x09, .data = 0x34,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFA,},
  {.address = 0x09, .data = 0x3C,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFB,},
  {.address = 0x09, .data = 0x03,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFC,},
  {.address = 0x09, .data = 0x5C,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFD,},
  {.address = 0x09, .data = 0x03,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFE,},
  {.address = 0x09, .data = 0x68,},
  {.address = 0x02, .data = 0x11,},
  {.address = 0x08, .data = 0xFF,},
  {.address = 0x09, .data = 0x03,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x3C,},
  {.address = 0x09, .data = 0x3F,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x3D,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x3E,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x3F,},
  {.address = 0x09, .data = 0x04,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x40,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x41,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x42,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x43,},
  {.address = 0x09, .data = 0x08,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x44,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x45,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x46,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x47,},
  {.address = 0x09, .data = 0x0C,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x48,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x49,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x4A,},
  {.address = 0x09, .data = 0x1F,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x4B,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x5C,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x5D,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x5E,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x5F,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x60,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x61,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x62,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x63,},
  {.address = 0x09, .data = 0x40,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x64,},
  {.address = 0x09, .data = 0xC8,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x65,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x66,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x01, .data = 0x00,},
  {.address = 0x12, .data = 0x00,},
  {.address = 0x01, .data = 0x80,},
  {.address = 0x17, .data = 0x40,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x68,},
  {.address = 0x09, .data = 0xA1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x69,},
  {.address = 0x09, .data = 0xB9,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6A,},
  {.address = 0x09, .data = 0x0D,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6B,},
  {.address = 0x09, .data = 0x80,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6C,},
  {.address = 0x09, .data = 0x99,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6D,},
  {.address = 0x09, .data = 0xBA,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6E,},
  {.address = 0x09, .data = 0x0C,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x6F,},
  {.address = 0x09, .data = 0x80,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x70,},
  {.address = 0x09, .data = 0x31,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x71,},
  {.address = 0x09, .data = 0x30,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x72,},
  {.address = 0x09, .data = 0x20,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x73,},
  {.address = 0x09, .data = 0xA2,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x74,},
  {.address = 0x09, .data = 0xF2,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x75,},
  {.address = 0x09, .data = 0xBB,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x76,},
  {.address = 0x09, .data = 0x90,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x77,},
  {.address = 0x09, .data = 0xA0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x78,},
  {.address = 0x09, .data = 0xFD,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x79,},
  {.address = 0x09, .data = 0x28,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7A,},
  {.address = 0x09, .data = 0x90,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7B,},
  {.address = 0x09, .data = 0xA1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7C,},
  {.address = 0x09, .data = 0x27,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7D,},
  {.address = 0x09, .data = 0xB0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7E,},
  {.address = 0x09, .data = 0x69,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x7F,},
  {.address = 0x09, .data = 0xC1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x80,},
  {.address = 0x09, .data = 0x17,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x81,},
  {.address = 0x09, .data = 0xBC,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x82,},
  {.address = 0x09, .data = 0x79,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x83,},
  {.address = 0x09, .data = 0xC0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x84,},
  {.address = 0x09, .data = 0xE3,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x85,},
  {.address = 0x09, .data = 0xAF,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x86,},
  {.address = 0x09, .data = 0x0A,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x87,},
  {.address = 0x09, .data = 0x82,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x88,},
  {.address = 0x09, .data = 0x00,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x89,},
  {.address = 0x09, .data = 0xBC,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8A,},
  {.address = 0x09, .data = 0x99,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8B,},
  {.address = 0x09, .data = 0xC0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8C,},
  {.address = 0x09, .data = 0x15,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8D,},
  {.address = 0x09, .data = 0x20,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8E,},
  {.address = 0x09, .data = 0xA9,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x8F,},
  {.address = 0x09, .data = 0xE1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x90,},
  {.address = 0x09, .data = 0x10,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x91,},
  {.address = 0x09, .data = 0xBC,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x92,},
  {.address = 0x09, .data = 0xA0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x93,},
  {.address = 0x09, .data = 0xA0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x94,},
  {.address = 0x09, .data = 0x12,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x95,},
  {.address = 0x09, .data = 0xBC,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x96,},
  {.address = 0x09, .data = 0x9A,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x97,},
  {.address = 0x09, .data = 0xE0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x98,},
  {.address = 0x09, .data = 0x18,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x99,},
  {.address = 0x09, .data = 0xB9,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9A,},
  {.address = 0x09, .data = 0xB5,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9B,},
  {.address = 0x09, .data = 0xE2,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9C,},
  {.address = 0x09, .data = 0x72,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9D,},
  {.address = 0x09, .data = 0xBA,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9E,},
  {.address = 0x09, .data = 0x12,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0x9F,},
  {.address = 0x09, .data = 0x82,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA0,},
  {.address = 0x09, .data = 0x93,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA1,},
  {.address = 0x09, .data = 0xA3,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA2,},
  {.address = 0x09, .data = 0x11,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA3,},
  {.address = 0x09, .data = 0x80,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA4,},
  {.address = 0x09, .data = 0xD5,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA5,},
  {.address = 0x09, .data = 0xAF,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA6,},
  {.address = 0x09, .data = 0x0C,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA7,},
  {.address = 0x09, .data = 0xC1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA8,},
  {.address = 0x09, .data = 0xF4,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xA9,},
  {.address = 0x09, .data = 0x26,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAA,},
  {.address = 0x09, .data = 0x47,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAB,},
  {.address = 0x09, .data = 0xE1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAC,},
  {.address = 0x09, .data = 0xC8,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAD,},
  {.address = 0x09, .data = 0xAE,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAE,},
  {.address = 0x09, .data = 0x34,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xAF,},
  {.address = 0x09, .data = 0xE1,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB0,},
  {.address = 0x09, .data = 0x06,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB1,},
  {.address = 0x09, .data = 0x39,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB2,},
  {.address = 0x09, .data = 0x14,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB3,},
  {.address = 0x09, .data = 0x82,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB4,},
  {.address = 0x09, .data = 0xEA,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB5,},
  {.address = 0x09, .data = 0xB4,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB6,},
  {.address = 0x09, .data = 0x16,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB7,},
  {.address = 0x09, .data = 0xE0,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB8,},
  {.address = 0x09, .data = 0xA6,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xB9,},
  {.address = 0x09, .data = 0x3B,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBA,},
  {.address = 0x09, .data = 0x80,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBB,},
  {.address = 0x09, .data = 0xA2,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBC,},
  {.address = 0x09, .data = 0x12,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBD,},
  {.address = 0x09, .data = 0xB5,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBE,},
  {.address = 0x09, .data = 0x0D,},
  {.address = 0x02, .data = 0x31,},
  {.address = 0x08, .data = 0xBF,},
  {.address = 0x09, .data = 0xE0,},
  {.address = 0x01, .data = 0x80,},
  {.address = 0x17, .data = 0x00,},
  {.address = 0x04, .data = 0x00,},
  {.address = 0x05, .data = 0x10,},
  {.address = 0x02, .data = 0x01,},
  {.address = 0x01, .data = 0x00,},
  {.address = 0x12, .data = 0x44,},
  {.address = 0x01, .data = 0x80,},
  {.address = 0x60, .data = 0x35,},
  {.address = 0x01, .data = 0x00,},
  {.address = 0x10, .data = 0x40,},
  {.address = 0x11, .data = 0x00,},
  {.address = 0x5E, .data = 0x02,},
  {.address = 0x01, .data = 0x80,},
  {.address = 0x0D, .data = 0x01,},
  {.address = 0x01, .data = 0x00,}
};

void lsm6dsox_multi_conf_init(void)
{
    /* Variable declaration */
    //lsm6dsox_pin_int2_route_t pin_int2_route;
    lsm6dsox_pin_int1_route_t pin_int1_route;

    lsm6dsox_emb_sens_t emb_sens;
    
    uint32_t i;
    
    /* Initialize mems driver interface */
    g_dev_ctx.write_reg = platform_write;
    g_dev_ctx.read_reg = platform_read;
    g_dev_ctx.handle = &imu_m_twi;

    /* Init test platform */
    platform_init();

    /* Wait sensor boot time */
    platform_delay(10);

    /* Check device ID */
    
    lsm6dsox_device_id_get(&g_dev_ctx, &whoamI);
    platform_delay(10);
    NRF_LOG_INFO("LSM6DSOX device id: %d", whoamI);
    if (whoamI != LSM6DSOX_ID)
        while (1)
            ;

    /* Restore default configuration */
    lsm6dsox_reset_set(&g_dev_ctx, PROPERTY_ENABLE);
    do
    {
        lsm6dsox_reset_get(&g_dev_ctx, &rst);
    } while (rst);

    /* Start Machine Learning Core configuration ******************************/
    /**************************************************************************/
    for (i = 0; i < (sizeof(lsm6dsox_yoga_pose_recognition) /
                     sizeof(ucf_line_t));
         i++)
    {

        lsm6dsox_write_reg(&g_dev_ctx, lsm6dsox_yoga_pose_recognition[i].address,
                           (uint8_t *)&lsm6dsox_yoga_pose_recognition[i].data, 1);
    }
    /* End Machine Learning Core configuration */

    /* At this point the device is ready to run but if you need you can also
   * interact with the device but taking in account the MLC configuration.
   *
   * For more information about Machine Learning Core tool please refer
   * to AN5259 "LSM6DSOX: Machine Learning Core".
   */

    /* Turn off embedded features */
    lsm6dsox_embedded_sens_get(&g_dev_ctx, &emb_sens);
    lsm6dsox_embedded_sens_off(&g_dev_ctx);
    platform_delay(10);

    /* Turn off Sensors */
    lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_OFF);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_OFF);

    /* Disable I3C interface */
    lsm6dsox_i3c_disable_set(&g_dev_ctx, LSM6DSOX_I3C_DISABLE);

    /* Enable Block Data Update */
    lsm6dsox_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);

    /* Set full scale */
    lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_4g);
    lsm6dsox_gy_full_scale_set(&g_dev_ctx, LSM6DSOX_2000dps);

    /* Enable Tap detection on X, Y, Z */
    lsm6dsox_tap_detection_on_z_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_tap_detection_on_y_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_tap_detection_on_x_set(&g_dev_ctx, PROPERTY_ENABLE);

    /* Set Tap threshold to 01000b, therefore the tap threshold
   * is 500 mg (= 12 * FS_XL / 32 )
   */
    lsm6dsox_tap_threshold_x_set(&g_dev_ctx, 0x04);
    lsm6dsox_tap_threshold_y_set(&g_dev_ctx, 0x04);
    lsm6dsox_tap_threshold_z_set(&g_dev_ctx, 0x04);
    /* Configure Single and Double Tap parameter
   *
   * For the maximum time between two consecutive detected taps, the DUR
   * field of the INT_DUR2 register is set to 0111b, therefore the Duration
   * time is 538.5 ms (= 7 * 32 * ODR_XL)
   *
   * The SHOCK field of the INT_DUR2 register is set to 11b, therefore
   * the Shock time is 57.36 ms (= 3 * 8 * ODR_XL)
   *
   * The QUIET field of the INT_DUR2 register is set to 11b, therefore
   * the Quiet time is 28.68 ms (= 3 * 4 * ODR_XL)
   */
    lsm6dsox_tap_dur_set(&g_dev_ctx, 0x07);
    lsm6dsox_tap_quiet_set(&g_dev_ctx, 0x03);
    lsm6dsox_tap_shock_set(&g_dev_ctx, 0x03);

    /* Enable Single and Double Tap detection. */
    lsm6dsox_tap_mode_set(&g_dev_ctx, LSM6DSOX_BOTH_SINGLE_DOUBLE);

    /* Apply high-pass digital filter on Wake-Up function */
    lsm6dsox_xl_hp_path_internal_set(&g_dev_ctx, LSM6DSOX_USE_SLOPE);

    /* Set Wake-Up threshold: 1 LSb corresponds to FS_XL/2^6 */
    lsm6dsox_wkup_threshold_set(&g_dev_ctx, 4);

    /* Set threshold to 60 degrees. */
    lsm6dsox_6d_threshold_set(&g_dev_ctx, LSM6DSOX_DEG_60);

    /* LPF2 on 6D/4D function selection. */
    lsm6dsox_xl_lp2_on_6d_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_4d_mode_set(&g_dev_ctx, PROPERTY_DISABLE);

    /* Set Free Fall duration to 3 and 6 samples event duration */
    lsm6dsox_ff_dur_set(&g_dev_ctx, 0x06);
    lsm6dsox_ff_threshold_set(&g_dev_ctx, LSM6DSOX_FF_TSH_312mg);

    /* Enable Tilt in embedded function. */
    emb_sens.tilt = PROPERTY_ENABLE;

    /* Enable pedometer */
    emb_sens.step = PROPERTY_ENABLE;
    lsm6dsox_pedo_sens_set(&g_dev_ctx, LSM6DSOX_FALSE_STEP_REJ_ADV_MODE);

    /* Route signals on interrupt pin 1 */
    /*
    lsm6dsox_pin_int2_route_get(&g_dev_ctx, NULL, &pin_int2_route);
    pin_int2_route.mlc1 = PROPERTY_ENABLE;
    pin_int2_route.step_detector = PROPERTY_ENABLE;
    //pin_int2_route.wake_up = PROPERTY_ENABLE;
    //pin_int2_route.tilt = PROPERTY_ENABLE;
    pin_int2_route.double_tap = PROPERTY_ENABLE;
    pin_int2_route.single_tap = PROPERTY_ENABLE;
    //pin_int2_route.six_d = PROPERTY_ENABLE;
    //pin_int2_route.free_fall = PROPERTY_ENABLE;
    lsm6dsox_pin_int2_route_set(&g_dev_ctx, NULL, pin_int2_route);
    */

   lsm6dsox_pin_int1_route_get(&g_dev_ctx, &pin_int1_route);
    pin_int1_route.mlc1 = PROPERTY_ENABLE;
    pin_int1_route.step_detector = PROPERTY_ENABLE;
    //pin_int1_route.wake_up = PROPERTY_ENABLE;
    //pin_int1_route.tilt = PROPERTY_ENABLE;
    pin_int1_route.double_tap = PROPERTY_ENABLE;
    pin_int1_route.single_tap = PROPERTY_ENABLE;
    //pin_int1_route.six_d = PROPERTY_ENABLE;
    //pin_int1_route.free_fall = PROPERTY_ENABLE;
    lsm6dsox_pin_int1_route_set(&g_dev_ctx, pin_int1_route);


    /* Configure interrupt pin mode notification */
    lsm6dsox_int_notification_set(&g_dev_ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

    /* Enable embedded features */
    lsm6dsox_embedded_sens_set(&g_dev_ctx, &emb_sens);

    /* Set Output Data Rate.
   * Selected data rate have to be equal or greater with respect
   * with MLC data rate.
   */
    
    lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_208Hz);

    /* Reset steps of pedometer */
    lsm6dsox_steps_reset(&g_dev_ctx);

    /* Main loop */
    /*
    while (1)
    {
    }
    */
    
}

void lsm6dsox_multi_conf_irq_handler(void)
{
    //NRF_LOG_INFO("lsm6dsox_multi_conf_irq_handler");
    lsm6dsox_all_sources_t status;

    /* Read interrupt source registers in polling mode (no int) */
    lsm6dsox_all_sources_get(&g_dev_ctx, &status);

    //NRF_LOG_INFO("AFTER lsm6dsox_all_sources_get");
    /*if (status.wake_up)
    {
        sprintf((char *)tx_buffer, "Wake-Up event on ");
        if (status.wake_up_x)
            strcat((char *)tx_buffer, "X");
        if (status.wake_up_y)
            strcat((char *)tx_buffer, "Y");
        if (status.wake_up_z)
            strcat((char *)tx_buffer, "Z");
        strcat((char *)tx_buffer, " direction\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    */
    if (status.step_detector)
    {
        /* Read steps */
        lsm6dsox_number_of_steps_get(&g_dev_ctx, (uint8_t *)&steps);
        sprintf((char *)tx_buffer, "steps :%d\r\n", steps);
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    /*
    if (status.mlc1)
    {
        lsm6dsox_mlc_out_get(&g_dev_ctx, mlc_out);
        sprintf((char *)tx_buffer, "Detect MLC interrupt code: %02X\r\n",
                mlc_out[0]);
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }*/
    if (status.double_tap)
    {
        sprintf((char *)tx_buffer, "D-Tap: ");
        if (status.tap_x)
            strcat((char *)tx_buffer, "x-axis");
        else if (status.tap_y)
            strcat((char *)tx_buffer, "y-axis");
        else
            strcat((char *)tx_buffer, "z-axis");
        if (status.tap_sign)
            strcat((char *)tx_buffer, " negative");
        else
            strcat((char *)tx_buffer, " positive");
        strcat((char *)tx_buffer, " sign\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    if (status.single_tap)
    {
        sprintf((char *)tx_buffer, "S-Tap: ");
        if (status.tap_x)
            strcat((char *)tx_buffer, "x-axis");
        else if (status.tap_y)
            strcat((char *)tx_buffer, "y-axis");
        else
            strcat((char *)tx_buffer, "z-axis");
        if (status.tap_sign)
            strcat((char *)tx_buffer, " negative");
        else
            strcat((char *)tx_buffer, " positive");
        strcat((char *)tx_buffer, " sign\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    /*
    if (status.tilt)
    {
        sprintf((char *)tx_buffer, "TILT Detected\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    
    if (status.six_d)
    {
        sprintf((char *)tx_buffer, "6D Or. switched to ");
        if (status.six_d_xh)
            strcat((char *)tx_buffer, "XH");
        if (status.six_d_xl)
            strcat((char *)tx_buffer, "XL");
        if (status.six_d_yh)
            strcat((char *)tx_buffer, "YH");
        if (status.six_d_yl)
            strcat((char *)tx_buffer, "YL");
        if (status.six_d_zh)
            strcat((char *)tx_buffer, "ZH");
        if (status.six_d_zl)
            strcat((char *)tx_buffer, "ZL");
        strcat((char *)tx_buffer, "\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    if (status.free_fall)
    {
        sprintf((char *)tx_buffer, "Free Fall Detected\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    */
}

/******************************************************************************/

/* Program: wrist_tilt */
static const uint8_t lsm6so_prg_wrist_tilt[] = {
      0x52, 0x00, 0x14, 0x00, 0x00, 0x00, 0xae, 0xb7,
      0x80, 0x00, 0x00, 0x06, 0x0f, 0x05, 0x73, 0x33,
      0x07, 0x54, 0x44, 0x22,
     };


void lsm6dsox_fsm_init(void)
{
  /* Variable declaration */
  lsm6dsox_pin_int1_route_t   pin_int1_route;
  lsm6dsox_emb_fsm_enable_t   fsm_enable;
  lsm6dsox_emb_sens_t         emb_sens;
  lsm6dsox_fsm_out_t          fsm_out;
  lsm6dsox_all_sources_t      status;
  uint16_t                   fsm_addr;

  /* Initialize mems driver interface */
  g_dev_ctx.write_reg = platform_write;
  g_dev_ctx.read_reg  = platform_read;
  g_dev_ctx.handle    = &imu_m_twi;

  	/* Init test platform */
	platform_init();

  /* Wait sensor boot time */
  platform_delay(10);

  /* Check device ID */
  lsm6dsox_device_id_get(&g_dev_ctx, &whoamI);
  if (whoamI != LSM6DSOX_ID)
    while(1);

  /* Restore default configuration (not FSM) */
  lsm6dsox_reset_set(&g_dev_ctx, PROPERTY_ENABLE);
  do {
    lsm6dsox_reset_get(&g_dev_ctx, &rst);
  } while (rst);

  /* Disable I3C interface */
  lsm6dsox_i3c_disable_set(&g_dev_ctx, LSM6DSOX_I3C_DISABLE);

  /* Enable Block Data Update */
  lsm6dsox_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);

  /* Set full scale */
  lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_2g);
  lsm6dsox_gy_full_scale_set(&g_dev_ctx, LSM6DSOX_2000dps);

  /* Route signals on interrupt pin 1 */
  lsm6dsox_pin_int1_route_get(&g_dev_ctx, &pin_int1_route);
  pin_int1_route.fsm1               = PROPERTY_ENABLE;
  pin_int1_route.fsm2               = PROPERTY_DISABLE;
  pin_int1_route.fsm3               = PROPERTY_DISABLE;
  pin_int1_route.fsm4               = PROPERTY_DISABLE;
  pin_int1_route.fsm5               = PROPERTY_DISABLE;
  pin_int1_route.fsm6               = PROPERTY_DISABLE;
  pin_int1_route.fsm7               = PROPERTY_DISABLE;
  lsm6dsox_pin_int1_route_set(&g_dev_ctx, pin_int1_route);

  /* Configure interrupt pin mode notification */
  lsm6dsox_int_notification_set(&g_dev_ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

  /*
   * Start Finite State Machine configuration
   */

  /* Reset Long Counter */
  lsm6dsox_long_cnt_int_value_set(&g_dev_ctx, 0x0000U);

  /* Set the first address where the programs are written */
  lsm6dsox_fsm_start_address_set(&g_dev_ctx, LSM6DSOX_START_FSM_ADD);

  /* Set the number of the programs */
  lsm6dsox_fsm_number_of_programs_set(&g_dev_ctx, 1 );

  /* Enable final state machine */
  fsm_enable.fsm_enable_a.fsm1_en    = PROPERTY_ENABLE ;
  fsm_enable.fsm_enable_a.fsm2_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm3_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm4_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm5_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm6_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm7_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm8_en    = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm9_en    = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm10_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm11_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm12_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm13_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm14_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm15_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm16_en   = PROPERTY_DISABLE;
  lsm6dsox_fsm_enable_set(&g_dev_ctx, &fsm_enable);

  /* Set Finite State Machine data rate */
  lsm6dsox_fsm_data_rate_set(&g_dev_ctx, LSM6DSOX_ODR_FSM_26Hz);

  /* Write Programs */
  fsm_addr = LSM6DSOX_START_FSM_ADD;

  /* wrist_tilt */
  lsm6dsox_ln_pg_write(&g_dev_ctx, fsm_addr, (uint8_t*)lsm6so_prg_wrist_tilt,
                      sizeof(lsm6so_prg_wrist_tilt));

  emb_sens.fsm = PROPERTY_ENABLE;
  lsm6dsox_embedded_sens_set(&g_dev_ctx, &emb_sens);
 /*
  * End Finite State Machine configuration
  */



  /* Set Output Data Rate */
  //lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_104Hz);
  //lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_104Hz);

      lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_208Hz);

#if 0
  /* Main loop */
  while(1)
  {
      //NRF_LOG_INFO("FSM WHILE");
    /* Read interrupt source registers in polling mode (no int) */
    
    lsm6dsox_all_sources_get(&g_dev_ctx, &status);

    if(status.fsm1) {
        sprintf((char*)tx_buffer, "wrist tilt detected\r\n");
        tx_com(tx_buffer, strlen((char const*)tx_buffer));
    }

    nrf_delay_ms(20);
  }
#endif
}

void lsm6dsox_fsm_irq_handler(void)
{
    lsm6dsox_all_sources_t status;

    static int calltime = 0;

      //NRF_LOG_INFO("FSM WHILE");
    /* Read interrupt source registers in polling mode (no int) */
    lsm6dsox_all_sources_get(&g_dev_ctx, &status);

    if(status.fsm1) {

        sprintf((char*)tx_buffer, "wrist tilt detected, %d\r\n", calltime++);
        tx_com(tx_buffer, strlen((char const*)tx_buffer));
    }
}

void fsm_multiconf_iqr_handler(void)
{
        lsm6dsox_all_sources_t status;

    static int calltime = 0;

      //NRF_LOG_INFO("FSM WHILE");
    /* Read interrupt source registers in polling mode (no int) */
    lsm6dsox_all_sources_get(&g_dev_ctx, &status);



    /*********************************/


    if(status.fsm1) {

        sprintf((char*)tx_buffer, "wrist tilt detected, %d\r\n", calltime++);
        tx_com(tx_buffer, strlen((char const*)tx_buffer));
    }
    if (status.step_detector)
    {
        /* Read steps */
        lsm6dsox_number_of_steps_get(&g_dev_ctx, (uint8_t *)&steps);
        sprintf((char *)tx_buffer, "steps :%d\r\n", steps);
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    if (status.double_tap)
    {
        sprintf((char *)tx_buffer, "D-Tap: ");
        if (status.tap_x)
            strcat((char *)tx_buffer, "x-axis");
        else if (status.tap_y)
            strcat((char *)tx_buffer, "y-axis");
        else
            strcat((char *)tx_buffer, "z-axis");
        if (status.tap_sign)
            strcat((char *)tx_buffer, " negative");
        else
            strcat((char *)tx_buffer, " positive");
        strcat((char *)tx_buffer, " sign\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
    if (status.single_tap)
    {
        sprintf((char *)tx_buffer, "S-Tap: ");
        if (status.tap_x)
            strcat((char *)tx_buffer, "x-axis");
        else if (status.tap_y)
            strcat((char *)tx_buffer, "y-axis");
        else
            strcat((char *)tx_buffer, "z-axis");
        if (status.tap_sign)
            strcat((char *)tx_buffer, " negative");
        else
            strcat((char *)tx_buffer, " positive");
        strcat((char *)tx_buffer, " sign\r\n");
        tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
}

void fsm_multiconf_init(void)
{
/* Variable declaration */
    lsm6dsox_pin_int1_route_t pin_int1_route;
  lsm6dsox_emb_fsm_enable_t   fsm_enable;
  lsm6dsox_fsm_out_t          fsm_out;
  uint16_t                   fsm_addr;

    lsm6dsox_emb_sens_t emb_sens;
    
    uint32_t i;
    
    /* Initialize mems driver interface */
    g_dev_ctx.write_reg = platform_write;
    g_dev_ctx.read_reg = platform_read;
    g_dev_ctx.handle = &imu_m_twi;

    /* Init test platform */
    platform_init();

    /* Wait sensor boot time */
    platform_delay(10);

    /* Check device ID */
    
    lsm6dsox_device_id_get(&g_dev_ctx, &whoamI);
    platform_delay(10);
    NRF_LOG_INFO("LSM6DSOX device id: %d", whoamI);
    if (whoamI != LSM6DSOX_ID)
        while (1)
            ;

    /* Restore default configuration */
    lsm6dsox_reset_set(&g_dev_ctx, PROPERTY_ENABLE);
    do
    {
        lsm6dsox_reset_get(&g_dev_ctx, &rst);
    } while (rst);

    /* Start Machine Learning Core configuration ******************************/
    /**************************************************************************/
    for (i = 0; i < (sizeof(lsm6dsox_yoga_pose_recognition) /
                     sizeof(ucf_line_t));
         i++)
    {

        lsm6dsox_write_reg(&g_dev_ctx, lsm6dsox_yoga_pose_recognition[i].address,
                           (uint8_t *)&lsm6dsox_yoga_pose_recognition[i].data, 1);
    }
    /* End Machine Learning Core configuration */

    /* At this point the device is ready to run but if you need you can also
   * interact with the device but taking in account the MLC configuration.
   *
   * For more information about Machine Learning Core tool please refer
   * to AN5259 "LSM6DSOX: Machine Learning Core".
   */

    /* Turn off embedded features */
    lsm6dsox_embedded_sens_get(&g_dev_ctx, &emb_sens);
    lsm6dsox_embedded_sens_off(&g_dev_ctx);
    platform_delay(10);

    /* Turn off Sensors */
    lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_OFF);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_OFF);

    /* Disable I3C interface */
    lsm6dsox_i3c_disable_set(&g_dev_ctx, LSM6DSOX_I3C_DISABLE);

    /* Enable Block Data Update */
    lsm6dsox_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);

    /* Set full scale */
    lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_4g);
    lsm6dsox_gy_full_scale_set(&g_dev_ctx, LSM6DSOX_2000dps);

    /* Enable Tap detection on X, Y, Z */
    lsm6dsox_tap_detection_on_z_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_tap_detection_on_y_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_tap_detection_on_x_set(&g_dev_ctx, PROPERTY_ENABLE);

    /* Set Tap threshold to 01000b, therefore the tap threshold
   * is 500 mg (= 12 * FS_XL / 32 )
   */
    lsm6dsox_tap_threshold_x_set(&g_dev_ctx, 0x04);
    lsm6dsox_tap_threshold_y_set(&g_dev_ctx, 0x04);
    lsm6dsox_tap_threshold_z_set(&g_dev_ctx, 0x04);
    /* Configure Single and Double Tap parameter
   *
   * For the maximum time between two consecutive detected taps, the DUR
   * field of the INT_DUR2 register is set to 0111b, therefore the Duration
   * time is 538.5 ms (= 7 * 32 * ODR_XL)
   *
   * The SHOCK field of the INT_DUR2 register is set to 11b, therefore
   * the Shock time is 57.36 ms (= 3 * 8 * ODR_XL)
   *
   * The QUIET field of the INT_DUR2 register is set to 11b, therefore
   * the Quiet time is 28.68 ms (= 3 * 4 * ODR_XL)
   */
    lsm6dsox_tap_dur_set(&g_dev_ctx, 0x07);
    lsm6dsox_tap_quiet_set(&g_dev_ctx, 0x03);
    lsm6dsox_tap_shock_set(&g_dev_ctx, 0x03);

    /* Enable Single and Double Tap detection. */
    lsm6dsox_tap_mode_set(&g_dev_ctx, LSM6DSOX_BOTH_SINGLE_DOUBLE);

    /* Apply high-pass digital filter on Wake-Up function */
    lsm6dsox_xl_hp_path_internal_set(&g_dev_ctx, LSM6DSOX_USE_SLOPE);

    /* Set Wake-Up threshold: 1 LSb corresponds to FS_XL/2^6 */
    lsm6dsox_wkup_threshold_set(&g_dev_ctx, 4);

    /* Set threshold to 60 degrees. */
    lsm6dsox_6d_threshold_set(&g_dev_ctx, LSM6DSOX_DEG_60);

    /* LPF2 on 6D/4D function selection. */
    lsm6dsox_xl_lp2_on_6d_set(&g_dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_4d_mode_set(&g_dev_ctx, PROPERTY_DISABLE);

    /* Set Free Fall duration to 3 and 6 samples event duration */
    lsm6dsox_ff_dur_set(&g_dev_ctx, 0x06);
    lsm6dsox_ff_threshold_set(&g_dev_ctx, LSM6DSOX_FF_TSH_312mg);

    /* Enable Tilt in embedded function. */
    emb_sens.tilt = PROPERTY_ENABLE;

    /* Enable pedometer */
    emb_sens.step = PROPERTY_ENABLE;
    lsm6dsox_pedo_sens_set(&g_dev_ctx, LSM6DSOX_FALSE_STEP_REJ_ADV_MODE);

    /* Route signals on interrupt pin 1 */
   lsm6dsox_pin_int1_route_get(&g_dev_ctx, &pin_int1_route);
    pin_int1_route.mlc1 = PROPERTY_ENABLE;
    pin_int1_route.step_detector = PROPERTY_ENABLE;
    //pin_int1_route.wake_up = PROPERTY_ENABLE;
    //pin_int1_route.tilt = PROPERTY_ENABLE;
    pin_int1_route.double_tap = PROPERTY_ENABLE;
    pin_int1_route.single_tap = PROPERTY_ENABLE;
    //pin_int1_route.six_d = PROPERTY_ENABLE;
    //pin_int1_route.free_fall = PROPERTY_ENABLE;

      pin_int1_route.fsm1               = PROPERTY_ENABLE;
  pin_int1_route.fsm2               = PROPERTY_DISABLE;
  pin_int1_route.fsm3               = PROPERTY_DISABLE;
  pin_int1_route.fsm4               = PROPERTY_DISABLE;
  pin_int1_route.fsm5               = PROPERTY_DISABLE;
  pin_int1_route.fsm6               = PROPERTY_DISABLE;
  pin_int1_route.fsm7               = PROPERTY_DISABLE;

    lsm6dsox_pin_int1_route_set(&g_dev_ctx, pin_int1_route);


    /* Configure interrupt pin mode notification */
    lsm6dsox_int_notification_set(&g_dev_ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

    /* Enable embedded features */
    lsm6dsox_embedded_sens_set(&g_dev_ctx, &emb_sens);

    /* Set Output Data Rate.
   * Selected data rate have to be equal or greater with respect
   * with MLC data rate.
   */
    
    lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_208Hz);

    /* Reset steps of pedometer */
    lsm6dsox_steps_reset(&g_dev_ctx);


    /******************************************/


  /*
   * Start Finite State Machine configuration
   */

  /* Reset Long Counter */
  lsm6dsox_long_cnt_int_value_set(&g_dev_ctx, 0x0000U);

  /* Set the first address where the programs are written */
  lsm6dsox_fsm_start_address_set(&g_dev_ctx, LSM6DSOX_START_FSM_ADD);

  /* Set the number of the programs */
  lsm6dsox_fsm_number_of_programs_set(&g_dev_ctx, 1 );

  /* Enable final state machine */
  fsm_enable.fsm_enable_a.fsm1_en    = PROPERTY_ENABLE ;
  fsm_enable.fsm_enable_a.fsm2_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm3_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm4_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm5_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm6_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm7_en    = PROPERTY_DISABLE ;
  fsm_enable.fsm_enable_a.fsm8_en    = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm9_en    = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm10_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm11_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm12_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm13_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm14_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm15_en   = PROPERTY_DISABLE;
  fsm_enable.fsm_enable_b.fsm16_en   = PROPERTY_DISABLE;
  lsm6dsox_fsm_enable_set(&g_dev_ctx, &fsm_enable);

  /* Set Finite State Machine data rate */
  lsm6dsox_fsm_data_rate_set(&g_dev_ctx, LSM6DSOX_ODR_FSM_26Hz);

  /* Write Programs */
  fsm_addr = LSM6DSOX_START_FSM_ADD;

  /* wrist_tilt */
  lsm6dsox_ln_pg_write(&g_dev_ctx, fsm_addr, (uint8_t*)lsm6so_prg_wrist_tilt,
                      sizeof(lsm6so_prg_wrist_tilt));

  emb_sens.fsm = PROPERTY_ENABLE;
  lsm6dsox_embedded_sens_set(&g_dev_ctx, &emb_sens);
 /*
  * End Finite State Machine configuration
  */


      lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);
    lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_208Hz);
}