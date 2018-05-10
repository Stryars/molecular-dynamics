// Copyright (c) 2014, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/* Author: Jan Winkler */

#include <cmath>

#include "include/hsv2rgb.h"

// Every variable is is range [0, 1] except fHin, which is in range [0, 360].
void HSVtoRGB(float fHin, float fSin, float fVin,
    float* fRout, float* fGout, float* fBout) {
  float fC = fVin * fSin;
  float fHPrime = fmod(fHin / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fVin - fC;

  if (0 <= fHPrime && fHPrime < 1) {
    *fRout = fC;
    *fGout = fX;
    *fBout = 0;
  } else if (1 <= fHPrime && fHPrime < 2) {
    *fRout = fX;
    *fGout = fC;
    *fBout = 0;
  } else if (2 <= fHPrime && fHPrime < 3) {
    *fRout = 0;
    *fGout = fC;
    *fBout = fX;
  } else if (3 <= fHPrime && fHPrime < 4) {
    *fRout = 0;
    *fGout = fX;
    *fBout = fC;
  } else if (4 <= fHPrime && fHPrime < 5) {
    *fRout = fX;
    *fGout = 0;
    *fBout = fC;
  } else if (5 <= fHPrime && fHPrime < 6) {
    *fRout = fC;
    *fGout = 0;
    *fBout = fX;
  } else {
    *fRout = 0;
    *fGout = 0;
    *fBout = 0;
  }

  *fRout += fM;
  *fGout += fM;
  *fBout += fM;
}
