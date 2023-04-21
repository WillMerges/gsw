/********************************************************************
*  Name: Shm.c
*
*  Purpose: Declares Shm object to store data in shared memory
*
*  Author: Will Merges
*
*********************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#include "lib/shm/Shm.h"
#include "lib/logging/MessageLogger.h"
#include "common/types.h"

// NOTE: shared memory can be manually altered with 'ipcs' and 'ipcrm' programs

// constructor
Shm::Shm(const char* file, const int id, size_t size):size(size),
                                                m_keyFile(file), m_keyId(id) {
    data = NULL;
    m_shmid = -1;
}

// creates shared memory but does not attach to it
RetType Shm::create() {
    MessageLogger logger("Shm", "create");

    // create key
    key_t key = ftok(m_keyFile, m_keyId);
    if(key == (key_t) -1) {
        logger.log_message("ftok failure, no key generated", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    // create the shm
    m_shmid = shmget(key, size, 0666|IPC_CREAT|IPC_EXCL);
    if(-1 == m_shmid) {
        logger.log_message("shmget failure", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    return SUCCESS;
}

RetType Shm::attach() {
    MessageLogger logger("Shm", "attach");

    // create key
    key_t key = ftok(m_keyFile, m_keyId);
    if(key == (key_t) -1) {
        logger.log_message("ftok failure, no key generated", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    // get id
    m_shmid = shmget(key, size, 0666);
    if(-1 == m_shmid) {
        logger.log_message("shmget failure", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    // attach to shared block
    data = (uint8_t*) shmat(m_shmid, (void*)0, 0);
    if((void*) -1 == data) {
        data = NULL;
        logger.log_message("shmat failure, cannot attach to shmem", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    // everyone who attaches should attempt to lock the shared pages into RAM
    // this avoids delays with page faults
    // technically only one process needs to call this lock, but the pages are
    // automatically unlocked upon termination of the process so each attached process
    // should lock the pages
    if(shmctl(m_shmid, SHM_LOCK, NULL) == -1) {
        logger.log_message("failed to lock shared memory pages into RAM", MessageLoggerDecls::WARN);
        // non-critical, don't fail
    }

    return SUCCESS;
}

RetType Shm::detach() {
    MessageLogger logger("Shm", "detach");

    if(data) {
        if(shmdt(data) == 0) {
            data = NULL;
            return SUCCESS;
        } else {
            logger.log_message("shmdt failure", MessageLoggerDecls::CRIT);
        }
    } else {
        logger.log_message("process is not attached, nothing to detach from", MessageLoggerDecls::WARN);
        return SUCCESS;
    }

    return FAILURE;
}

RetType Shm::destroy() {
    MessageLogger logger("Shm", "destroy");

    if(data == NULL) {
        logger.log_message("No shmem to destroy", MessageLoggerDecls::WARN);
        return SUCCESS;
    }

    if(shmctl(m_shmid, IPC_RMID, NULL) == -1) {
        logger.log_message("shmctl failure, unable to destroy shared memory", MessageLoggerDecls::CRIT);
        return FAILURE;
    }

    m_shmid = -1;
    data = NULL;

    return SUCCESS;
}
