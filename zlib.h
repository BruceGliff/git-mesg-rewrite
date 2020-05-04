#pragma once
//  Alloc and return decompressed from src data.\
    Write size of decompressed data in dst_size
char * decompress_data(char * src, int src_size, int * dst_size);
//  Alloc and return compressed from src data.\
    Write size of compressed data in dst_size
char * compress_data(char * src, int stc_size, int * dst_size);


