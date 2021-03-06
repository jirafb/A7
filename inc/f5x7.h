/*
    created with FontEditor written by H. Reddmann
    HaReddmann at t-online dot de

    File Name           : f5x7.h
    Date                : 8.1.2012
    Font size in bytes  : 0x018E, 398
    Font width          : 6
    Font height         : 7
    Font first char     : 0x20
    Font last char      : 0x80
    Font bits per pixel : 1
    Font is compressed  : false

    The font data are defined as

    struct _FONT_ {
     // common shared fields
       uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
       uint8_t    font_Width_in_Pixel_for_fixed_drawing;
       uint8_t    font_Height_in_Pixel_for_all_Characters;
       uint8_t    font_Bits_per_Pixels;
                    // if MSB are set then font is a compressed font
       uint8_t    font_First_Char;
       uint8_t    font_Last_Char;
       uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
                    // for each character the separate width in pixels,
                    // characters < 128 have an implicit virtual right empty row
                    // characters with font_Char_Widths[] == 0 are undefined

     // if compressed font then additional fields
       uint8_t    font_Byte_Padding;
                    // each Char in the table are aligned in size to this value
       uint8_t    font_RLE_Table[3];
                    // Run Length Encoding Table for compression
       uint8_t    font_Char_Size_in_Bytes[font_Last_Char - font_First_Char +1];
                    // for each char the size in (bytes / font_Byte_Padding) are stored,
                    // this get us the table to seek to the right beginning of each char
                    // in the font_data[].

     // for compressed and uncompressed fonts
       uint8_t    font_data[];
                    // bit field of all characters
    }
*/

#ifndef f5x7_H
#define f5x7_H

const unsigned char f5x7[] = {
    0x01, 0x8E, 0x06, 0x07, 0x01, 0x20, 0x80,
    0x04, 0x01, 0x03, 0x05, 0x03, 0x04, 0x04, 0x01, 0x02, 0x02, 0x02, 0x03, 0x01, 0x03, 0x01, 0x02, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x01, 0x03, 0x04, 0x03, 0x04, 
    0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x05, 0x05, 0x04, 0x02, 0x02, 0x02, 0x03, 0x05, 
    0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x04, 0x04, 0x01, 0x02, 0x04, 0x02, 0x05, 0x04, 0x04, 
    0x04, 0x04, 0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x04, 0x04, 0x03, 0x02, 0x01, 0x02, 0x04, 0x01, 
    0x04, 
    0x00, 0x00, 0x00, 0xF0, 0x1A, 0x00, 0x06, 0x92, 0x9F, 0xE4, 0x27, 0xB1, 0xFC, 0x34, 0x93, 0x05, 
    0x4D, 0xA6, 0x29, 0x69, 0x50, 0x03, 0x5F, 0x30, 0xE8, 0x33, 0x18, 0x10, 0x1C, 0x04, 0x18, 0x81, 
    0x40, 0x80, 0x70, 0x07, 0x4F, 0x28, 0xE4, 0x01, 0x08, 0x7E, 0x80, 0x50, 0x2C, 0x65, 0x0A, 0x95, 
    0x4A, 0x1A, 0x8E, 0xE4, 0x07, 0x39, 0x95, 0x4A, 0x18, 0x4F, 0xA9, 0x84, 0x09, 0xE4, 0x0A, 0x03, 
    0x4D, 0xA9, 0xA4, 0x31, 0xA4, 0x52, 0x1E, 0x0A, 0x1A, 0x41, 0x11, 0x51, 0x28, 0x14, 0x8A, 0x88, 
    0x82, 0x10, 0xA4, 0x12, 0x06, 0x0C, 0x49, 0xA3, 0xE1, 0xF0, 0x16, 0x0B, 0xDE, 0xAF, 0x54, 0xD2, 
    0x78, 0x42, 0xA1, 0xD0, 0x2F, 0x14, 0xF2, 0xFC, 0x4A, 0xA5, 0xD0, 0xAF, 0x50, 0x08, 0x78, 0x42, 
    0xA9, 0xDC, 0x8F, 0x40, 0xF8, 0x85, 0x7E, 0x21, 0x50, 0x28, 0xF4, 0xF9, 0x31, 0x24, 0xA1, 0x1F, 
    0x08, 0x04, 0xFA, 0x19, 0x18, 0x86, 0xDF, 0x6F, 0xC0, 0x80, 0xFD, 0x3C, 0xA1, 0x90, 0xE7, 0x97, 
    0x48, 0x18, 0x3C, 0xA1, 0x98, 0xFF, 0x97, 0xC8, 0xD8, 0x44, 0xA5, 0x52, 0x26, 0x10, 0xF8, 0x05, 
    0x02, 0x1F, 0x10, 0xE8, 0x73, 0xC0, 0x80, 0x30, 0x87, 0x07, 0x8C, 0x01, 0x7B, 0x84, 0x66, 0x8C, 
    0x59, 0x68, 0x40, 0xC0, 0x11, 0x06, 0xB1, 0x54, 0x69, 0xF4, 0x0F, 0x1E, 0x70, 0xC1, 0xBF, 0x20, 
    0x20, 0x00, 0x02, 0x81, 0x40, 0x60, 0x40, 0x80, 0x21, 0x91, 0x78, 0x3F, 0x12, 0x09, 0x83, 0x21, 
    0x91, 0x48, 0x18, 0x12, 0xE9, 0x87, 0x61, 0xB1, 0x50, 0xBE, 0x42, 0x00, 0x41, 0xA5, 0xF2, 0x7E, 
    0x04, 0x02, 0xAE, 0x07, 0xEC, 0xFD, 0x10, 0x34, 0xD2, 0x07, 0xC4, 0x23, 0xE0, 0x08, 0x38, 0x1E, 
    0x81, 0x80, 0xC3, 0x90, 0x48, 0x18, 0x3E, 0x85, 0x82, 0x40, 0x50, 0x28, 0x7C, 0x1C, 0x81, 0x80, 
    0x62, 0x51, 0x08, 0x1E, 0x12, 0x07, 0x04, 0xE2, 0x31, 0x60, 0x30, 0x06, 0x03, 0x86, 0x80, 0x31, 
    0x48, 0x18, 0x0C, 0x89, 0x01, 0x85, 0x31, 0x68, 0x2C, 0x16, 0xE2, 0xFE, 0xBF, 0x23, 0x04, 0x01, 
    0x41, 0xE0, 0xC7, 0xF0, 0xB4, 0x42
};

#endif

