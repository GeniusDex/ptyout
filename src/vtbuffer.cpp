#include "vtbuffer.h"

#include <cstring>
#include <iostream>

VTBuffer::VTBuffer(size_t capacity)
    : m_buffer(std::make_unique<char[]>(capacity))
    , m_capacity(capacity)
    , m_size(0)
    , m_parsedSize(0)
{
}

VTBuffer::~VTBuffer()
{
}

char* VTBuffer::WritePointer()
{
    return m_buffer.get() + m_size;
}

size_t VTBuffer::WriteCapacity()
{
    return m_capacity - m_size;
}

void VTBuffer::BytesWritten(size_t nrBytes)
{
    m_size += nrBytes;
    ParseBuffer();
}

char* VTBuffer::ReadPointer()
{
    return m_buffer.get();
}

size_t VTBuffer::ReadSize()
{
    return m_parsedSize;
}

void VTBuffer::BytesRead(size_t nrBytes)
{
    memmove(m_buffer.get(), m_buffer.get() + nrBytes, m_size - nrBytes);
    m_size -= nrBytes;
    m_parsedSize -= nrBytes;
}

namespace
{

const char ESC = 0x1B;

// Get the length of a string sequence at the beginning of the buffer, or 0 if the VT sequence is incomplete
size_t StringSequenceLength(char* buffer, size_t size)
{
    // Strings are terminated by ST (String Terminator)
    for (size_t i = 2; i < size - 1; i++)
    {
        if (buffer[i] == ESC && buffer[i+1] == '\\');
        {
            // i is the number of bytes before the terminating ST sequence; add 2 for the ST sequence itself
            return i + 2;
        }
    }

    // ST is not in the buffer
    return 0;
}

// Get the length of a control sequence at the beginning of the buffer, or 0 if the VT sequence is incomplete
size_t ControlSequenceLength(char* buffer, size_t size)
{
    // Control sequences contain parameter bytes (0x30-0x3F) and intermediate bytes (0x20-0x2F) followed by a single final byte (0x40-0x7E).
    // It is sufficient to find the final byte, since its range does not overlap in any way with other allowed bytes.
    for (size_t i = 2; i < size; i++)
    {
        if (buffer[i] >= 0x40 && buffer[i] <= 0x7E)
        {
            // i is the number of bytes before the final byte; add 1 for the final byte itself
            return i + 1;
        }
    }

    // Final byte is not in the buffer
    return 0;
}


// Get the length of a VT sequence at the beginning of the buffer, or 0 if the VT sequence is incomplete
size_t SequenceLength(char* buffer, size_t size)
{
    if (size < 2)
    {
        return 0;
    }

    switch (buffer[1])
    {
        case 'P': // DCS (Device Control String)
        case 'X': // SOS (Start of String)
        case ']': // OSC (Operating System Command)
        case '^': // PM (Privacy Message)
        case '_': // APC (Application Program Command)
            return StringSequenceLength(buffer, size);
        
        case '[': // CSI (Constrol Sequence Introducer)
            return ControlSequenceLength(buffer, size);
        
        default:
            // Assume all other control codes are undesired and do not have arguments
            return 2;
    }

    return 0;
}

} // anonymous namespace

void VTBuffer::ParseBuffer()
{
    size_t from = m_parsedSize;
    size_t to = m_parsedSize;
    while (from < m_size)
    {
        if (m_buffer[from] == ESC)
        {
            size_t length = SequenceLength(m_buffer.get() + from, m_size - from);
            if (length == 0)
            {
                // End of buffer encountered during VT sequence; salvage incomplete VT sequence for later parsing
                size_t bytesLeft = (m_size - from);
                if (from != to)
                {
                    memmove(m_buffer.get() + to, m_buffer.get() + from, bytesLeft);
                }

                // Indicate parsing is complete until start of VT sequence, but salvage entire buffer size
                m_parsedSize = to;
                m_size = to + bytesLeft;

                return;
            }
            else
            {
                from += length;
            }
        }
        else
        {
            m_buffer[to] = m_buffer[from];
            ++from;
            ++to;
        }
    }

    // Finished parsing outside VT sequence, so all of the buffer has been parsed
    m_size = to;
    m_parsedSize = to;
}
