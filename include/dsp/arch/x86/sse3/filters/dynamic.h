/*
 * dynamic.h
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_FILTERS_DYNAMIC_H_
#define DSP_ARCH_X86_SSE3_FILTERS_DYNAMIC_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    #ifdef ARCH_X86_64

    void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        size_t mask;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          4f")

            // Load delay buffer xmm6=d0[4], xmm7=d1[4], xmm8=d2[4], xmm9=d3[4]
            __ASM_EMIT("movaps      0x00(%[d]), %%xmm6")                    // xmm6     = d0
            __ASM_EMIT("movaps      0x10(%[d]), %%xmm8")                    // xmm8     = d2
            __ASM_EMIT("movaps      0x20(%[d]), %%xmm7")                    // xmm7     = d1
            __ASM_EMIT("movaps      0x30(%[d]), %%xmm9")                    // xmm9     = d3

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4]
            __ASM_EMIT("movaps      %[X_MASK], %%xmm14")                    // xmm14    = m[0] m[1] m[2] m[3] = ff 00 00 00
            __ASM_EMIT("xorps       %%xmm15, %%xmm15")                      // xmm15    = m[4] m[5] m[6] m[7] = 00 00 00 00
            __ASM_EMIT("mov         $1, %[mask]")                           // mask     = 1
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")                        // xmm1     = 0

            // Process first 7 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm12")                      // xmm12    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm13")                      // xmm13    = r
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1") // xmm1     = s*a0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm11")// xmm11    = r*i0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2") // xmm2     = s*a1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm12")// xmm12    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm8, %%xmm11")                       // xmm11    = r*i0+d2 = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3") // xmm3     = s*a2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm13")// xmm13    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm0")                       // xmm0     = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm10")                      // xmm10    = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4") // xmm4     = s2*b1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm0") // xmm0     = r2*j1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5") // xmm5     = s2*b2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm10")// xmm10    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm0, %%xmm12")                       // xmm12    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm10, %%xmm13")                      // xmm13    = r*i2 + r2*j2 = q2

            // Shift buffer
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm11, %%xmm11")               // xmm11    = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
            __ASM_EMIT("addps       %%xmm9, %%xmm12")                       // xmm12    = q1 + d3
            __ASM_EMIT("movss       %%xmm1, %%xmm11")                       // xmm11    = s2[3] r2[0] r2[1] r2[2]

            // Update delay only by mask, xmm14=MASK_LO, xmm15=MASK_HI
            __ASM_EMIT("andps       %%xmm14, %%xmm2")                       // xmm2     = (p1 + d1) & MASK_LO
            __ASM_EMIT("andps       %%xmm15, %%xmm12")                      // xmm12    = (q1 + d3) & MASK_HI
            __ASM_EMIT("movaps      %%xmm14, %%xmm4")                       // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm15, %%xmm5")                       // xmm5     = MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm8, %%xmm5")                        // xmm5     = d2 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = d0' = (p1 + d1) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm12, %%xmm5")                       // xmm5     = d2' = (q1 + d3) & MASK_HI | (d2 & ~MASK_HI)
            __ASM_EMIT("andps       %%xmm14, %%xmm3")                       // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm15, %%xmm13")                      // xmm13    = q2 & MASK_HI
            __ASM_EMIT("movaps      %%xmm14, %%xmm6")                       // xmm6     = MASK_LO
            __ASM_EMIT("movaps      %%xmm15, %%xmm8")                       // xmm8     = MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm6")                        // xmm6     = d1 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm9, %%xmm8")                        // xmm8     = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm3, %%xmm6")                        // xmm6     = d1' = (p2 & MASK_LO) | (d1 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm13, %%xmm8")                       // xmm8     = d3' = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm6, %%xmm7")                        // xmm7     = d1' = (p2 & MASK_LO) | (d1 & ~MASK_LO)
            __ASM_EMIT("movaps      %%xmm8, %%xmm9")                        // xmm9     = d3' = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0' = (p1 + d1) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("movaps      %%xmm5, %%xmm8")                        // xmm8     = d2' = (q1 + d3) & MASK_HI | (d2 & ~MASK_HI)

            // Repeat loop
            __ASM_EMIT("movaps      %%xmm14, %%xmm2")                       // xmm2     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("shufps      $0x93, %%xmm15, %%xmm15")               // xmm15    = m[7] m[4] m[5] m[6]
            __ASM_EMIT("shufps      $0x93, %%xmm14, %%xmm14")               // xmm14    = m[3] m[0] m[1] m[2]
            __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")            // mask     = (mask << 1) | 1
            __ASM_EMIT("movss       %%xmm14, %%xmm15")                      // xmm15    = m[3] m[4] m[5] m[6]
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")           // f++
            __ASM_EMIT("movss       %%xmm2, %%xmm14")                       // xmm14    = m[0] m[0] m[1] m[2]
            __ASM_EMIT("dec         %[count]")                              // count--
            __ASM_EMIT("jz          3f")                                    // jump to completion
            __ASM_EMIT("cmp         $0xff, %[mask]")
            __ASM_EMIT("jb          1b")

            // 8x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm12")                      // xmm12    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm13")                      // xmm13    = r
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1") // xmm1     = s*a0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm11")// xmm11    = r*i0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2") // xmm2     = s*a1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm12")// xmm12    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm8, %%xmm11")                       // xmm11    = r*i0+d2 = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3") // xmm3     = s*a2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm13")// xmm13    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm0")                       // xmm0     = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm10")                      // xmm10    = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4") // xmm4     = s2*b1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm0") // xmm0     = r2*j1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5") // xmm5     = s2*b2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm10")// xmm10    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm0, %%xmm12")                       // xmm12    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm10, %%xmm13")                      // xmm13    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm11, %%xmm11")               // xmm11    = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
            __ASM_EMIT("addps       %%xmm9, %%xmm12")                       // xmm12    = q1 + d3
            __ASM_EMIT("movss       %%xmm11, (%[dst])")                     // *dst     = r2[3]
            __ASM_EMIT("movss       %%xmm1, %%xmm11")                       // xmm11    = s2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d1' = p2
            __ASM_EMIT("movaps      %%xmm13, %%xmm9")                       // xmm9     = d3' = q2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0' = p1 + d1
            __ASM_EMIT("movaps      %%xmm12, %%xmm8")                       // xmm8     = d2' = q1 + d3

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")           // f++
            __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         2b")

            // Prepare last loop
            __ASM_EMIT("shufps      $0x93, %%xmm14, %%xmm14")               // xmm14    = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm15, %%xmm15")               // xmm15    = m[7] m[4] m[5] m[6]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
            __ASM_EMIT("movss       %%xmm14, %%xmm15")                      // xmm15    = m[3] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm2, %%xmm14")                       // xmm14    = 0 m[0] m[1] m[2]
    //                __ASM_EMIT("and         $0xff, %[mask]")                        // mask     = (mask << 1) & 0xff

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm12")                      // xmm12    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm11, %%xmm13")                      // xmm13    = r
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1") // xmm1     = s*a0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm11")// xmm11    = r*i0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2") // xmm2     = s*a1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm12")// xmm12    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm8, %%xmm11")                       // xmm11    = r*i0+d2 = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3") // xmm3     = s*a2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm13")// xmm13    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm0")                       // xmm0     = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm11, %%xmm10")                      // xmm10    = r2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4") // xmm4     = s2*b1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm0") // xmm0     = r2*j1
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5") // xmm5     = s2*b2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm10")// xmm10    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm0, %%xmm12")                       // xmm12    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm10, %%xmm13")                      // xmm13    = r*i2 + r2*j2 = q2

            // Shift buffer
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm11, %%xmm11")               // xmm11    = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
            __ASM_EMIT("addps       %%xmm9, %%xmm12")                       // xmm12    = q1 + d3
            __ASM_EMIT("movss       %%xmm11, (%[dst])")                     // *dst     = r2[3]
            __ASM_EMIT("movss       %%xmm1, %%xmm11")                       // xmm11    = s2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")           // f++
            __ASM_EMIT("add         $4, %[dst]")                            // dst      ++

            // Update delay only by mask, xmm14=MASK_LO, xmm15=MASK_HI
            __ASM_EMIT("andps       %%xmm14, %%xmm2")                       // xmm2     = (p1 + d1) & MASK_LO
            __ASM_EMIT("andps       %%xmm15, %%xmm12")                      // xmm12    = (q1 + d3) & MASK_HI
            __ASM_EMIT("movaps      %%xmm14, %%xmm4")                       // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm15, %%xmm5")                       // xmm5     = MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm8, %%xmm5")                        // xmm5     = d2 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = d0' = (p1 + d1) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm12, %%xmm5")                       // xmm5     = d2' = (q1 + d3) & MASK_HI | (d2 & ~MASK_HI)
            __ASM_EMIT("andps       %%xmm14, %%xmm3")                       // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm15, %%xmm13")                      // xmm13    = q2 & MASK_HI
            __ASM_EMIT("movaps      %%xmm14, %%xmm6")                       // xmm6     = MASK_LO
            __ASM_EMIT("movaps      %%xmm15, %%xmm8")                       // xmm8     = MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm6")                        // xmm6     = d1 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm9, %%xmm8")                        // xmm8     = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm3, %%xmm6")                        // xmm6     = d1' = (p2 & MASK_LO) | (d1 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm13, %%xmm8")                       // xmm8     = d3' = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm6, %%xmm7")                        // xmm7     = d1' = (p2 & MASK_LO) | (d1 & ~MASK_LO)
            __ASM_EMIT("movaps      %%xmm8, %%xmm9")                        // xmm9     = d3' = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0' = (p1 + d1) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("movaps      %%xmm5, %%xmm8")                        // xmm8     = d2' = (q1 + d3) & MASK_HI | (d2 & ~MASK_HI)

            // Repeat loop
            __ASM_EMIT("shufps      $0x93, %%xmm14, %%xmm14")               // xmm14    = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm15, %%xmm15")               // xmm15    = m[7] m[4] m[5] m[6]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
            __ASM_EMIT("movss       %%xmm14, %%xmm15")                      // xmm15    = m[3] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm2, %%xmm14")                       // xmm14    = 0 m[0] m[1] m[2]
            __ASM_EMIT("and         $0xff, %[mask]")                        // mask     = (mask << 1) & 0xff
            __ASM_EMIT("jnz         3b")                                    // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("movaps      %%xmm6, 0x00(%[d])")                    // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm8, 0x10(%[d])")                    // xmm8     = d2
            __ASM_EMIT("movaps      %%xmm7, 0x20(%[d])")                    // xmm7     = d1
            __ASM_EMIT("movaps      %%xmm9, 0x30(%[d])")                    // xmm9     = d3

            // Exit label
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [d] "+r" (d), [mask] "=&r" (mask), [f] "+r" (f), [count] "+r" (count)
            :
              [X_MASK] "m" (X_MASK0001)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    #endif /* ARCH_X86_64 */
}

#endif /* DSP_ARCH_X86_SSE3_FILTERS_DYNAMIC_H_ */