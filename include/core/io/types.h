/*
 * types.h
 *
 *  Created on: 7 мар. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_TYPES_H_
#define INCLUDE_CORE_IO_TYPES_H_

namespace lsp
{
    namespace io
    {
        enum lsp_wrap_flatgs_t
        {
            WRAP_CLOSE      = 1 << 0,
            WRAP_DELETE     = 1 << 1
        };
    }
}

#endif /* INCLUDE_CORE_IO_TYPES_H_ */
