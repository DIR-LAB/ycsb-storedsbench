//
//  array_dram.h
//  DRAM Performance
//
//  Created by Christopher Storm York  on 8/8/19.
//  Copyright © 2019 Storm York . All rights reserved.
//

#ifndef array_dram_h
#define array_dram_h

int array_dram_init(const char *path);
int array_dram_read(const char *key, void *result);
int array_dram_scan(const char *key, int len, void *result);
int array_dram_update(const char *key, void *value);
int array_dram_insert(const char *key, void *value);
int array_dram_delete(const char *key);

#endif /* array_dram_h */
