/*
 * CharDecoder.h
 *
 *  Created on: 7 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_CHARDECODER_H_
#define CORE_IO_CHARDECODER_H_

#include <core/types.h>
#include <core/io/charset.h>
#include <core/io/File.h>
#include <core/io/IInputStream.h>
#include <core/io/IOutputStream.h>
#include <core/io/Writer.h>

namespace lsp
{
    namespace io
    {
        class CharDecoder
        {
            protected:
                uint8_t        *bBuf;
                size_t          bBufSize;
                size_t          bBufPos;
                size_t          bBufCapacity;

                lsp_wchar_t    *cBuf;
                size_t          cBufSize;
                size_t          cBufPos;
                size_t          cBufCapacity;

#if defined(PLATFORM_WINDOWS)
                lsp_utf16_t    *tBuf;       // Temporary buffer

                UINT            nCodePage;
#else
                iconv_t         hIconv;
#endif /* PLATFORM_WINDOWS */

            private:
                CharDecoder & operator = (const CharDecoder &src);  // Deny copying

            protected:
                inline void     cleanup_byte_buffer();
                inline void     cleanup_char_buffer();
                status_t        decode();

            public:
                explicit CharDecoder();
                ~CharDecoder();

            public:
                /**
                 * Initialize character set decoder
                 * @param buf_size buffer size
                 * @param charset source character set
                 * @return status of operation
                 */
                status_t        init(size_t buf_size, const char *charset = NULL);

                /**
                 * Destroy character set decoder
                 * @return status of operation
                 */
                status_t        close();

                /**
                 * Fill character set decoder with new data from file
                 * @param fd file descriptor
                 * @param max maximum amount of bytes to read, negative value = unlimited
                 * @return number of decoded characters in buffer or error code
                 */
                ssize_t         fill(File *fd, ssize_t max = -1);

                /**
                 * Fill character set decoder with new data from input stream
                 * @param is input stream
                 * @param max maximum amount of bytes to read, negative value = unlimited
                 * @return number of decoded characters in buffer or error code
                 */
                ssize_t         fill(IInputStream *is, ssize_t max = -1);

                /**
                 * Fetch one single character from buffer
                 * @return single character value or error code
                 */
                lsp_swchar_t    fetch();

                /**
                 * Fetch characters from internal buffer
                 * @param buf target buffer to perform fetch
                 * @param count number of characters to fetch
                 * @return number of characters fetched
                 */
                ssize_t         fetch(lsp_wchar_t *buf, size_t count);

                /**
                 * Output internal buffer contents to the writer
                 * @param wr writer
                 * @param count number of code points to write
                 * @return number of characters written or negative error code
                 */
                ssize_t         flush(Writer *wr, size_t count);

                /**
                 * Get number of chararacters available in internal buffer
                 * @return
                 */
                inline size_t   available() const { return cBufSize - cBufPos; };
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_CHARDECODER_H_ */
