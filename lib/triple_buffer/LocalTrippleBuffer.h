/******************************************************************************
*  Name: TripleBuffer.h
*
*  Purpose: triple buffer implementation
*
*  Author: Will Merges
*
******************************************************************************/

#ifndef LOCAL_TRIPLE_BUFFER_H
#define LOCAL_TRIPLE_BUFFER_H

/// @brief triple buffer allocated to local memory (e.g. non-shared)
/// @tparam TYPE    the type of each buffer
template <typename TYPE>
class LocalTripleBuffer : public TripleBuffer<TYPE> {
public:
    /// @brief public constructor
    LocalTripleBuffer() : TripleBuffer<TYPE>(m_ctl, m_buffs) {}
private:
    volatile uint_fast8_t m_ctl;
    TYPE m_buffs[3];
};

#endif
