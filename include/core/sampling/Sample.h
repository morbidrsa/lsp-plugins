/*
 * Sample.h
 *
 *  Created on: 12 мая 2017 г.
 *      Author: sadko
 */

#ifndef CORE_SAMPLING_SAMPLE_H_
#define CORE_SAMPLING_SAMPLE_H_

#include <core/types.h>

namespace lsp
{
    class Sample
    {
        private:
            float      *vBuffer;
            size_t      nLength;
            size_t      nMaxLength;
            size_t      nChannels;

        public:
            Sample();
            ~Sample();

        public:
            inline bool valid() const { return (vBuffer != NULL) && (nChannels > 0) && (nLength > 0) && (nMaxLength > 0); }
            inline size_t length() const { return nLength; }
            inline size_t max_length() const { return nMaxLength; }
            inline float *getBuffer(size_t channel) { return &vBuffer[nMaxLength * channel]; }
            inline float *getBuffer(size_t channel, size_t offset) { return &vBuffer[nMaxLength * channel + offset]; }
            inline size_t channels() const { return nChannels; };

            /** Set length of sample
             *
             * @param length length to set
             * @return actual length of the sample
             */
            inline size_t setLength(size_t length)
            {
                if (length > nMaxLength)
                    length = nMaxLength;
                return nLength = length;
            }

            /** Extend length of sample
             *
             * @param length length to extend
             * @return actual length of the sample
             */
            inline size_t extend(size_t length)
            {
                if (length > nMaxLength)
                    length = nMaxLength;
                if (nLength < length)
                    nLength = length;
                return nLength;
            }

            /** Clear sample (make length equal to zero
             *
             */
            inline void clear()
            {
                nLength     = 0;
            }

            /** Initialize sample, all previously allocated data will be lost
             *
             * @param channels number of channels
             * @param max_length maximum possible sample length
             * @param length initial sample length
             * @return true if data was successful allocated
             */
            bool init(size_t channels, size_t max_length, size_t length = 0);

            /** Resize sample, all previously allocated data will be kept
             *
             * @param channels number of channels
             * @param max_length maximum possible sample length
             * @param length initial sample length
             * @return if data was successful resized
             */
            bool resize(size_t channels, size_t max_length, size_t length = 0);

            /** Drop sample contents
             *
             */
            void destroy();
    };

} /* namespace lsp */

#endif /* CORE_SAMPLING_SAMPLE_H_ */
