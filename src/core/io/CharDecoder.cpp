/*
 * CharDecoder.cpp
 *
 *  Created on: 7 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/CharDecoder.h>

namespace lsp
{
    namespace io
    {
        CharDecoder::CharDecoder()
        {
            bBuf            = NULL;
            bBufSize        = 0;
            bBufPos         = 0;
            bBufCapacity    = 0;

            cBuf            = NULL;
            cBufSize        = 0;
            cBufPos         = 0;
            cBufCapacity    = 0;

            #if defined(PLATFORM_WINDOWS)
                tBuf            = NULL;
                nCodePage       = 0;
            #else
                hIconv          = -1;
            #endif /* PLATFORM_WINDOWS */
        }
        
        CharDecoder::~CharDecoder()
        {
            close();
        }

        status_t CharDecoder::close()
        {
            if (bBuf != NULL)
            {
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
            }

            bBufSize        = 0;
            bBufPos         = 0;
            bBufCapacity    = 0;

            cBufSize        = 0;
            cBufPos         = 0;
            cBufCapacity    = 0;

            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
                tBuf            = NULL;
            #else
                if (hIconv != iconv_t(-1))
                {
                    iconv_close(hIconv);
                    hIconv      = iconv_t(-1);
                }
            #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        status_t CharDecoder::init(size_t buf_size, const char *charset)
        {
            if (bBuf == NULL)
            {
                size_t to_alloc = buf_size * sizeof(lsp_wchar_t);
                to_alloc       += buf_size * sizeof(uint8_t);

                #if defined(PLATFORM_WINDOWS)
                    to_alloc       += buf_size * sizeof(lsp_utf16_t) * 2;
                #endif /* PLATFORM_WINDOWS */

                uint8_t *ptr    = reinterpret_cast<uint8_t *>(::malloc(to_alloc));
                if (ptr == NULL)
                    return STATUS_NO_MEM;

                bBuf            = ptr;
                cBuf            = reinterpret_cast<lsp_wchar_t *>(&bBuf[buf_size]);
                #if defined(PLATFORM_WINDOWS)
                    tBuf            = reinterpret_cast<lsp_wchar_t *>(&cBuf[buf_size]);
                #endif /* PLATFORM_WINDOWS */
            }

            #if defined(PLATFORM_WINDOWS)
                ssize_t cp  = codepage_from_name(charset);
                if (cp < 0)
                {
                    free(bBuf);
                    bBuf        = NULL;
                    cBuf        = NULL;
                    tBuf        = NULL;
                    return set_error(STATUS_BAD_LOCALE);
                }
                nCodePage   = cp;
            #else
                hIconv      = init_iconv_to_wchar_t(charset);
                if (hIconv == iconv_t(-1))
                {
                    free(bBuf);
                    bBuf        = NULL;
                    cBuf        = NULL;
                    return STATUS_BAD_LOCALE;
                }
            #endif /* PLATFORM_WINDOWS */

            bBufSize        = 0;
            bBufPos         = 0;
            bBufCapacity    = buf_size;

            cBufSize        = 0;
            cBufPos         = 0;
            cBufCapacity    = buf_size;

            return STATUS_OK;
        }

        status_t CharDecoder::decode()
        {
            // TODO
        }

        void CharDecoder::cleanup_byte_buffer()
        {
            if (bBufPos <= 0)
                return;

            ssize_t left    = bBufSize - bBufPos;
            if (left > 0)
                ::memmove(bBuf, &bBuf[bBufPos], left);

            bBufSize        = left;
            bBufPos         = 0;
        }

        void CharDecoder::cleanup_char_buffer()
        {

        }

        ssize_t CharDecoder::fill(File *fd, ssize_t max = -1)
        {
            status_t res = STATUS_OK;

            while (cBufSize < cBufCapacity)
            {
                // Try to fill byte buffer with new data
                cleanup_byte_buffer();
                if (bBufSize < bBufCapacity)
                {
                    ssize_t n = fd->read(&bBuf[bBufSize], bBufCapacity - bBufSize);
                    if (n > 0)
                        bBufSize    += n;
                }

                // Perform decoding
                res     = decode();
                if (res != STATUS_OK)
                    return -res;
            }

            return cBufCapacity;
        }

        ssize_t CharDecoder::fill(IInputStream *is, ssize_t max = -1)
        {

        }

        lsp_swchar_t CharDecoder::fetch()
        {

        }

        ssize_t CharDecoder::fetch(lsp_wchar_t *buf, size_t count)
        {

        }

        ssize_t CharDecoder::flush(Writer *wr, size_t count)
        {

        }
    
    } /* namespace io */
} /* namespace lsp */
