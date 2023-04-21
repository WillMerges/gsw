/********************************************************************
*  Name: shm.h
*
*  Purpose: Defines Shm object to store data in shared memory.
*
*  Author: Will Merges
*
*********************************************************************/
#ifndef SHM_H
#define SHM_H

#include <stdint.h>
#include <stdlib.h>
#include "common/types.h"

// faciliates access to shared memory
class Shm {
public:
    /// @brief constructor
    /// @param file     a filename to use to generate a unique key
    /// @param id       an ID to use to generate a unique key
    /// @param size     the size of the shared memory block
    /// NOTE: a unique filename/id pair generates a unique key
    Shm(const char* file, const int id, size_t size);

    /// @brief default destructor
    virtual ~Shm() {}

    /// @brief attach the current process to the shared memory block
    RetType attach();

    /// @brief detach the current process from the shared memory block
    RetType detach();

    /// @brief destroy the shared memory block
    // NOTE: must have called create or attach first
    RetType destroy();

    /// @brief create shared memory block
    // NOTE: does not attach the process to the block
    RetType create();

    // pointer to shared memory block
    // NULL when not attached
    uint8_t* data;

    // size of shared memory block
    const size_t size;

private:
    // key values
    const char* m_keyFile;
    const int m_keyId;

    // shared memory id
    int m_shmid;
};

#endif
