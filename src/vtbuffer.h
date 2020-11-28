#ifndef VTBUFFER_H
#define VTBUFFER_H

#include <memory>

// VTBuffer is a buffer which strips ANSI VT sequences from text in the buffer
class VTBuffer
{
public:
    VTBuffer(size_t capacity);
    ~VTBuffer();

    // WritePointer returns a pointer to the first position where new data can be written
    char* WritePointer();
    // WriteCapacity returns the capacity available when writing to the memory at WritePointer() onwards
    size_t WriteCapacity();
    // BytesWritten indicates that a number of bytes have been written at the WritePointer() location
    void BytesWritten(size_t nrBytes);

    // ReadPointer returns a pointer to the first position where existing data can be read
    char* ReadPointer();
    // ReadSize returns the number of bytes available when reading from the memory at ReadPointer() onwards
    size_t ReadSize();
    // BytesRead indicates that a number of bytes have been read from the BytesRead() location
    void BytesRead(size_t nrBytes);

private:
    // Parse the current buffer after a write operation
    void ParseBuffer();

private:
    std::unique_ptr<char[]> m_buffer;  // The buffer itself, always filled from the beginning
    size_t m_capacity;                 // Total capacity of the buffer (constructor argument)
    size_t m_size;                     // Number of bytes currently in use in the buffer, either parsed or unparsed
    size_t m_parsedSize;               // Number of bytes in the buffer that are already parsed
};

#endif // VTBUFFER_H
