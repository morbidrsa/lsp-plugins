/*
 * FileCharInputStream.h
 *
 *  Created on: 16 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_FILEREADER_H_
#define CORE_IO_FILEREADER_H_

#include <stdio.h>
#include <core/io/charset.h>
#include <core/io/Reader.h>
#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/types.h>

namespace lsp
{
    namespace io
    {
        class FileReader: public Reader
        {
            protected:
                uint8_t        *bBuf;
                lsp_wchar_t    *cBuf;
                size_t          bBufSize;
                size_t          bBufPos;
                size_t          cBufSize;
                size_t          cBufPos;
                File           *pFD;
                size_t          nFlags;
                LSPString       sLine;

#if defined(PLATFORM_WINDOWS)
                UINT            nCodePage;
#else
                iconv_t         hIconv;
#endif /* PLATFORM_WINDOWS */
    
            protected:
                void            do_destroy();
                status_t        init_encoding(const char *charset);
                status_t        fill_char_buf();

            public:
                FileReader();
                virtual ~FileReader();

            public:
                /** Wrap input reader around stdio file descriptor. When stream is closed, file descriptor
                 * keeps to be opened. Before attach currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param close close descriptor on reader close
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t wrap(FILE *fd, bool close, const char *charset = NULL);

                /** Wrap input reader around native file descriptor. When stream is closed, file descriptor
                 * keeps to be opened. Before attach currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param close close descriptor on reader close
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t wrap(lsp_fhandle_t fd, bool close, const char *charset = NULL);

                /** Wrap input reader around abstract file descriptor. When stream is closed, file descriptor
                 * keeps to be opened. Before attach currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param flags wrapping flags
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t wrap(File *fd, size_t flags, const char *charset = NULL);

                /** Open input stream associated with file. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param path file location path
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(const char *path, const char *charset = NULL);

                /** Open input stream associated with file. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param path file location path
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(const LSPString *path, const char *charset = NULL);

                /** Open input stream associated with file. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param path file location path
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(const Path *path, const char *charset = NULL);

                virtual ssize_t     read(lsp_wchar_t *dst, size_t count);

                virtual int         read();

                virtual status_t    read_line(LSPString *s, bool force = false);

                virtual ssize_t     skip(size_t count);

                virtual status_t    error();

                virtual status_t    close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_FILEREADER_H_ */
